#include "painlessMesh.h"
#include <ArduinoJson.h>

#define MESH_PREFIX "AQ1_SSL3_"               // MESH_PREFIX: name of the node network
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

Scheduler userScheduler;                      // to control your personal task
painlessMesh mesh;

#define RXD2 16
#define TXD2 17

void setup(){
  Serial.begin(19200);
  while(!Serial) continue;

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while(!Serial2) continue;

  setup_MESH();
}

void loop(){
  loop_MESH();
}