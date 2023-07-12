String getReadings_Throughput(){
  throughput = ((msg_size * 8) / (1000)) / taskThroughput_rate_seconds;
  msg_size = 0;

  StaticJsonDocument<128> doc;

  doc["node_num"] = nodeNumber;
  doc["msg_success"] = msg_sent_success;
  doc["msg_fail"] = msg_sent_fail;
  doc["throughput"] = throughput;

  return doc.as<String>();
}