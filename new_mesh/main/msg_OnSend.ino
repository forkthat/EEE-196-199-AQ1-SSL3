void sendMessage_DHT22() {
  String msg = getReadings_DHT22();
  mesh.sendBroadcast(msg);
  taskSendMessage_DHT22.setInterval(TASK_SECOND * 2);
}

void sendMessage_MQ135() {
  String msg = getReadings_MQ135();
  mesh.sendBroadcast(msg);
  taskSendMessage_MQ135.setInterval(TASK_SECOND * 5);
}