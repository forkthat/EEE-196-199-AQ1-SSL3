/*
####################################################################
https://randomnerdtutorials.com/esp-mesh-esp32-esp8266-painlessmesh/
####################################################################
*/

void sendMessage() {
//   String msg = "Hi from node1";
//   msg += mesh.getNodeId();
//   mesh.sendBroadcast( msg );
//   taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Runs when a message is received on the mesh network
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  DynamicJsonDocument doc(2048);
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