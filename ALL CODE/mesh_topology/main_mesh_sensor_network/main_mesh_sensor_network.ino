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

// ####################################################################
// DHT22
// ####################################################################

#include <DHT.h>

#define DHT_SENSOR_PIN  21                    // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22

String readings;
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

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

int latency_rate_seconds = 30;
unsigned long taskLatency_rate_seconds = TASK_SECOND * latency_rate_seconds;

int latency_data[10];
int latency_index = 0;
int sum = 0;
int ave_latency = 0;
String msg_Latency;
bool flag_delay_received = false;

// ####################################################################
// THROUGHPUT
// ####################################################################

// every hour
int throughput_rate_seconds = 60 * 60;
unsigned long taskThroughput_rate_seconds = TASK_SECOND * throughput_rate_seconds;

unsigned int msg_size = 0;
double throughput = 0;

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
