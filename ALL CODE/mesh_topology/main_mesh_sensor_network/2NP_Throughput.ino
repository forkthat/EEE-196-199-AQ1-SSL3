// every hour
int calcThroughput_rate_seconds = 60 * 60;
unsigned long taskCalculateThroughput_rate_seconds = TASK_SECOND * calcThroughput_rate_seconds;

// every hour
int sendMsgThroughput_rate_seconds = 60 * 60;
unsigned long taskSendMsg_Throughput_seconds = TASK_SECOND * sendMsgThroughput_rate_seconds;
unsigned long taskSendMsg_Throughput_seconds_low = TASK_SECOND * (sendMsgThroughput_rate_seconds + 1);
unsigned long taskSendMsg_Throughput_seconds_high = TASK_SECOND * (sendMsgThroughput_rate_seconds + 5);

unsigned int msg_size = 0;
unsigned long startTime = 0;
double throughput = 0;

String getReadings_Throughput(){
  StaticJsonDocument<128> doc;

  doc["node_num"] = nodeNumber;
  doc["msg_success"] = msg_sent_success;
  doc["msg_fail"] = msg_sent_fail;

  doc["Throughput"] = throughput;

  return doc.as<String>();
}