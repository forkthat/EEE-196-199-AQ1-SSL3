// ####################################################################
// MESH
// ####################################################################

#include "painlessMesh.h"

#define MESH_PREFIX "AQ1_SSL3_"               // MESH_PREFIX: name of the node network
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

Scheduler userScheduler;                      // to control your personal task
painlessMesh mesh;

int task_delay = 10000;

// ####################################################################
// JSON
// ####################################################################

// #include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include <math.h>

// ####################################################################
// CRITICAL NODE INFORMATION AND TIMINGS
// ####################################################################

int nodeNumber = 3;                           // Change node number for each node and visually label each node
const uint32_t gateway_mesh_ID = 1843304141;  // ID of the ESP32 gateway
uint32_t msg_sent_success = 0;                // keeps count the successful packets sent during connection
uint32_t msg_sent_fail = 0;                   // keeps count the packets not sent during connection

// ===============================================================

// int DHT22_rate_seconds = 10;                  
// int MQ135_rate_seconds = 10;                  
// int customWorkingPeriod_SDS011_minutes = 1;
int DHT22_rate_seconds = 60 * 5;                  // every 5 mins
int MQ135_rate_seconds = 60 * 5;                  // every 5 mins
int customWorkingPeriod_SDS011_minutes = 5;
int customWorkingPeriod_SDS011_seconds = 60 * customWorkingPeriod_SDS011_minutes;  // every 5 mins

unsigned long taskSendMsg_DHT22_seconds = TASK_SECOND * DHT22_rate_seconds;
unsigned long taskSendMsg_DHT22_seconds_low = TASK_SECOND * (DHT22_rate_seconds + 1);
unsigned long taskSendMsg_DHT22_seconds_high = TASK_SECOND * (DHT22_rate_seconds + 5);

unsigned long taskSendMsg_MQ135_seconds = TASK_SECOND * MQ135_rate_seconds;
unsigned long taskSendMsg_MQ135_seconds_low = TASK_SECOND * (MQ135_rate_seconds + 1);
unsigned long taskSendMsg_MQ135_seconds_high = TASK_SECOND * (MQ135_rate_seconds + 5);

unsigned long taskSendMsg_SDS011_seconds = TASK_SECOND * customWorkingPeriod_SDS011_seconds;
unsigned long taskSendMsg_SDS011_seconds_low = TASK_SECOND * (customWorkingPeriod_SDS011_seconds + 1);
unsigned long taskSendMsg_SDS011_seconds_high = TASK_SECOND * (customWorkingPeriod_SDS011_seconds + 5);

// ===============================================================

// int latency_rate_seconds = 60;              
// double throughput_rate_seconds = 60;
// int MTBF_rate_seconds = 60;              
int latency_rate_seconds = 60;              // every 60 seconds
double throughput_rate_seconds = 60 * 15;   // every 15 minutes
int MTBF_rate_seconds = 60;                 // every 60 seconds
unsigned long taskLatency_rate_seconds = TASK_SECOND * latency_rate_seconds;
unsigned long taskLatency_rate_seconds_low = TASK_SECOND * (latency_rate_seconds + 1);
unsigned long taskLatency_rate_seconds_high = TASK_SECOND * (latency_rate_seconds + 5);

unsigned long taskThroughput_rate_seconds = TASK_SECOND * throughput_rate_seconds;
unsigned long taskThroughput_rate_seconds_low = TASK_SECOND * (throughput_rate_seconds + 1);
unsigned long taskThroughput_rate_seconds_high = TASK_SECOND * (throughput_rate_seconds + 5);

unsigned long taskMTBF_rate_seconds = TASK_SECOND * MTBF_rate_seconds;
unsigned long taskMTBF_rate_seconds_low = TASK_SECOND * (MTBF_rate_seconds + 1);
unsigned long taskMTBF_rate_seconds_high = TASK_SECOND * (MTBF_rate_seconds + 5);

// ####################################################################
// TASKS
// ####################################################################

Task taskSendMessage_DHT22(random(taskSendMsg_DHT22_seconds_low, taskSendMsg_DHT22_seconds_high), TASK_FOREVER, &sendMessage_DHT22);
Task taskSendMessage_MQ135(random(taskSendMsg_MQ135_seconds_low, taskSendMsg_MQ135_seconds_high), TASK_FOREVER, &sendMessage_MQ135);
Task taskSendMessage_SDS011(random(taskSendMsg_SDS011_seconds_low, taskSendMsg_SDS011_seconds_high), TASK_FOREVER, &sendMessage_SDS011);
Task task_Latency(random(taskLatency_rate_seconds_low, taskLatency_rate_seconds_high), TASK_FOREVER, &sendMessage_Latency);
Task task_Throughput(random(taskThroughput_rate_seconds_low, taskThroughput_rate_seconds_high), TASK_FOREVER, &end_Throughput);
Task task_MTBF(random(taskMTBF_rate_seconds_low, taskMTBF_rate_seconds_high), TASK_FOREVER, &sendMessage_MTBF);

// ####################################################################
// DHT22
// ####################################################################

#include <DHT.h>

#define DHT_SENSOR_PIN  21        // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22

String readings;
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

// ####################################################################
// MQ135
// ####################################################################

#include <MQUnifiedsensor.h>

#define placa "ESP-32"
#define Voltage_Resolution 5
#define pin 34                    // Analog input 0 of your arduino
#define type "MQ-135"       
#define ADC_Bit_Resolution 12                 
#define RatioMQ135CleanAir 3.6    // (RS/R0) = 3.6 ppm  

String str_CO;
float CO = 0;
float CO2 = 0;
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

// ####################################################################
// SDS011
// ####################################################################

#include "SdsDustSensor.h"
float pm25, pm10;
SdsDustSensor sds(Serial2);

// ####################################################################
// LATENCY
// ####################################################################

#include <cmath>
String msg_Latency;

// ####################################################################
// THROUGHPUT
// ####################################################################

int msg_size = 0;
double throughput = 0;

// ####################################################################
// MEAN TIME BETWEEN FAILURES
// ####################################################################

unsigned long num_failures = 0;
unsigned long long timer_MTBF_start;
unsigned long long timer_MTBF_end;
unsigned long long diff_MTBF = 0;
unsigned long long diff_MTBF_prev = 0;
long double sum_MTBF = 0;
long double calcu_MTBF = 0;
bool flag_MTBF_uptime = false;
bool flag_MTBF_change = true;

// ####################################################################
// SETUP AND LOOP
// ####################################################################

void setup() {
  Serial.begin(19200);
  while(!Serial) continue;

  Serial.println("======================================================");
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
  loop_MTBF();
}
