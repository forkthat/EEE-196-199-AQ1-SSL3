void receivedDelayCallback(uint32_t nodeId, int32_t delay){
  Serial.printf("Latency from %i: %i", nodeId, delay);
  latency_data[latency_index] = delay;
  latency_index += 1;

  if (latency_index >= 10){
    for(int i = 0; i < 10; i++){
      sum += latency_data[i];
    }
    ave_latency = sum/10;
    latency_index = 0;
  }

  StaticJsonDocument<128> doc;
  doc["node"] = nodeNumber;
  doc["msg_s"] = msg_sent_success;
  doc["msg_f"] = msg_sent_fail;
  
  doc["lat"] = ave_latency;

  msg_Latency = doc.as<String>();
  flag_delay_received = true;
}