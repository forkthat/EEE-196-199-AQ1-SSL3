String getReadings_MTBF(){
  StaticJsonDocument<128> doc;
  doc["node"] = nodeNumber;
  doc["msg_s"] = msg_sent_success;
  doc["msg_f"] = msg_sent_fail;
  doc["MTBF"] = calcu_MTBF;
  return doc.as<String>();
}

void calculate_MTBF() {
  num_failures += 1;
  sum_MTBF += (millis() - timer_MTBF);
  calcu_MTBF = sum_MTBF / num_failures;
  timer_MTBF = millis();
}