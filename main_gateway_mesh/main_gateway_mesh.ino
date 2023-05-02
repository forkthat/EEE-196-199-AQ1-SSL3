// ####################################################################
// MESH
// ####################################################################

#include "painlessMesh.h"

#define MESH_PREFIX "AQ1_SSL3_"               // MESH_PREFIX: name of the node network
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

Scheduler userScheduler;                      // to control your personal task
painlessMesh mesh;

/*
####################################################################
TODO: 
  on message receive, send data to router
####################################################################
*/

#define RXD2 16
#define TXD2 17

void setup(){
  Serial.begin(115200);
  while(!Serial) continue;

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while(!Serial2) continue;

  setup_MESH();
}

void loop(){
  // // read it from USB and send it to hSerial 
  // if (Serial.available()){ 
  //   hSerial.write(Serial.read()); 
  // }

  // // read it from hSerial and send it to USB
  // if (hSerial.available()){ 
  //   Serial.write(hSerial.read()); 
  // }

  loop_MESH();
}