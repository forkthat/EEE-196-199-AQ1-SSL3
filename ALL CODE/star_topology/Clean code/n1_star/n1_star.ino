#include <ArduinoJson.h>
#include <math.h>
#include <TaskScheduler.h>


// ####################################################################
// WIFI AND THINGSBOARD
// ####################################################################

#include <WiFi.h>
#include <WiFiClient.h>
#include <ThingsBoard.h>

// const char* ssid = "GeeHomeMesh";
// const char* password = "XpwATM1Ldeco";

const char* ssid = "Experimental Network";
const char* password = "tanongmokayjaybie";

#define TOKEN "NODE1_TOKEN"

WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

// ####################################################################
// SENSOR TIMINGS
// ####################################################################

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


// ####################################################################
// DHT22
// ####################################################################

#include <DHT.h>

#define DHT_SENSOR_PIN  21                   
#define DHT_SENSOR_TYPE DHT22
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);



// ####################################################################
// MQ135
// ####################################################################

#include <MQ135.h>

#define PIN_MQ135 34 // MQ135 Analog Input Pin

MQ135 mq135_sensor(PIN_MQ135);


// ####################################################################
// SDS011
// ####################################################################

#include "SdsDustSensor.h"


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

void loop() {
  loop_WIFI();
  loop_thingsboard();
}











  

