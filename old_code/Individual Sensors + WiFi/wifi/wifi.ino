#include "WiFi.h"
 
const char* ssid = "_WiFi@UPD";
const char* password =  "";
 
void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  while (WiFi.status() == WL_CONNECTED) {
    delay(1000);
    Serial.println("Connected to the WiFi network");
  }
 
}
 
void loop() {}