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

int nodeNumber = 2;                           // Change node number for each node and visually label each node

auto taskSendMsg_DHT22_seconds = TASK_SECOND * 30;
auto taskSendMsg_MQ135_seconds = TASK_SECOND * 30;

int customWorkingPeriod_SDS011_minutes = 1;
auto taskSendMsg_SDS011_seconds = TASK_SECOND * 60;

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
  Serial.begin(9600);
  while (!Serial) continue;
  setup_JSON();
  setup_DHT22();
  setup_MQ135();
  setup_SDS011();
  setup_MESH();
}

void loop() {
  loop_JSON();
  loop_DHT22();
  loop_MQ135();
  loop_SDS011();
  loop_MESH();
}
