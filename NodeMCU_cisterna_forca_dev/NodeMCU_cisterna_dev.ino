/*
  @author: Marcelo Schmitt

  https://github.com/deusanyjunior
  https://github.com/deusanyjunior/interscityroom8/blob/master/HTTPPostRequest.ino
*/
  
#include <SPI.h>
#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

#include <ArduinoJson.h>

#define DEBUG
//#define VERBOSE

#ifdef DEBUG
    #define D(X) X
    #ifdef VERBOSE
        #define PUBNUB_DEBUG
        #define V(X) X
    #else
        #define V(X)
    #endif
#else
    #define D(X)
    #define V(X)
#endif

//////////////////Propriedades Wireless////////////////////////////
const char* ssid = "MarDroid";
const char* pass = "mirtero1";
///////////////////////////////////////////////////////////////////

////////////////Propriedades do PubNub/////////////////////////////
#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>

const static char pubkey[] = "pub-c-6ce39655-3209-4cea-acc9-543751fe4e55";
const static char subkey[] = "sub-c-6e52b6c2-8e6b-11e8-b7a4-ce74daf54d52";
const static char canal_de_dados[] = "dados_cisterna";
const static char canal_de_comandos[] = "comando_cisterna";
///////////////////////////////////////////////////////////////////

/////////////////Recursos NTP//////////////////////////////////////
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServerIP; // time.nist.gov NTP server address //0.br.pool.ntp.org
const char* ntpServerName = "0.br.pool.ntp.org";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

// A time instance
time_t rawTime;
time_t startTime;
time_t now;
struct tm ts;
char timeBuffer[80];
bool timeDefined = false;
///////////////////////////////////////////////////////////////////

/////////////////Propriedades da celula de carga///////////////////
//#define  ADDO  0    //Data Out //versao para ESP01
//#define  ADSK  2    //SCK //versao para ESP01
#define  ADDO  D0    //Data Out //versao para nodeMCU (ESP12)
#define  ADSK  D2    //SCK //versao para nodeMCU (ESP12)

//Este sktech utiliza uma versão adaptada da biblioteca HX711_ADC
#include <HX711_ADC.h>

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell(D0, D2);

#define calibration_factor 1696.0 //calibrado para devolver medidas em gramas

long last_reading = millis();
//float strength = 0.0;
///////////////////////////////////////////////////////////////////


//peso do cano de casa: 113g

/////////////////Constantes da cisterna////////////////////////////
//numero identificador da cisterna
#define ID_CISTERNA 1

//area da base da cisterna (em metros quadrados)
#define BASE_AREA 0.46152

//peso do cano (em newtons)
#define PIPE_WEIGHT 4 // testando com 400g = 0.4kg = 4N

//diâmetro do cano (em metros)
#define PIPE_DIAMETER 0.04 //40mm = 0.04 metro

///////////////////////////////////////////////////////////////////


#define ACTUATOR_PIN D6

long inicio = millis();

void setup() {
    Serial.begin(115200);
    D(Serial.println("Iniciando celula de carga ...");)

    //setup da celula de carga
    LoadCell.begin();
    long stabilisingtime = 5000; // tare preciscion can be improved by adding a few seconds of stabilising time
    LoadCell.start(stabilisingtime);
    LoadCell.setCalFactor(calibration_factor); // user set calibration factor (float)
    D(Serial.println("Startup + tare is complete");)
    D(Serial.println();)
    //
    
    pinMode(ACTUATOR_PIN, OUTPUT);
  
//    pwm.setup(ACTUATOR_PIN, 100, 512);
    // attempt to connect using WPA2 encryption:
    D(Serial.println("Attempting to connect to WPA network...");)
    wifiConnection();
    
    udp.begin(localPort);
    
    PubNub.begin(pubkey, subkey);

    randomSeed(analogRead(A0));
}

void loop() {

    if (WiFi.status() != WL_CONNECTED) {
        wifiConnection();
    } else {
        //Get load cell reading
        float strength = read_load_cell();

        //float litros = strength;
        float litros = calculate_water_volume(strength) * 1000;
        D(Serial.print("litros: ");)
        D(Serial.println(litros);)
        
        // Get current time from NTP
        if (!timeDefined) {
          if (!getTimeNTP()) return; // Get time from NTP servers
          timeDefined = true;
        }
        // set current time based on statup time
        time (&rawTime);
        now = rawTime + startTime;
        ts = *localtime(&now);
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%dT%H:%M:%S", &ts);

        D(Serial.println(timeBuffer);)
        String json_string = "{\"cisterna\":" + String(ID_CISTERNA) + "," + "\"time_stamp\":\"" + String(timeBuffer) + "\"," + "\"eon\":{\"litros\":" + String(litros) + "}}";
        
        publishToPubNub(json_string);
        
        checkPubNubHistory();
    }
    delay(1000);
  
}

// Method to (re)connect to WiFi network
void wifiConnection() {
    D(Serial.print("Connecting to ");)
    D(Serial.println(ssid);)
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      D(Serial.print(".");)
    }
    D(Serial.println("");)
    D(Serial.println("WiFi connected");)
    D(Serial.print("Endereco IP: ");)
    D(Serial.println(WiFi.localIP());)
}

