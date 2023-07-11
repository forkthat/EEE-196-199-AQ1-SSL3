/*
  ####################################################################
  DONE: 
    Use SINGLE message to send message to mesh-facing gateway ESP32
    https://gitlab.com/painlessMesh/painlessMesh/-/wikis/home

    on message receive, send data to gateway
  ####################################################################
*/

unsigned long currentMillis, previousMillis;

void sendToGateway(uint32_t gateway_mesh_ID, String &msg){
  if(mesh.sendSingle(gateway_mesh_ID, msg)){
    Serial.printf("Sending SUCCESS to %u msg=%s\n", gateway_mesh_ID, msg.c_str());
    msg_sent_success += 1;
    // size in bytes
    msg_size += msg.length();
  } else {
    Serial.printf("Sending FAILED to %u msg=%s\n", gateway_mesh_ID, msg.c_str());
    msg_sent_fail += 1;
  }
}

void sendMessage_DHT22(){
  String msg = getReadings_DHT22();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_DHT22.setInterval(random(taskSendMsg_DHT22_seconds_low, taskSendMsg_DHT22_seconds_high));
}

void sendMessage_MQ135(){
  String msg = getReadings_MQ135();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_MQ135.setInterval(random(taskSendMsg_MQ135_seconds_low, taskSendMsg_MQ135_seconds_high));
}

void sendMessage_SDS011(){
  String msg = getReadings_SDS011();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_SDS011.setInterval(random(taskSendMsg_SDS011_seconds_low, taskSendMsg_SDS011_seconds_high));
}

/*
  ####################################################################
  NETWORK PARAMETERS
    > Latency
    > Throughput
  ####################################################################
*/

void sendMessage_Latency(){
  mesh.startDelayMeas(gateway_mesh_ID);
  if(flag_delay_received == true){
    sendToGateway(gateway_mesh_ID, msg_Latency);
    flag_delay_received = false;
  }
  task_Latency.setInterval(taskLatency_rate_seconds);
}

void start_Throughput(){
  task_Throughput.setCallback(&end_Throughput);
  delay(taskThroughput_rate_seconds);
}

void end_Throughput(){
  String msg_Throughput = getReadings_Throughput();
  sendToGateway(gateway_mesh_ID, msg_Throughput);
  task_Throughput.setCallback(&start_Throughput);
}