/*
  @author: Marcelo Schmitt
*/
  
#include <SPI.h>
#include <ESP8266WiFi.h>

#define DEBUG

#ifdef DEBUG
    #define D(X) X
//    #define PUBNUB_DEBUG
#else
    #define D(X)
#endif

#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>

#define SERVO_PIN D6
#define SHARP_PIN A0

#include <ArduinoJson.h>

int status = WL_IDLE_STATUS;                // the Wifi radio's status

// Wireless settings
const char* ssid = "MarDroid";
const char* pass = "mirtero1";

// PubNub settings
const static char pubkey[] = "pub-c-3ef4941c-5a95-497a-b7bf-ed6d0823b186";         // your publish key 
const static char subkey[] = "sub-c-cd8b8d8e-6941-11e8-98cb-067913ebee63";         // your subscribe key
//const static char channel[] = "cisterna"; // channel to use
const static char dados_channel[] = "dados_cisterna"; // channel to use
const static char comando_channel[] = "comando_cisterna"; 

// Constantes de calibracao e mensuracao
#define INTERVALOS 6
const float abscissas[] = {0.3, 0.35, 0.47, 0.57, 1.48, 1.96, 2.47};
const float inclinacao[] = {-200, -166,67, -200, -65,93, -20,83, -19,60};
const int ordenadas[] = {150, 140, 120, 100, 40, 30, 20};

long inicio = millis();

void setup() {
  //pinMode(SHARP_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);

//  pwm.setup(SERVO_PIN, 100, 512);
  
  Serial.begin(115200);
  D(Serial.println("Serial set up");)

  // attempt to connect using WPA2 encryption:
  D(Serial.println("Attempting to connect to WPA network...");)

  wifiConnection();

  PubNub.begin(pubkey, subkey);
}

bool par = true;

void loop() {

    if (WiFi.status() != WL_CONNECTED) {
        wifiConnection();
    } else {
        //Leitura do sensor
//        float voltagem_sensor = (1.0*analogRead(SHARP_PIN)/1024) * 5;
        int leitura_analogica = analogRead(SHARP_PIN);
        float voltagem_sensor = (1.0*leitura_analogica/1024) * 3.0;
        //float voltagem_node = map(voltagem_sensor, 0.0, 5.0, 0.0, 1.0);
        //Aplicação de um filtro: 
        //Se a voltagem medida pelo sensor for menor que 0.4V ~ 130cm 
        //então a leitura está errada pois da base até o topo da cisterna 
        //há apenas 130cm de altura.
//        if (voltagem_sensor < 0.4) {
//            //leitura inconsistente
//            Serial.println("leitura maior que 130cm");
//            return;
//        }
        //Aplicação de um filtro:
        //Se a voltagem medida pelo snsor for maior que 2.5V ~ 20cm 
        //então a leitura está errada pois a água dentro da cisterna 
        //deve chegar a no máximo 110cm de altura (no máximo a 20cm 
        //de distância de sensor).
//        if (voltagem_sensor > 2.5) { //fazer um #define calcular os filtros com base na distancia
//            //leitura inconsistente
//            Serial.println("leitura menor que 20cm");
//            return;
//        }

        D(Serial.print("leitura_analogica: ");)
        D(Serial.print(leitura_analogica);)
        D(Serial.print(" voltagem_sensor: ");)
        D(Serial.print(voltagem_sensor);)
//        D(Serial.print(" voltagem_node ");)
//        D(Serial.print(voltagem_node);)
        float distancia = calcula_distancia(voltagem_sensor);
//        float distancia = calcula_distancia(voltagem_node);
        D(Serial.print(" distancia: ");)
        D(Serial.println(distancia);)

        //Time stamp provisorio
        String str_dist = "[{\"tempo\":\"" + String((1.0*millis()-inicio)/1000) + "\"}," + "{\"distancia\":\"" + String(distancia) + "\"}]";
        char msg[70];
        str_dist.toCharArray(msg, 70);
        Serial.println(msg);
        
        //Envio da mensagem para a nuvem (usando o servico do PubNub)
        WiFiClient *client;        
        client = PubNub.publish(dados_channel, msg);
        if (!client) {
            Serial.println("publishing error");
            delay(1000);
            return;
        }
        if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success) {
            Serial.print("Got HTTP status code error from PubNub, class: ");
            Serial.print(PubNub.get_last_http_status_code_class(), DEC);
        }
//        while (client->available()) {
//            D(Serial.write(client->read());)
//        }
        client->stop();
//        Serial.println("---");

        Serial.println();
        
        // Parte de controle da cistenar (despejo de produto de limpeza)
        String ordem = "";
        
//        Serial.println("retrieving message history");
        client = PubNub.history(comando_channel, 1);
        if (!client) {
            Serial.println("history error");
            delay(1000);
            return;
        }
        while (client->connected()) {
            while (client->connected() && !client->available()) ; // wait
            char c = client->read();
            ordem += c;
//            Serial.print(c);
        }
//        Serial.print("ordem: ");
//        Serial.print(ordem);
        client->stop();
//        Serial.println();

        ordem.replace("[", "");
        ordem.replace("]", "");
//        Serial.print("ordem: ");
//        Serial.print(ordem);
//        Serial.println();

        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(ordem);
        if (!root.success()) {
          Serial.println("parseObject() failed");
        }
        const char* comando = root["comando"];
//        Serial.print("comando: ");
//        Serial.print(comando);
//        Serial.println();

        if (String(comando).equals("ativa")) {
            analogWrite(SERVO_PIN, 100);  
        } else if (String(comando).equals("desativa")) {
            analogWrite(SERVO_PIN, 800);
        }
        
    }
    delay(1000);
  
}

float calcula_distancia(float leitura) {
    int i;
    for (i = 0; i < INTERVALOS+1; i++) 
        if (leitura < abscissas[i]) break;

//    D(Serial.print(ordenadas[i]);)
//    D(Serial.print(" + ");)
//    D(Serial.print(inclinacao[i]);)
//    D(Serial.print("*(");)
//    D(Serial.print(leitura);)
//    D(Serial.print(" - ");)
//    D(Serial.print(abscissas[i]);)
//    D(Serial.print(")");)
//    D(Serial.println();)
    float distancia = ordenadas[i] + inclinacao[i]*(leitura - abscissas[i]);
    return distancia;  
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
    D(Serial.print("IP address: ");)
    D(Serial.println(WiFi.localIP());)
}

