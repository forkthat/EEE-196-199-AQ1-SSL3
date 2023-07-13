/*
  ####################################################################
  DONE: 
    Use SINGLE message to send message to mesh-facing gateway ESP32
    https://gitlab.com/painlessMesh/painlessMesh/-/wikis/home

    on message receive, send data to gateway
  ####################################################################
*/

unsigned long currentMillis, previousMillis;

void sendToGateway(uint32_t gateway_mesh_ID, String &msg) {
  if(mesh.sendSingle(gateway_mesh_ID, msg)){
    Serial.printf("Sending SUCCESS to %u msg=%s\n", gateway_mesh_ID, msg.c_str());
    // size in bytes
    msg_size += msg.length();
    msg_sent_success += 1;
    if (flag_MTBF_uptime == false) {
      flag_MTBF_change = true;
    }
    flag_MTBF_uptime = true;
  } else {
    Serial.printf("Sending FAILED to %u msg=%s\n", gateway_mesh_ID, msg.c_str());
    msg_sent_fail += 1;
    if (flag_MTBF_uptime == true) {
      flag_MTBF_change = true;
    }
    flag_MTBF_uptime = false;
  }
  calculate_MTBF();
}

void sendMessage_DHT22() {
  String msg = getReadings_DHT22();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_DHT22.setInterval(random(taskSendMsg_DHT22_seconds_low, taskSendMsg_DHT22_seconds_high));
}

void sendMessage_MQ135() {
  String msg = getReadings_MQ135();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_MQ135.setInterval(random(taskSendMsg_MQ135_seconds_low, taskSendMsg_MQ135_seconds_high));
}

void sendMessage_SDS011() {
  String msg = getReadings_SDS011();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_SDS011.setInterval(random(taskSendMsg_SDS011_seconds_low, taskSendMsg_SDS011_seconds_high));
}

/*
  ####################################################################
  NETWORK PARAMETERS
    > Latency
    > Throughput
    > MTBF
  ####################################################################
*/

void sendMessage_Latency() {
  mesh.startDelayMeas(gateway_mesh_ID);
  task_Latency.setInterval(random(taskLatency_rate_seconds_low, taskLatency_rate_seconds_high));
}

void start_Throughput() {
  task_Throughput.setCallback(&end_Throughput);
  delay(taskThroughput_rate_seconds);
}

void end_Throughput() {
  String msg = getReadings_Throughput();
  sendToGateway(gateway_mesh_ID, msg);
  task_Throughput.setCallback(&start_Throughput);
}

void sendMessage_MTBF() {
  String msg = getReadings_MTBF();
  sendToGateway(gateway_mesh_ID, msg);
  task_MTBF.setInterval(random(taskMTBF_rate_seconds_low, taskMTBF_rate_seconds_high));
}