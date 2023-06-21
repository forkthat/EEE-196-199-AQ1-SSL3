char thingsboardServer[] = "thingsboard.cloud";

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
  //send data to thingsboard
  if (millis() - lastSend > 5000) {  // Update and send only after 5 seconds; PREV: 1 SEC
    loop_DHT22();
    loop_MQ135();
    loop_SDS011();
    lastSend = millis();
  }
  tb.loop();
}
