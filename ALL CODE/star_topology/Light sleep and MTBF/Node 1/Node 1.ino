#include <ThingsBoard.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <math.h>
#include <TaskScheduler.h>


// ####################################################################
// WIFI AND THINGSBOARD
// ####################################################################

// const char* ssid = "PLDTHOMEFIBRwKJ3r";
// const char* password = "PLDTWIFIjwq5R";

// const char* ssid = "GeeHomeMesh";
// const char* password = "XpwATM1Ldeco";

const char* ssid = "Experimental Network";
const char* password = "tanongmokayjaybie";

// const char* ssid = "PLDTHOMEFIBR648f8";
// const char* password = "PLDTWIFIxvxz3";

RTC_DATA_ATTR unsigned long previousMillis = 0;
RTC_DATA_ATTR unsigned long interval = 30000;

#define TOKEN "NODE1_TOKEN"

char thingsboardServer[] = "thingsboard.cloud";

WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

RTC_DATA_ATTR int status = WL_IDLE_STATUS;
RTC_DATA_ATTR unsigned long lastSend;


// ####################################################################
// SENSOR TIMINGS
// ####################################################################

RTC_DATA_ATTR int customWorkingPeriod_SDS011_minutes = 0;


// ####################################################################
// MEAN TIME BETWEEN FAILURE (MTBF)
// ####################################################################

RTC_DATA_ATTR int numberOfTestingHours = 24;
RTC_DATA_ATTR int measurementsPerHour = 11; // 11 measurements per hour kasi 5 min each sensor reading
RTC_DATA_ATTR int currentMeasurement = 0;  // Measures the number of times sensor measurement has been done
RTC_DATA_ATTR int totalMeasurement = numberOfTestingHours * measurementsPerHour;  // Total number of times measurement should be done, depends on totalRuntime
RTC_DATA_ATTR unsigned long totalRuntime = numberOfTestingHours * 60 * 60 * 1000;  // Total test duration in milliseconds 
RTC_DATA_ATTR unsigned int numFailures = 0;  // Number of failures during the test
RTC_DATA_ATTR bool testExpired = false;  // Flag variable to indicate if the test duration has expired


// ####################################################################
// DHT22
// ####################################################################

#include <DHT.h>

#define DHT_SENSOR_PIN  21                    // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

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

void setup_MQ135(){

}

void loop_DHT22_MQ135(){
  // DHT22
  float tempC = dht_sensor.readTemperature();
  float humi  = dht_sensor.readHumidity();
  Serial.println("Temperature: " + String(tempC) + " C");
  Serial.println("Humidity: " + String(humi) + "%");
  if (isnan(tempC) || isnan(humi)) { //don't send to thingsboard for temp and humi no value
    numFailures++;
    tb.sendTelemetryFloat("N1 Failures", numFailures);
  }
  else {
    tb.sendTelemetryFloat("N1 Temperature", tempC);
    tb.sendTelemetryFloat("N1 Humidity", humi);
    bool dataSent_DHT22 = tb.sendTelemetryFloat("N1 Temperature", tempC) && tb.sendTelemetryFloat("N1 Humidity", humi);
    // Check if data was sent successfully
    if (dataSent_DHT22) { 
      Serial.println("Temp and humidity data sent to ThingsBoard successfully");
    } else {
      numFailures++;
      Serial.print("Failed to send temp and humidity data to ThingsBoard!");
    }
  }

  // MQ135
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(tempC, humi);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(tempC, humi);
  float correctedPPM_ugm3 = correctedPPM * 1000;
  Serial.print("CO: ");
  Serial.print(correctedPPM_ugm3);
  Serial.print(" ug/m3 \n");
  if (isnan(correctedPPM_ugm3)) { // don't send to thingsboard for CO if no value
    numFailures++;
    tb.sendTelemetryFloat("N1 Failures", numFailures);
  }
  else {
    tb.sendTelemetryFloat("N1 CO", correctedPPM_ugm3);
    bool dataSent_MQ135 = tb.sendTelemetryFloat("N1 CO", correctedPPM_ugm3);
    // Check if data was sent successfully
    if (dataSent_MQ135) { 
      Serial.println("CO data sent to ThingsBoard successfully");
    } else {
      numFailures++;
      Serial.print("Failed to send CO data to ThingsBoard!");
    }
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
  Serial.println(sds.setCustomWorkingPeriod(customWorkingPeriod_SDS011_minutes).toString()); 
}

void loop_SDS011() {
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    saveReadings_SDS011(float(pm.pm25), float(pm.pm10));
     if (isnan(pm.pm25) || isnan(pm.pm10)) { //don't send to thingsboard for PM2.5 and 10 if no value
       numFailures++;
       tb.sendTelemetryFloat("N1 Failures", numFailures);
     }
     else {
       Serial.println(pm.toString() + " ug/m3");
       tb.sendTelemetryFloat("N1 PM2.5", pm.pm25);
       tb.sendTelemetryFloat("N1 PM10", pm.pm10);
       bool dataSent_NOVA = tb.sendTelemetryFloat("N1 PM2.5", pm.pm25) && tb.sendTelemetryFloat("N1 PM10", pm.pm10);
       // Check if data was sent successfully
       if (dataSent_NOVA) { 
         Serial.println("PM data sent to ThingsBoard successfully");
       } 
       else {
        numFailures++;
        Serial.print("Failed to send PM data to ThingsBoard!");
       }
     }
    // if you want to just print the measured values, you can use toString() method as well
  } else {
  }
  tb.sendTelemetryFloat("N1 Failures", numFailures); // sending the total numFailures currently for the loop
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
  if (currentMeasurement > totalMeasurement) {
    // Test duration exceeded, print the MTBF and set the flag to stop measurements
    float mtbf = numFailures > 0 ? (float)totalRuntime / numFailures : 0;
    Serial.print("Test duration: 1 day\n");
    Serial.print("Total runtime: ");
    Serial.print(totalRuntime);
    Serial.print(" ms\n");
    Serial.print("Number of failures: ");
    Serial.print(numFailures);
    tb.sendTelemetryFloat("N1 Failures", numFailures);
    if (mtbf == 0) {
      Serial.print("\nMTBF: ");
      Serial.print("Infinity (No failures occurred)");
      tb.sendTelemetryFloat("N1 MTBF", mtbf);
    } else {
      Serial.print("\nMTBF: ");
      Serial.print(mtbf);
      Serial.print(" ms/failure");
      tb.sendTelemetryFloat("N1 MTBF", mtbf);
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

  if (millis() - lastSend > 1000) {  // Update and send only after 1 second
    loop_DHT22();
    loop_DHT22_MQ135();
    loop_SDS011();
    lastSend = millis();
  }
  tb.loop();
  currentMeasurement++;
  Serial.println("Current measurement: " + String(currentMeasurement));
  Serial.println("Current number of failures: " + String(numFailures));
  delay(15000); // To allow for data to be transmitted to ThingsBoard
  esp_sleep_enable_timer_wakeup(4 * 60 * 1000000 + 30 * 1000000); // Sleep for 4 minutes and 30 seconds
  delay(100);
  esp_light_sleep_start();
}
