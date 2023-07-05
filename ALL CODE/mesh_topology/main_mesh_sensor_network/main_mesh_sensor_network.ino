// ####################################################################
// MESH
// ####################################################################

#include "painlessMesh.h"

#define MESH_PREFIX "AQ1_SSL3_"               // MESH_PREFIX: name of the node network
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

Scheduler userScheduler;                      // to control your personal task
painlessMesh mesh;

// ####################################################################
// JSON
// ####################################################################

// #include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include <math.h>

// ####################################################################
// NODE NUMBER AND SENSOR TIMINGS
// ####################################################################

int nodeNumber = 3;                           // Change node number for each node and visually label each node
const uint32_t gateway_mesh_ID = 1843304141;  // ID of the ESP32 gateway
uint32_t msg_sent_success = 0;                // keeps count the successful packets sent during connection
uint32_t msg_sent_fail = 0;                   // keeps count the packets not sent during connection

int DHT22_rate_seconds = 60 * 5;
int MQ135_rate_seconds = 60 * 5;
int customWorkingPeriod_SDS011_minutes = 5;

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

// LATENCY
int checkLatency_rate_seconds = 30;
unsigned long taskCheckLatency_rate_seconds = TASK_SECOND * checkLatency_rate_seconds;

int sendMsgLatency_rate_seconds = 30;
unsigned long taskSendMsg_Latency_seconds = TASK_SECOND * sendMsgLatency_rate_seconds;
unsigned long taskSendMsg_Latency_seconds_low = TASK_SECOND * (sendMsgLatency_rate_seconds + 1);
unsigned long taskSendMsg_Latency_seconds_high = TASK_SECOND * (sendMsgLatency_rate_seconds + 5);

// THROUGHPUT
int throughput_rate_seconds = 60 * 60;
unsigned long taskCalculateThroughput_rate_seconds = TASK_SECOND * throughput_rate_seconds;

int sendMsgThroughput_rate_seconds = 60 * 60;
unsigned long taskSendMsg_Throughput_seconds = TASK_SECOND * sendMsgThroughput_rate_seconds;
unsigned long taskSendMsg_Throughput_seconds_low = TASK_SECOND * (sendMsgThroughput_rate_seconds + 1);
unsigned long taskSendMsg_Throughput_seconds_high = TASK_SECOND * (sendMsgThroughput_rate_seconds + 5);

// ####################################################################
// DHT22
// ####################################################################

#include <DHT.h>

#define DHT_SENSOR_PIN  21                    // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22

String readings;

// ####################################################################
// MQ135
// ####################################################################

#include <MQUnifiedsensor.h>

#define placa "ESP-32"
#define Voltage_Resolution 5
#define pin 34                                // Analog input 0 of your arduino
#define type "MQ-135"       
#define ADC_Bit_Resolution 12                 
#define RatioMQ135CleanAir 3.6                // (RS/R0) = 3.6 ppm  

double CO2 = (0);

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

  setup_MESH();
  setup_DHT22();
  setup_MQ135();
  setup_SDS011();
}

void loop() {
  loop_MESH();
  loop_DHT22();
  loop_MQ135();
  loop_SDS011();
}
