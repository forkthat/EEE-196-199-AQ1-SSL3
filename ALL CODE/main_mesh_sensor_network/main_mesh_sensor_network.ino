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

int nodeNumber = 1;                           // Change node number for each node and visually label each node
uint32_t gateway_mesh_ID = 1843304141;        // ID of the ESP32 gateway
uint32_t msg_sent_success = 0;                // keeps count the successful packets sent during connection
uint32_t msg_sent_fail = 0;                   // keeps count the packets not sent during connection

auto taskSendMsg_DHT22_seconds = TASK_SECOND * 30;
auto taskSendMsg_DHT22_seconds_low = TASK_SECOND * 25;
auto taskSendMsg_DHT22_seconds_high = TASK_SECOND * 35;

auto taskSendMsg_MQ135_seconds = TASK_SECOND * 30;
auto taskSendMsg_MQ135_seconds_low = TASK_SECOND * 25;
auto taskSendMsg_MQ135_seconds_high = TASK_SECOND * 35;

int customWorkingPeriod_SDS011_minutes = 1;
auto taskSendMsg_SDS011_seconds = TASK_SECOND * 60;
auto taskSendMsg_SDS011_seconds_low = TASK_SECOND * 60;
auto taskSendMsg_SDS011_seconds_high = TASK_SECOND * 70;

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
#define Voltage_Resolution 3.3
#define pin 34                                // Analog input 0 of your arduino
#define type "MQ-135"       
#define ADC_Bit_Resolution 12                 // For arduino UNO/MEGA/NANO
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
  Serial.begin(115200);
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