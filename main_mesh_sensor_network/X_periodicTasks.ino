Task taskSendMessage_DHT22(taskSendMsg_DHT22_seconds, TASK_FOREVER, &sendMessage_DHT22);
Task taskSendMessage_MQ135(taskSendMsg_MQ135_seconds, TASK_FOREVER, &sendMessage_MQ135);
Task taskSendMessage_SDS011(taskSendMsg_SDS011_seconds, TASK_FOREVER, &SendMessage_SDS011);

/*
  ####################################################################
  TODO: 
    Use SINGLE message to send message to mesh-facing gateway ESP32
    https://gitlab.com/painlessMesh/painlessMesh/-/wikis/home
  ####################################################################
*/

uint32_t gateway_mesh_ID = 1843304141;

void sendToGateway(uint32_t gateway_mesh_ID, String &msg){
  if(mesh.sendSingle(gateway_mesh_ID, msg)){
    Serial.printf("Sending success to %u msg=%s\n", gateway_mesh_ID, msg.c_str());
  } else {
    Serial.printf("Sending failed to %u msg=%s\n", gateway_mesh_ID, msg.c_str());
  }
}

void sendMessage_DHT22(){
  String msg = getReadings_DHT22();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_DHT22.setInterval(taskSendMsg_DHT22_seconds);
}

void sendMessage_MQ135(){
  String msg = getReadings_MQ135();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_MQ135.setInterval(taskSendMsg_MQ135_seconds);
}

void SendMessage_SDS011(){
  String msg = getReadings_SDS011();
  sendToGateway(gateway_mesh_ID, msg);
  taskSendMessage_SDS011.setInterval(taskSendMsg_SDS011_seconds);
}