#include <ArduinoJson.h>
#include <math.h>
#include <TaskScheduler.h>


// ####################################################################
// WIFI AND THINGSBOARD
// ####################################################################

#include <WiFi.h>
#include <ThingsBoard.h>

//#define WIFI_SSID "Thingsboard"
//#define WIFI_PASSWORD "dennis@thingsboard"

#define WIFI_SSID "PLDTHOMEFIBRwKJ3r"
#define WIFI_PASSWORD "PLDTWIFIjwq5R"

#define TOKEN "NODE1_TOKEN"

WiFiClient wifiClient;

char thingsboardServer[] = "thingsboard.cloud";
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

void sendToGateway(uint32_t gateway_mesh_ID, String &msg){}

// ####################################################################
// DHT22
// ####################################################################

#include <DHT.h>

#define DHT_SENSOR_PIN  21                    // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22


// ####################################################################
// MQ135
// ####################################################################

#include <MQUnifiedsensor.h>

#define placa "ESP-32"
#define Voltage_Resolution 5
#define pin 34                                // Analog input 0 of your arduino
#define type "MQ-135"       
#define ADC_Bit_Resolution 12                 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6                // (RS/R0) = 3.6 ppm


// ####################################################################
// SDS011
// ####################################################################

#include "SdsDustSensor.h"

// ####################################################################
// SETUP AND LOOP
// ####################################################################

void setup() {
  Serial.begin(19200);
  while(!Serial) continue;
  setup_thingsboard();
  setup_DHT22();
  //setup_MQ135();
  setup_SDS011();
  InitWiFi();
}

void loop() {
  loop_thingsboard();

  if ( millis() - lastSend > 5000 ) { // Update and send only after 5 seconds, PREV: 1 SEC
    loop_DHT22();
    loop_MQ135();
    loop_SDS011();;
    lastSend = millis();
  }

  tb.loop();
}



  

