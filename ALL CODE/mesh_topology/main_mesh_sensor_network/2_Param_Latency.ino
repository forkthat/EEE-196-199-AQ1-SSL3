void receivedDelayCallback(uint32_t nodeId, int32_t delay){
  Serial.printf("Latency from %i: %i", nodeId, delay);
  latency_data[latency_index] = delay;
  latency_index += 1;

  if (latency_index >= 10){
    for(int i = 0; i < 10; i++){
      sum_Latency += latency_data[i];
    }
    ave_Latency = sum_Latency/10;
    latency_index = 0;
  }

  StaticJsonDocument<128> doc;
  doc["node"] = nodeNumber;
  doc["msg_s"] = msg_sent_success;
  doc["msg_f"] = msg_sent_fail;
  doc["lat"] = ave_Latency;
  msg_Latency = doc.as<String>();
  
  sendToGateway(gateway_mesh_ID, msg_Latency);
}