float tempC = dht_sensor.readTemperature();
float humi  = dht_sensor.readHumidity();

void setup_DHT22() {
  dht_sensor.begin();
}

void loop_DHT22() {
  Serial.println("Temperature: " + String(tempC) + " C");
  Serial.println("Humidity: " + String(humi) + "%");
  if (isnan(tempC) || isnan(humi) || tempC == 0.00 || humi == 0.00) { //don't send to thingsboard for temp and humi if 0 or no value
  }
  else {
    tb.sendTelemetryFloat("N1 Temperature", tempC);
    tb.sendTelemetryFloat("N1 Humidity", humi);
  }
}
