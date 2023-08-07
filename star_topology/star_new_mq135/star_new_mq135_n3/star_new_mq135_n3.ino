// ####################################################################
// DOWNLOAD LIBRARY ng MQ135 by GeorgeK
// ####################################################################


#include <ThingsBoard.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <math.h>
#include <TaskScheduler.h>

//Scheduler userScheduler;                      // to control your personal task
//painlessMesh mesh;

// ####################################################################
// WIFI
// ####################################################################

// const char* ssid = "PLDTHOMEFIBRwKJ3r";
// const char* password = "PLDTWIFIjwq5R";

//const char* ssid = "MSI";
//const char* password = "thesisit";

const char* ssid = "Experimental Network";
const char* password = "tanongmokayjaybie";

unsigned long previousMillis = 0;
unsigned long interval = 30000;

#define TOKEN "NODE3_TOKEN"

char thingsboardServer[] = "thingsboard.cloud";

WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

// ####################################################################
// NODE NUMBER AND SENSOR TIMINGS
// ####################################################################

int nodeNumber = 1;                           // Change node number for each node and visually label each node
uint32_t msg_sent_success = 0;                // keeps count the successful packets sent during connection
uint32_t msg_sent_fail = 0;                   // keeps count the packets not sent during connection

int DHT22_rate_seconds = 30;
int MQ135_rate_seconds = 30;
int customWorkingPeriod_SDS011_minutes = 1;

unsigned long taskSendMsg_DHT22_seconds = TASK_SECOND * DHT22_rate_seconds;
unsigned long taskSendMsg_DHT22_seconds_low = TASK_SECOND * (DHT22_rate_seconds + 1);
unsigned long taskSendMsg_DHT22_seconds_high = TASK_SECOND * (DHT22_rate_seconds + 5);

unsigned long taskSendMsg_MQ135_seconds = TASK_SECOND * MQ135_rate_seconds;
unsigned long taskSendMsg_MQ135_seconds_low = TASK_SECOND * (MQ135_rate_seconds + 1);
unsigned long taskSendMsg_MQ135_seconds_high = TASK_SECOND * (MQ135_rate_seconds + 5);

int customWorkingPeriod_SDS011_seconds = customWorkingPeriod_SDS011_minutes * 60;
unsigned long taskSendMsg_SDS011_seconds = TASK_SECOND * customWorkingPeriod_SDS011_seconds;
unsigned long taskSendMsg_SDS011_seconds_low = TASK_SECOND * (customWorkingPeriod_SDS011_seconds + 1);
unsigned long taskSendMsg_SDS011_seconds_high = TASK_SECOND * (customWorkingPeriod_SDS011_seconds + 5);

//Task taskSendMessage_DHT22(taskSendMsg_DHT22_seconds, TASK_FOREVER, &sendMessage_DHT22);
//Task taskSendMessage_MQ135(taskSendMsg_MQ135_seconds, TASK_FOREVER, &sendMessage_MQ135);
//Task taskSendMessage_SDS011(taskSendMsg_SDS011_seconds, TASK_FOREVER, &SendMessage_SDS011);

void sendToGateway(uint32_t gateway_mesh_ID, String &msg){}


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
  /////DHT22
  float tempC = dht_sensor.readTemperature();
  float humi  = dht_sensor.readHumidity();
  Serial.println("Temperature: " + String(tempC) + " C");
  Serial.println("Humidity: " + String(humi) + "%");
  if (isnan(tempC) || isnan(humi) || tempC == 0.00 || humi == 0.00) { //don't send to thingsboard for temp and humi if 0 or no value
  }
  else {
    float hcal1 = 44.94615445;
    float hcal2 = 15.47692292;
    float uh1 = 25.4;
    float uh2 = 41.9;
    float tcal1 = 28.44905641;
    float tcal2 = 4.447171448;
    float ut1 = 1.6;
    float ut2 = 27.4;
    float tempC = dht_sensor.readTemperature();
    float humi  = dht_sensor.readHumidity();
    float finalhumi = ( ( (humi - hcal1) * uh1) / hcal2) + uh2;
    float finaltemp = ( ( (tempC - tcal1) * ut1) / tcal2) + ut2;
    tb.sendTelemetryFloat("N2 Temperature", finaltemp);
    tb.sendTelemetryFloat("N3 Humidity", finalhumi);
  }

  ////MQ135
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(tempC, humi);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(tempC, humi);
  float correctedPPM_ugm3 = correctedPPM * 1000;

  // Serial.print("RZero: ");
  // Serial.print(rzero);
  // Serial.print("\t Corrected RZero: ");
  // Serial.print(correctedRZero);
  // Serial.print("\t Resistance: ");
  // Serial.print(resistance);
  // Serial.print("\t PPM: ");
  // Serial.print(ppm);
  // Serial.print("ppm");
  // Serial.print("\t Corrected PPM: ");
  // Serial.print(correctedPPM);
  // Serial.print("ppm");
  Serial.print("CO: ");
  Serial.print(correctedPPM_ugm3);
  Serial.print(" ug/m3 \n");
  if (isnan(correctedPPM_ugm3) || correctedPPM_ugm3 == 0.00) { //don't send to thingsboard for CO if 0 or no value
  }
  else {
    float ccal1 = 0;
    float ccal2 = 38258.32098;
    float uc1 = 700;
    float uc2 = 0;
    float finalco = ( ( (correctedPPM_ugm3 - ccal1) * uc1) / ccal2) + uc2;
    tb.sendTelemetryFloat("N3 CO", finalco);
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

  // Serial.print("\t MA_CRZero: ");
  // Serial.print(MA_CRZero);
  // Serial.print("\t MA_CPPM: ");
  // Serial.print(MA_CPPM);
  // Serial.println("ppm");

  counter++;
  if (counter >= 10){
    counter = 0;
  }

  delay(300);
  
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

  doc["node_num"] = nodeNumber;
  doc["msg_success"] = msg_sent_success;
  doc["msg_fail"] = msg_sent_fail;

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
     
     }
     else {
      float pcal1 = 1.24000001;
      float pcal2 = 5.980000067;
      float up1 = 149;
      float up2 = 51;
      float finalpm = ( ( (pm.pm25 - pcal1) * up1) / pcal2) + up2;
       tb.sendTelemetryFloat("N3 PM2.5", finalpm);
       tb.sendTelemetryFloat("N3 PM10", pm.pm10);
     }
    // if you want to just print the measured values, you can use toString() method as well
    Serial.println(pm.toString() + " ug/m3");
    //Serial.println(pm.toString());
  } else {
    // notice that loop delay is set to 5s (sensor sends data every 3 minutes) and some reads are not available
    // Serial.print("Could not read values from sensor, reason: ");
    // Serial.println(pm.statusToString());
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

  // float pm25_result = pm25 / 10;
  // float pm10_result = pm10 / 10;

  //send data to thingsboard
  if (millis() - lastSend > 5000) {  // Update and send only after 5 seconds; PREV: 1 SEC
    loop_DHT22();
    loop_DHT22_MQ135();
    loop_SDS011();
    lastSend = millis();
  }
  tb.loop();
}




  