/* 
 *  Recebe a forca em newtons e devolve o volume de agua (em metros cubicos) pela seguinte formula
 *  Litros = B*h = B * (Fr + Pc) / (g * PI * (d^2 / 4) * ro) 
 *  onde
 *  B: area da base do recipiente
 *  Fr: forca sobre o sensor
 *  Pc: peso do cano
 *  g: aceleracao gravitacional = 9.8 m/s^2
 *  PI: constante matemática = 3.1415
 *  d: diametro do cano
 *  ro: densidade da agua = 997kg/m^3
 */
float calculate_water_volume(float strength) {
    D(float volume = BASE_AREA * (strength + PIPE_WEIGHT) / (9.8 * 3.1415 * (PIPE_DIAMETER*PIPE_DIAMETER / 4) * 997);)
    D(Serial.print("volume: ");)
    D(Serial.println(volume);)
    return BASE_AREA * (strength + PIPE_WEIGHT) / (9.8 * 3.1415 * (PIPE_DIAMETER*PIPE_DIAMETER / 4) * 997);
}

/*
 * Devolve a leitura de forca mensurada pela celula de carga. 
 * Ajuste o calibration_factor para obter leituras na unidade de medida desejada.
 */
float read_load_cell() {
    //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
    //longer delay in scetch will reduce effective sample rate (be carefull with delay() in loop)
    LoadCell.update();
    //get smoothed value from data set + current calibration factor
    if (millis() > last_reading + 250) {
      float reading = LoadCell.getData();
      D(Serial.print("load cell reading: ");)
      D(Serial.print(reading);)
      Serial.println();
      last_reading = millis();
      //1000g = 1kg = 10N => Xg = 0,01N
      return reading / 100; //dividindo por 100 para retornar o peso em newtons
    }
    //receive from serial terminal
    if (Serial.available() > 0) {
      float i;
      char inByte = Serial.read();
      if (inByte == 't') LoadCell.tareNoDelay();
    }
    //check if last tare operation is complete
    if (LoadCell.getTareStatus() == true) {
      Serial.println("Tare complete");
    }
}

void publishToPubNub(String json_string) {
    V(Serial.println("PublishToPubNub");)
    char json_char_array[100];
    json_string.toCharArray(json_char_array, 100);
    D(Serial.println(json_char_array);)
    V(Serial.println("Publishing to PubNub ...");)
    WiFiClient *client;
    client = PubNub.publish(canal_de_dados, json_char_array);
    if (!client) {
        Serial.println("publishing error");
        delay(1000);
        return;
    }
    if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success) {
        Serial.print("Got HTTP status code error from PubNub, class: ");
        Serial.print(PubNub.get_last_http_status_code_class(), DEC);
    }
    V(Serial.println("Publich responce");)
    V(while (client->available()) {)
        V(Serial.write(client->read());)
    V(})
    V(Serial.println();)
    client->stop();
}

void checkPubNubHistory() {
    V(Serial.println("Retrieving message history");)
    WiFiClient *client;
    client = PubNub.history(canal_de_comandos, 1);
    if (!client) {
        Serial.println("PubNub history error");
        return;
    }
    String command_json = "";
    while (client->connected()) {
        while (client->connected() && !client->available()) ; // wait
        char c = client->read();
        command_json += c;
    }
    client->stop();
    V(Serial.print("command_json: ");)
    V(Serial.print(command_json);)
    V(Serial.println();)
    command_json.replace("[", "");
    command_json.replace("]", "");
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(command_json);
    if (!root.success()) {
      D(Serial.println("parseObject() failed");)
    }
    const char* command = root["comando"];
    if (String(command).equals("ativa")) {
        analogWrite(ACTUATOR_PIN, 800);
    } else if (String(command).equals("desativa")) {
        analogWrite(ACTUATOR_PIN, 100);
    }
}

// Method to get the NTP response and backup the *startTime*
bool getTimeNTP() {
    Serial.println("Get time from NTP servers");
    //get a random server from the pool
    WiFi.hostByName(ntpServerName, timeServerIP);
  
    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(1000);
  
    int cb = udp.parsePacket();
    if (!cb) {
        Serial.println("no NTP packet yet");
        delay(10000);
        return false;
    } else {
        Serial.print("NTP packet received, length=");
        Serial.println(cb);
        // We've received a packet, read the data from it
        udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    
        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
    
        // now convert NTP time into everyday time:
        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        const unsigned long seventyYears = 2208988800UL;
    
        // subtract seventy years:
        //unsigned long epoch = secsSince1900 - seventyYears;

        //subtarct 3 hours to match São Paulo - Brazil timezone
        const long timeZoneOffset = -10800L; //3 hours in seconds
        now = secsSince1900 - seventyYears + timeZoneOffset ;
        
        // backup time after seventy years
        time( &rawTime );
        startTime = now - rawTime;
    
        Serial.println("Time defined");
    
        return true;
    }
}

// Method to send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
  
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

