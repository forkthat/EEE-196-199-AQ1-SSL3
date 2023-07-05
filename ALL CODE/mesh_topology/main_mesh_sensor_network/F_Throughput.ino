unsigned int msg_size = 0;
unsigned long startTime = 0;
int throughput_period_seconds = 15;
double throughput = 0;

String getReadings_Throughput(){
  StaticJsonDocument<128> doc;

  doc["node_num"] = nodeNumber;
  doc["msg_success"] = msg_sent_success;
  doc["msg_fail"] = msg_sent_fail;

  doc["Throughput"] = throughput;

  return doc.as<String>();
}