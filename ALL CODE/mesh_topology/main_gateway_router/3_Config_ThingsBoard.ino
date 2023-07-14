void setup_thingsboard() {
  lastSend = 0;
}

void loop_thingsboard() {
  if (!tb.connected()) {
    while (!tb.connected()) {
      Serial.print("Connecting to ThingsBoard node ...");
      if (tb.connect(thingsboardServer, TOKEN)) {
        Serial.println("[DONE]");
      } else {
        Serial.print("[FAILED]");
        Serial.println(" : retrying in 5 seconds]");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }

  // float pm25_result = pm25 / 10;
  // float pm10_result = pm10 / 10;

  //send data to thingsboard
  if (millis() - lastSend > interval_TB) {  // Update and send only after interval_TB seconds
    tb.sendTelemetryFloat(key_name_temp, tempC);
    tb.sendTelemetryFloat(key_name_hum, hum);
    tb.sendTelemetryFloat(key_name_CO, CO);
    tb.sendTelemetryFloat(key_name_CO2, CO2);
    tb.sendTelemetryFloat(key_name_PM25, pm25);
    tb.sendTelemetryFloat(key_name_PM10, pm10);
    tb.sendTelemetryFloat(key_name_packet_loss, packet_loss);
    tb.sendTelemetryFloat(key_name_latency, latency);
    tb.sendTelemetryFloat(key_name_throughput, throughput);
    tb.sendTelemetryFloat(key_name_MTBF, MTBF);
    lastSend = millis();
  }
  tb.loop();
}
