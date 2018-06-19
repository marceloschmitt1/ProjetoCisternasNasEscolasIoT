/*
  PubNub over NodeMCU
  
  This sample client will publish raw (string) PubNub messages,
  subscribe to receive data, and fetch messages from history.
  This sketch should work with ESP12 like chips like ESP12E,
  ESP32, NodeMCU chips with ESP12. 
  Tested with NodeMCU (ESP12E) Amica.
  
  created May 31, 2017
  by Marcelo Schmitt
*/
  
#include <SPI.h>
#include <ESP8266WiFi.h>

#define DEBUG

#ifdef DEBUG
    #define D(X) X
    #define PUBNUB_DEBUG
#else
    #define D(X)
#endif

#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>
  
int status = WL_IDLE_STATUS;                // the Wifi radio's status

// Wireless settings
const char* ssid = "wifi-ssid";
const char* pass = "wifi-password";

// PubNub settings
const static char pubkey[] = "demo";         // your publish key 
const static char subkey[] = "demo";         // your subscribe key
const static char channel[] = "hello_world"; // channel to use


//WiFiClient client;

void setup() {
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
      
        WiFiClient *client;
    
        char *msg = "\"shoriuken!\"";
        par = !par;
        if (par) msg = "\"haduken\"";
      
        client = PubNub.publish(channel, msg);
        if (!client) {
            Serial.println("publishing error");
            delay(1000);
            return;
        }
        if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success) {
            Serial.print("Got HTTP status code error from PubNub, class: ");
            Serial.print(PubNub.get_last_http_status_code_class(), DEC);
        }
        while (client->available()) {
            Serial.write(client->read());
        }
        client->stop();
        Serial.println("---");
      
  
        Serial.println("waiting for a message (subscribe)");
        PubSubClient *pclient = PubNub.subscribe(channel);
        if (!pclient) {
            Serial.println("subscription error");
            delay(1000);
            return;
        }
        while (pclient->wait_for_data()) {
          char c = pclient->read();
          Serial.print(c);
        }
        pclient->stop();
        Serial.println();
  
        
        Serial.println("retrieving message history");
        client = PubNub.history(channel);
        if (!client) {
            Serial.println("history error");
            delay(1000);
            return;
        }
        while (client->connected()) {
            while (client->connected() && !client->available()) ; // wait
            char c = client->read();
            Serial.print(c);
        }
        client->stop();
        Serial.println();
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
    D(Serial.print("IP address: ");)
    D(Serial.println(WiFi.localIP());)
}

