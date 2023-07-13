void receivedDelayCallback(uint32_t nodeId, int32_t delay){
  Serial.printf("Latency from %zu: %zu \n", nodeId, delay);

  StaticJsonDocument<128> doc;
  doc["node"] = nodeNumber;
  doc["msg_s"] = msg_sent_success;
  doc["msg_f"] = msg_sent_fail;
  doc["lat"] = delay/pow(10, 6);      // convert microseconds to seconds
  msg_Latency = doc.as<String>();
  
  sendToGateway(gateway_mesh_ID, msg_Latency);
}