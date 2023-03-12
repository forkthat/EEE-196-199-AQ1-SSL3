/*
####################################################################
https://randomnerdtutorials.com/esp-mesh-esp32-esp8266-painlessmesh/
####################################################################
*/

#include "painlessMesh.h"

// MESH_PREFIX: name of the node
#define MESH_PREFIX "AQ1_SSL3_"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

/*
####################################################################
Change node number for each node
TODO: visually label each node
####################################################################
*/
#define NODE_NUMBER 1

// Run tasks in the background
Scheduler userScheduler;
painlessMesh mesh;

// call sendMessage() function every 1 second as long as
// the program is running
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

// 
void sendMessage() {
  String hi = "Hi from Node ";
  String msg = hi + NODE_NUMBER + "::";
  msg += mesh.getNodeId();
  mesh.sendBroadcast(msg);
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

// Runs when a message is received on the mesh network
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

// Runs when a new node joins the mesh network
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

// Runs when a new node joins or leaves the network
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

// Runs when the network adjusts the time, so that all nodes are synced
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

// void setup() {

// }

// void loop() {

// }