String getReadings_MTBF(){
  StaticJsonDocument<128> doc;
  doc["node"] = nodeNumber;
  doc["msg_s"] = msg_sent_success;
  doc["msg_f"] = msg_sent_fail;
  doc["MTBF"] = calcu_MTBF;
  return doc.as<String>();
}

void calculate_MTBF() {
  if (timer_MTBF_start > timer_MTBF_end){
    diff_MTBF += 0;
  } else {
    diff_MTBF = (timer_MTBF_end - timer_MTBF_start);
    diff_MTBF_prev = diff_MTBF;
  }

  if (num_failures > 0) {
    calcu_MTBF = ((double)sum_MTBF / (double)num_failures) / 1000;
  }

  if (flag_MTBF_uptime == true){
    /*
    Note to future reader: 
    I have no idea why this is in reverse.
    If there was a change from low to high, and the uptime starts
    then timer_MTBF_start should be the one to use.
    I have no clue what I'm doing.

    Ammend:
    Okay, so timer_MTBF_end should be the one moving since the system is in uptime.
    And when it is downtime, the timer_MTBF_start should be the one moving.
    I get it now.
    */
    timer_MTBF_end = millis();
  } else {
    timer_MTBF_start = millis();
  }

  if (flag_MTBF_change == true){ 
    num_failures += 1; 
    sum_MTBF += diff_MTBF_prev;
    flag_MTBF_change = false;
  }
  
  // Serial.printf("num_failures: %lu \t", num_failures);
  // Serial.printf("timer_MTBF_start: %llu \t", timer_MTBF_start);
  // Serial.printf("timer_MTBF_end: %llu \t", timer_MTBF_end);
  // Serial.printf("diff_MTBF: %llu \t", diff_MTBF);
  // Serial.printf("sum_MTBF: %llf \t", sum_MTBF);
  // Serial.printf("calcu_MTBF: %llf \n", calcu_MTBF);
}

void loop_MTBF(){
  calculate_MTBF();
}