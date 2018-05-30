/*
  PubNub over WiFi Example using WiFi Shield 101 

  This sample client will publish raw (string) PubNub messages from
  boards like Arduino Uno and Zero with WiFi Shield 101
  
  See https://www.arduino.cc/en/Reference/WiFi101 for more info 
  
  created Dec 17, 2015
  by Tomomi Imura
*/
  
#include <SPI.h>

//const char* host = "http://pubsub.pubnub.com";
//const int httpPort = 80;
//
//const char* d_origin = "pubsub.pubnub.com";

//#include <WiFi101.h>
#include <ESP8266WiFi.h>
#define PubNub_BASE_CLIENT WiFiClient
#define PUBNUB_DEBUG
#include <PubNub.h>
  
//static char ssid = "MarDroid";   // your network SSID (name)
//static char pass = "mirtero1";  // your network password
int status = WL_IDLE_STATUS;                // the Wifi radio's status

// Wireless settings
const char* ssid = "ssid";
const char* pass = "senha";


const static char pubkey[] = "demo";         // your publish key 
const static char subkey[] = "demo";         // your subscribe key
const static char channel[] = "hello_world"; // channel to use


WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Serial set up");

  // attempt to connect using WPA2 encryption:
  Serial.println("Attempting to connect to WPA network...");
//  status = WiFi.begin(ssid, pass);

  Serial.println(ssid);
  Serial.println(pass);


  wifiConnection();

  PubNub.begin(pubkey, subkey);
  
}

bool par = false;

void loop() {
//    Serial.println("loop");
    /* Publish */

    if (WiFi.status() != WL_CONNECTED) {
        wifiConnection();
    } else {
      delay(1000);
        
      WiFiClient *client;
  
//      char msg[] = "\"Yo!\"";
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
  
}


// Method to (re)connect to WiFi network
void wifiConnection() {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

