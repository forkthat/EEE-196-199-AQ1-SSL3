Task taskSendMessage_DHT22(taskSendMsg_DHT22_seconds, TASK_FOREVER, &sendMessage_DHT22);
Task taskSendMessage_MQ135(taskSendMsg_MQ135_seconds, TASK_FOREVER, &sendMessage_MQ135);
Task taskSendMessage_SDS011(taskSendMsg_SDS011_seconds, TASK_FOREVER, &sendMessage_SDS011);
Task taskCheckLatency(taskCheckLatency_interval_seconds, TASK_FOREVER, &checkLatency);
Task taskcalculateThroughput(taskcalculateThroughput_interval_seconds, TASK_FOREVER, &calculateThroughput);

/*
  ####################################################################
  DONE: 
    Use SINGLE message to send message to mesh-facing gateway ESP32
    https://gitlab.com/painlessMesh/painlessMesh/-/wikis/home

    on message receive, send data to gateway
  ####################################################################
*/

unsigned int msg_size = 0;
unsigned long startTime = 0;
double throughput;

void sendToGateway(uint32_t gateway_mesh_ID, String &msg){
  // size in bytes
  msg_size += msg.length();

  if(mesh.sendSingle(gateway_mesh_ID, msg)){
    Serial.printf("Sending SUCCESS to %u msg=%s\n", gateway_mesh_ID, msg.c_str());
    msg_sent_success += 1;
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

void checkLatency(){
  mesh.startDelayMeas(gateway_mesh_ID);
  taskSendMessage_SDS011.setInterval(taskCheckLatency_interval_seconds);
}

void calculateThroughput(){
  startTime = millis();

  if((millis() - startTime) >= 10){
    //            {    bits    }   {kilo}    {per second}
    throughput = ((msg_size * 8) / (1000)) / 10;
    msg_size = 0;
  }
}