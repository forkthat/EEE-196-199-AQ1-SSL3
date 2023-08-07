#include <ThingsBoard.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <math.h>
#include <TaskScheduler.h>

// ####################################################################
// WIFI
// ####################################################################

// const char* ssid = "PLDTHOMEFIBRwKJ3r";
// const char* password = "PLDTWIFIjwq5R";

// const char* ssid = "GeeHomeMesh";
// const char* password = "XpwATM1Ldeco";

// const char* ssid = "Experimental Network";
// const char* password = "tanongmokayjaybie";

const char* ssid = "PLDTHOMEFIBR648f8";
const char* password = "PLDTWIFIxvxz3";

unsigned long previousMillis = 0;
unsigned long interval = 30000;

#define TOKEN "NODE1_TOKEN"

char thingsboardServer[] = "thingsboard.cloud";

WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

// ####################################################################
// SENSOR TIMINGS
// ####################################################################

int customWorkingPeriod_SDS011_minutes = 0;


// ####################################################################
// MEAN TIME BETWEEN FAILURE (MTBF)
// ####################################################################

unsigned long startTime;  // Start time of the test
unsigned long totalRuntime = 24 * 60 * 60 * 1000;  // Total test duration in milliseconds (1 day)
unsigned int numFailures = 0;  // Number of failures during the test
bool testExpired = false;  // Flag variable to indicate if the test duration has expired


// ####################################################################
// DHT22
// ####################################################################

#include <DHT.h>

#define DHT_SENSOR_PIN  21                    // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

String readings;
String getReadings_DHT22(){
}
void setup_DHT22(){
  dht_sensor.begin();
}

void loop_DHT22(){
  
}

// ####################################################################
// MQ135
// ####################################################################

#include <MQ135.h>

#define PIN_MQ135 34 // MQ135 Analog Input Pin

MQ135 mq135_sensor(PIN_MQ135);

int counter = 0;
double arr_CRZero[10];
double arr_CPPM[10];
double sum_CRZero;
double sum_CPPM;
double MA_CRZero;
double MA_CPPM;

String getReadings_MQ135(){
}

void setup_MQ135(){

}

void loop_DHT22_MQ135(){
  //DHT22
  delay(300000); // measures every 5 min
  float tempC = dht_sensor.readTemperature();
  float humi  = dht_sensor.readHumidity();
  Serial.println("Temperature: " + String(tempC) + " C");
  Serial.println("Humidity: " + String(humi) + "%");
  if (isnan(tempC) || isnan(humi) || tempC == 0.00 || humi == 0.00) { //don't send to thingsboard for temp and humi if 0 or no value
    numFailures++;
  }
  else {
    tb.sendTelemetryFloat("N1 Temperature", tempC);
    tb.sendTelemetryFloat("N1 Humidity", humi);
  }

  //MQ135
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(tempC, humi);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(tempC, humi);
  float correctedPPM_ugm3 = correctedPPM * 1000;
  Serial.print("CO: ");
  Serial.print(correctedPPM_ugm3);
  Serial.print(" ug/m3 \n");
  if (isnan(correctedPPM_ugm3) || correctedPPM_ugm3 == 0.00) { //don't send to thingsboard for CO if 0 or no value
    numFailures++;
  }
  else {
    tb.sendTelemetryFloat("N1 CO", correctedPPM_ugm3);
  }
  
  
  arr_CRZero[counter] = correctedRZero;
  arr_CPPM[counter] = correctedPPM;

  sum_CRZero = 0;
  sum_CPPM = 0;
  for(int i = 0; i < 10; i++){
    sum_CRZero = sum_CRZero + arr_CRZero[i];
    sum_CPPM = sum_CPPM + arr_CPPM[i];
  }
  MA_CRZero = sum_CRZero / 10;
  MA_CPPM = sum_CPPM / 10;
  counter++;
  if (counter >= 10){
    counter = 0;
  }
}


// ####################################################################
// SDS011
// ####################################################################

#include "SdsDustSensor.h"

float pm25, pm10;
SdsDustSensor sds(Serial2);

void saveReadings_SDS011(float pm25_new, float pm10_new){
  pm25 = pm25_new;
  pm10 = pm10_new;
}

String getReadings_SDS011(){
  StaticJsonDocument<128> doc;

  doc["PM_2.5"] = pm25;
  doc["PM_10"] = pm10;

  return doc.as<String>();
}

void setup_SDS011() {
  sds.begin();
  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setCustomWorkingPeriod(customWorkingPeriod_SDS011_minutes).toString()); // sensor sends data every 5 minutes
}

void loop_SDS011() {
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    saveReadings_SDS011(float(pm.pm25), float(pm.pm10));
     if (isnan(pm.pm25) || isnan(pm.pm10) || pm.pm25 == 0.00 || pm.pm10 == 0.00) { //don't send to thingsboard for PM2.5 and 10 if 0 or no value
       numFailures++;
     }
     else {
       tb.sendTelemetryFloat("N1 PM2.5", pm.pm25);
       tb.sendTelemetryFloat("N1 PM10", pm.pm10);
     }
    // if you want to just print the measured values, you can use toString() method as well
    Serial.println(pm.toString() + " ug/m3");
  } else {
  }
}

// ####################################################################
// SETUP AND LOOP
// ####################################################################

void setup() {
  Serial.begin(115200);
  while(!Serial) continue;
  setup_WIFI();
  setup_DHT22();
  setup_MQ135();
  setup_SDS011();
  setup_thingsboard();
  startTime = millis();  // Record the start time
}

void reconnect_WIFI()
{
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

void loop() {
  if (testExpired) {
    delay(500);  // Add a small delay to avoid excessive looping if MTBF calculation is finished
    return;
  }

  // MTBF Calculation
  unsigned long currentTime = millis();
  if (currentTime - startTime >= totalRuntime) {
    // Test duration exceeded, print the MTBF and set the flag to stop measurements
    float mtbf = numFailures > 0 ? (float)totalRuntime / numFailures : 0;
    Serial.print("Test duration: 1 hour\n");
    Serial.print("Total runtime: ");
    Serial.print(totalRuntime);
    Serial.print(" ms\n");
    Serial.print("Number of failures: ");
    Serial.print(numFailures);
    Serial.print("\nMTBF: ");
    Serial.print(mtbf);
    Serial.print(" ms/failure\n");
    tb.sendTelemetryFloat("N1 MTBF", mtbf);
    if (mtbf == 0) {
      Serial.print("Infinity (No failures occurred)");
    } else {
      Serial.print(mtbf);
      Serial.print(" ms/failure");
    }
    Serial.println();
    testExpired = true;  // Set the flag to stop further measurements
    return;
  }
  loop_WIFI();
  loop_thingsboard();
}

void init_WIFI() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println(WiFi.localIP());
}

void setup_WIFI()
{
  init_WIFI();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop_WIFI()
{
  reconnect_WIFI();
}

void setup_thingsboard() {
  lastSend = 0;
}

void loop_thingsboard() {
  if (!tb.connected()) {
    while (!tb.connected()) {
      Serial.print("Connecting to ThingsBoard node ...");
      if (tb.connect(thingsboardServer, TOKEN)) {
        Serial.println("[DONE]");
      } else {
        Serial.print("[FAILED]");
        Serial.println(" : retrying in 5 seconds]");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }

  //send data to thingsboard
  if (millis() - lastSend > 1000) {  // Update and send only after 1 second
    loop_DHT22();
    loop_DHT22_MQ135();
    loop_SDS011();
    lastSend = millis();
  }
  tb.loop();
}




  

