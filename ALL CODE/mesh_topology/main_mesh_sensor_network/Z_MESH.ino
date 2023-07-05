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
//   Serial.printf("############################################\n");
//   Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  
//   DynamicJsonDocument doc(512);
//   deserializeJson(doc, msg);

//   int node_num = doc["node_num"];
//   double tempC = doc["tempC"];
//   double tempF = doc["tempF"];
//   double hum = doc["hum"];
//   double CO = doc["CO"];
//   double CO2 = doc["CO2"];
//   float pm25 = doc["PM_2.5"];
//   float pm10 = doc["PM_10"];

//   if (node_num != 0){ Serial.printf("Node Num: %i\n", node_num); }
//   if ((tempC != 0) && (tempF != 0)){ Serial.printf("Temperature: %f C; %f F\n", tempC, tempF); }
//   if (hum != 0){ Serial.printf("Humidity: %f \%\n", hum); }
//   if (CO != 0){ Serial.printf("CO: %f ug/m^3\n", CO); }
//   if (CO2 != 0){ Serial.printf("CO2: %f ug/m^3\n", CO2); }
//   if (pm25 != 0){ Serial.printf("PM 2.5: %f **UNIT**\n", pm25); }
//   if (pm10 != 0){ Serial.printf("PM 10: %f **UNIT**\n", pm10); }

//   ####################################################################
//   NODE NAMES
//   ####################################################################
//   521153793
//   521154105
}

// Runs when a new node joins the mesh network
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

// Runs when a new node joins or leaves the network
void changedConnectionCallback() {
  // Serial.printf("Changed connections\n");
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

  userScheduler.addTask(taskSendMessage_DHT22);
  userScheduler.addTask(taskSendMessage_MQ135);
  userScheduler.addTask(taskSendMessage_SDS011);
  userScheduler.addTask(taskCheckLatency);
  userScheduler.addTask(taskCalculateThroughput);
}

void loop_MESH() {
  if (mesh.isConnected(gateway_mesh_ID)){
    taskSendMessage_DHT22.enableIfNot();
    taskSendMessage_MQ135.enableIfNot();
    taskSendMessage_SDS011.enableIfNot();
    taskCheckLatency.enableIfNot();
    taskCalculateThroughput.enableIfNot();
  } else {
    taskSendMessage_DHT22.disable();
    taskSendMessage_MQ135.disable();
    taskSendMessage_SDS011.disable();
    taskCheckLatency.disable();
    taskCalculateThroughput.disable();
  }
  mesh.update();
}