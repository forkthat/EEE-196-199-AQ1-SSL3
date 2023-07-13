String getReadings_Throughput() {
  throughput = (double)(msg_size * 8) / (double)taskThroughput_rate_seconds;
  msg_size = 0;

  StaticJsonDocument<128> doc;
  doc["node"] = nodeNumber;
  doc["msg_s"] = msg_sent_success;
  doc["msg_f"] = msg_sent_fail;
  doc["thr_bps"] = throughput;
  return doc.as<String>();
}