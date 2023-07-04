#include <ThingsBoard.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
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
const char* serverIP = "192.168.1.72";
const int serverPort = 1234;

// const char* ssid = "PLDTHOMEFIBR648f8";
// const char* password = "PLDTWIFIxvxz3";

RTC_DATA_ATTR unsigned long previousMillis = 0;
RTC_DATA_ATTR unsigned long interval = 30000;

#define TOKEN "NODE1_TOKEN"

char thingsboardServer[] = "thingsboard.cloud";

WiFiClient wifiClient;

WiFiUDP udpClient;
IPAddress serverAddress;

ThingsBoard tb(wifiClient);

RTC_DATA_ATTR int status = WL_IDLE_STATUS;
RTC_DATA_ATTR unsigned long lastSend;


// ####################################################################
// SENSOR TIMINGS
// ####################################################################

RTC_DATA_ATTR int customWorkingPeriod_SDS011_minutes = 0;
RTC_DATA_ATTR unsigned long startTime = 0;
RTC_DATA_ATTR unsigned long endTime = 0;
float elapsedTime = endTime - startTime;  // Time spent for sending data


// ####################################################################
// MEAN TIME BETWEEN FAILURE (MTBF)
// ####################################################################

RTC_DATA_ATTR int numberOfTestingHours = 1; 
RTC_DATA_ATTR int currentMeasurement = 0;  // Measures the number of times sensor measurement has been done
RTC_DATA_ATTR unsigned long totalRuntime = numberOfTestingHours * 60 * 60 * 1000;  // Total test duration in milliseconds 
RTC_DATA_ATTR unsigned int numFailures = 0;  // Number of failures during the test

RTC_DATA_ATTR const int packetSize = 20; // each packet has 20 bytes kasi 4 bytes each air quality parameter (temp, humi, CO, pm2.5, pm10)
RTC_DATA_ATTR const int numPackets = 12;  // Total number of packets to send, 12 per hour kasi 5 minutes each sensor measurement
RTC_DATA_ATTR const int totalSize = packetSize * numPackets * numberOfTestingHours;  // Total data size in bytes 
RTC_DATA_ATTR unsigned int packetCount = 0;
RTC_DATA_ATTR byte packet[packetSize]; // array to store the packets


// ####################################################################
// NETWORK PARAMETERS CALCULATION
// ####################################################################

float throughput = (totalSize * 8) / (elapsedTime / 1000.0) / 1000.0;  // Mbps
float averageLatency = elapsedTime / numPackets;  // milliseconds
float packetLoss = ((numPackets - packetCount) / static_cast<float>(numPackets)) * 100.0;
float mtbf = numFailures > 0 ? (float)totalRuntime / numFailures : 0;


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

  // STORE IN PACKET
  packet[0] = tempC;
  packet[1] = humi;

  Serial.println("Temperature: " + String(tempC) + " C");
  Serial.println("Humidity: " + String(humi) + "%");
  if (isnan(tempC) || isnan(humi) || tempC == 0.00 || humi == 0.00) { //don't send to thingsboard for temp and humi if 0 or no value
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

  // STORE IN PACKET
  packet[2] = correctedPPM_ugm3;

  if (isnan(correctedPPM_ugm3) || correctedPPM_ugm3 == 0.00) { // don't send to thingsboard for CO if 0 or no value
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
    // STORE IN PACKET
    packet[3] = pm.pm25;
    packet[4] = pm.pm10;
     if (isnan(pm.pm25) || isnan(pm.pm10) || pm.pm25 == 0.00 || pm.pm10 == 0.00) { //don't send to thingsboard for PM2.5 and 10 if 0 or no value
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

  // SERVER
  serverAddress = IPAddress();
  if (!serverAddress.fromString(serverIP)) {
    Serial.println("Invalid server IP address");
    while (1) {
      delay(1000);
    }
  }
  udpClient.begin(1234);  // UDP port for client

  setup_DHT22();
  setup_MQ135();
  setup_SDS011();
  setup_thingsboard();
}

void loop() {
  // PACKET PREPARATION
  if (packetCount == 0) {
    // Start measuring
    startTime = millis();
  }

  if (packetCount == numPackets) {
    // Finished sending packets, stop measuring
    endTime = millis();
    

    // PRINT ONLY NETWORK PARAMETER RESULTS ONCE
    Serial.println("Measurement Results:");
    Serial.print("Throughput: ");
    Serial.print(throughput);
    Serial.println(" Mbps");
    Serial.print("Average Latency: ");
    Serial.print(averageLatency);
    Serial.println(" ms");
    Serial.print("Packet Loss: ");
    Serial.print(packetLoss);
    Serial.println("%");
    Serial.print("Test duration: 1 day\n");
    Serial.print("Total runtime: ");
    Serial.print(totalRuntime);
    Serial.print(" ms\n");
    Serial.print("Number of failures: ");
    Serial.print(numFailures);
    if (mtbf == 0) {
      Serial.print("\nMTBF: ");
      Serial.print("Infinity (No failures occurred)");
    } else {
      Serial.print("\nMTBF: ");
      Serial.print(mtbf);
      Serial.print(" ms/failure");
    }
    loop_thingsboard_network();
    Serial.println();
    while (1) {
      delay(1000);
    }
  loop_WIFI();
  loop_thingsboard_sensors();
  }
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

void reconnect_WIFI() {
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

void setup_WIFI() {
  init_WIFI();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop_WIFI() {
  reconnect_WIFI();
}

void setup_thingsboard() {
  lastSend = 0;
}

void loop_thingsboard_network() {
  if (!tb.connected()) {
    while (!tb.connected()) {
      Serial.println("Connecting to ThingsBoard node ...");
      if (tb.connect(thingsboardServer, TOKEN)) {
        Serial.println("Successfuly connected to ThingsBoard!");
        tb.sendTelemetryFloat("N1 Throughput", throughput);
        tb.sendTelemetryFloat("N1 Latency", averageLatency);
        tb.sendTelemetryFloat("N1 Packet Loss", packetLoss);
        tb.sendTelemetryFloat("N1 MTBF", mtbf);
        tb.sendTelemetryFloat("N1 Failures", numFailures);
      } else {
        Serial.print("[FAILED]");
        Serial.println(" : retrying in 5 seconds]");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}

void loop_thingsboard_sensors() {
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


  // PACKET SENDING
  packetCount++;
  udpClient.beginPacket(serverAddress, serverPort);
  udpClient.write(packet, packetSize);
  udpClient.endPacket();


  // SENSOR MEASUREMENT COUNTER
  currentMeasurement++;
  Serial.println(currentMeasurement);


  // ESP32 DEEP SLEEP MODE 
  delay(15000); // To allow for data to be transmitted to ThingsBoard
  esp_sleep_enable_timer_wakeup(4 * 60 * 1000000 + 30 * 1000000); // Sleep for 4 minutes and 30 seconds
  esp_deep_sleep_start();
}



  

