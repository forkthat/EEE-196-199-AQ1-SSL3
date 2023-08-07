// Necesaary Libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLvAHw_AQB"
#define BLYNK_DEVICE_NAME "Mesh"
#define BLYNK_AUTH_TOKEN "dY-71M4kS5u3dFcsizKTAZM1vztxq1GU"

char ssid[] = "_WiFi@UPD";
char pass[] = "";

// Serial2 pins of ESP32
#define RXD2 16
#define TXD2 17

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer
 
  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    //wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    //digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    //wifiFlag = 0;
    //digitalWrite(wifiLed, HIGH);
    Serial.println("Blynk Connected");
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80); 
  timer.setInterval(2000L, checkBlynkStatus);
  // Blynk.config(auth);
  delay(1000);
}

void loop() {
  Blynk.run();
  timer.run(); // Initiates SimpleTimer
}
 