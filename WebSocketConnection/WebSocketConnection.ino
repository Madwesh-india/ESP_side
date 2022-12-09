#include <WiFi.h> // ESP32 WiFi Library
#include <WebServer.h> // WebServer Library for ESP32
#include <WebSocketsClient.h> // WebSocket Client Library for WebSocket
#include <ArduinoJson.h> // Arduino JSON Library
#include <string.h>


const char* ssid = "maheshwari"; // Wifi SSID
const char* password = "maheshwari65"; //Wi-FI Password
WebSocketsClient webSocket; // websocket client class instance
StaticJsonDocument<100> doc; // Allocate a static JSON document

int relay = 4; //D4
int photo = 5;

void setup() {
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  pinMode(photo, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print local IP address

  webSocket.setExtraHeaders("user-agent: Mozilla");
  webSocket.beginSSL("wss://mad-esp-server.glitch.me", 426);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(6000);

}

void loop() {
  webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_CONNECTED){
    DynamicJsonDocument doc(1024);
    char msg[100];
    doc["isfirst"] = "true";
    doc["device"] = "ESP";
    serializeJson(doc, msg);
    webSocket.sendTXT(msg);
    Serial.println(msg);
  }
  if (type == WStype_TEXT)
  {
    // deserialize incoming Json String
    DeserializationError error = deserializeJson(doc, payload); 
    if (error) { // Print erro msg if incomig String is not JSON formated
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    else{
      Serial.print("Connection: ");
      serializeJson(doc["conection"], Serial);
      Serial.print("\n");
      if(strcmp(doc["conection"].as<String>() .c_str(), "message" ) == 0){
        if(strcmp(doc["message"].as<String>() .c_str(), "pc_on") == 0){
          digitalWrite(relay, LOW);
          Serial.print("RELAY HIGH\n");
          delay(1000);
          digitalWrite(photo, HIGH);
          delay(1000);
          digitalWrite(photo, LOW);

        }
        else if(strcmp(doc["message"].as<String>() .c_str(), "pc_off") == 0){
          digitalWrite(relay,  HIGH);
          Serial.print("RELAY LOW\n");
        }
      }
      
    }
    //Serial.print(doc["conection"].as<char[]>());
  }
  if(type == WStype_DISCONNECTED){
    Serial.print("\nDisconnected\n");
  webSocket.beginSSL("wss://mad-esp-server.glitch.me", 426);
  }
}