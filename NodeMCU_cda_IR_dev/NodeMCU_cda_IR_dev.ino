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
/* 
 * TODO
 * Sometimes PubNubDefs.h throws a "Connection reset in bottom half"
 * publishing a message to PubNub
 */
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
const char* ssid = "rede_Mar";
const char* pass = "34753475";
///////////////////////////////////////////////////////////////////

////////////////Propriedades do PubNub/////////////////////////////
#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>

const static char pubkey[] = "pub-c-6ce39655-3209-4cea-acc9-543751fe4e55";
const static char subkey[] = "sub-c-6e52b6c2-8e6b-11e8-b7a4-ce74daf54d52";
const static char canal_de_dados[] = "dados_cisterna1";
const static char canal_de_comandos[] = "comando_cisterna1";
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


/////////////////Propriedades do sensor sharp//////////////////////
#define SHARP_PIN A0 // Pino do sensor infravermelho Sharp GP2Y0A21YK0F

//#define SHARP_HIGH 1.30
//#define SHARP_HIGH 1.70
#define SHARP_HIGH 0.58

//For Sharp GP2Y0A02YK0F
/*
#define INTERVALS 7
const float abscissas[] = {0.3, 0.35, 0.47, 0.57, 0.91, 1.48, 1.96, 2.47};
const float inclinacao[] = {-200, -166.67, -120, -117.64, -65.93, -20.83, -19.60};
const int ordenadas[] = {150, 140, 120, 100, 60, 40, 30, 20};
*/
// For Sharp GP2Y0A21YK0F
#define INTERVALS 8
const float abscissas[] = {0.34, 0.49, 0.62, 0.65, 0.83, 0.97, 1.29, 1.59, 2.38};
const float inclinacao[] = {-140, -100, -70, -55.55, -35.71, -15.62, -16.67, -6.32};
const int ordenadas[] = {83, 62, 49, 40, 30, 25, 20, 15, 10};
//83 0.34
//62 0.49
//54 0.54
//49 0.62
//43 0.69
//40 0.65
//30 0.83
//25 0.97
//20 1.29 ou 1.31
//15 1.59
//10 2.38

//80 0.45 ou 0.33
//73 0.5 ou outra coisa
//61 0.59
//
///////////////////////////////////////////////////////////////////


//peso do cano de casa: 113g

/////////////////Constantes da cisterna////////////////////////////
// Dados da cisterna na Amorim Lima
//Capacidade: 2500 L = 2.5 m^3
//Altura: 172 cm = 1.72 m
//Diâmetro: 157 cm
//Área da base * Altura = Capacidade => Área da base = 2.5/1.72 = 1.4534 m^2

//caixa da água
//https://www.fortlev.com.br/produto/caixa-dagua-de-polietileno-2/
//Capacidade - litros
//A - Altura com tampa
//B - Altura sem tampa
//C - Diâmetro com tampa
//D - Diâmetro sem tampa
//E - Diâmetro da base
//500   0,72  0,58  1,24  1,22  0,95
//Altura sem tampa 0.58m
//Diamtro da base 0.95m


//numero identificador da cisterna
#define ID_CISTERNA 1

//altura em metros: 1.72 m = 172 cm
#define CISTERNA_HIGH 0.54m
//area da base da cisterna (em metros quadrados)
#define CISTERNA_BASE_AREA 0.7089

//capacidade da cisterna (em metros cubicos)
#define CISTERNA_CAPACITY 0.411104544

//distancia do sensor ateh o nivel do ladrao (em metros)
//#define OFFSET 0.3

///////////////////////////////////////////////////////////////////


#define ACTUATOR_PIN D6

//long inicio = millis();

