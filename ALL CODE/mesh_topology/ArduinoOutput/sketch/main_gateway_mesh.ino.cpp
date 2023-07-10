#include <Arduino.h>
#line 1 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/main_gateway_mesh.ino"
#define RXD2 16
#define TXD2 17

#line 4 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/main_gateway_mesh.ino"
void setup();
#line 14 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/main_gateway_mesh.ino"
void loop();
#line 17 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/3_NetConfig_MESH.ino"
void sendMessage();
#line 25 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/3_NetConfig_MESH.ino"
void receivedCallback( uint32_t from, String &msg );
#line 37 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/3_NetConfig_MESH.ino"
void newConnectionCallback(uint32_t nodeId);
#line 42 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/3_NetConfig_MESH.ino"
void changedConnectionCallback();
#line 47 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/3_NetConfig_MESH.ino"
void nodeTimeAdjustedCallback(int32_t offset);
#line 51 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/3_NetConfig_MESH.ino"
void setup_MESH();
#line 62 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/3_NetConfig_MESH.ino"
void loop_MESH();
#line 4 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/main_gateway_mesh.ino"
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
#line 1 "/home/dennis/GitHub/EEE-196-199-AQ1-SSL3/ALL CODE/mesh_topology/main_gateway_mesh/3_NetConfig_MESH.ino"
/*
####################################################################
https://randomnerdtutorials.com/esp-mesh-esp32-esp8266-painlessmesh/
####################################################################
*/

#include "painlessMesh.h"
#include <ArduinoJson.h>

#define MESH_PREFIX "AQ1_SSL3_"               // MESH_PREFIX: name of the node network
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

Scheduler userScheduler;                      // to control your personal task
painlessMesh mesh;

void sendMessage() {
//   String msg = "Hi from node1";
//   msg += mesh.getNodeId();
//   mesh.sendBroadcast( msg );
//   taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Runs when a message is received on the mesh network
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  StaticJsonDocument<300> doc;
  deserializeJson(doc, msg);

  if (Serial2.available()) {
    serializeJson(doc, Serial2);
  }
}

// Runs when a new node joins the mesh network
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

// Runs when a new node joins or leaves the network
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

// Runs when the network adjusts the time, so that all nodes are synced
void nodeTimeAdjustedCallback(int32_t offset) {
  // Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup_MESH() {
  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop_MESH() {
  mesh.update();
}