void setup() {
    Serial.begin(115200);
    //D(Serial.println("Iniciando celula de carga ...");)

    pinMode(SHARP_PIN, INPUT);
    
    pinMode(ACTUATOR_PIN, OUTPUT);
  
//    pwm.setup(ACTUATOR_PIN, 100, 512);
    
    D(Serial.println("Attempting to connect to WPA network...");)
    wifiConnection();
    
    udp.begin(localPort);
    
    PubNub.begin(pubkey, subkey);
    
}

void loop() {

    //if (false) {
    if (WiFi.status() != WL_CONNECTED) {
        wifiConnection();
    } else {
        float voltage = read_sharp();
        float distance = calculate_distance(voltage);
        D(Serial.print("distance: ");)
        D(Serial.println(distance);)
        if (!distance) {
            delay(500);
            return;
        }
        
        // O volume de agua eh o volume total menos a distancia do sensor ateh a superficie da agua.
        //float litros = (CISTERNA_CAPACITY - ((CISTERNA_HIGH - distance) * CISTERNA_BASE_AREA))*1000; // volume de agua em litros
        float litros = ((SHARP_HIGH - distance) * CISTERNA_BASE_AREA)*1000; // volume de agua em litros
        //float litros = voltage;
        D(Serial.print("litros: ");)
        D(Serial.println(litros);)
        //litros = litros > 0 ? litros : 0;

        
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
        
        //checkPubNubHistory();
        
    }
    //delay(3000);  
    delay(60000);
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
 * Tira 9 medidas do sensor e toma a mediana para obter um valor 
 * mais confiável.
 */
float read_sharp() {
    D(Serial.println("Leitura do sensor sharp: ");)
    int sharp_readings[9];
    for (int i = 0; i < 9; i++) {
        sharp_readings[i] = 10000; // O certo seria usar INT_MAX no lugar de 10000
        int sharp_reading = analogRead(SHARP_PIN);
        V(Serial.println(sharp_reading);)
        int j = 0;
        while (sharp_readings[j] < sharp_reading) j++;
        for (int k = i;k > j; k--) sharp_readings[k] = sharp_readings[k-1];
        sharp_readings[j] = sharp_reading;
        delay(200);
    }
    D(Serial.print("sharp_readings median: ");)
    D(Serial.println(sharp_readings[4]);)
    float sensor_voltage = (1.0*sharp_readings[4]/1024) * 3.0;
    D(Serial.print("voltage: ");)
    D(Serial.println(sensor_voltage);)
    return sensor_voltage;
}

/*
 * Calcula e retora a distancia em metros
 */
float calculate_distance(float voltage) {
    // Leitura incorreta
    if (voltage < abscissas[0] || voltage > abscissas[INTERVALS]) {
        D(Serial.print("bad sensor reading: ");)
        D(Serial.println(voltage);)
        return 0;
    }
    int i;
    for (i = 1; i < INTERVALS; i++) 
        if (voltage < abscissas[i]) break;
//    D(Serial.print(ordenadas[i]);)
//    D(Serial.print(" + ");)
//    D(Serial.print(inclinacao[i]);)
//    D(Serial.print("*(");)
//    D(Serial.print(leitura);)
//    D(Serial.print(" - ");)
//    D(Serial.print(abscissas[i]);)
//    D(Serial.print(")");)
//    D(Serial.println();)
    //float distancia = ordenadas[i] + inclinacao[i]*(voltage - abscissas[i]); //em centimetros
    
    float distance = ordenadas[i] + inclinacao[i-1]*(voltage - abscissas[i]);
    //D(Serial.print("calculated distance: ");)
    //D(Serial.println(distance);)
    return distance / 100;
}

void publishToPubNub(String json_string) {
    V(Serial.println("PublishToPubNub");)
    char json_char_array[100];
    json_string.toCharArray(json_char_array, 100);
    D(Serial.println(json_char_array);)
    V(Serial.println("Publishing to PubNub ...");)
    WiFiClient *client;
    client = PubNub.publish(canal_de_dados, json_char_array);
    delay(1500);
    if (!client) {
        Serial.println("publishing error at sketch");
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

