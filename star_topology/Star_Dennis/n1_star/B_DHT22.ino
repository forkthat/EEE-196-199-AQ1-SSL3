DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);


void setup_DHT22(){
  dht_sensor.begin();
}

void loop_DHT22(){
  float tempC = dht_sensor.readTemperature();
  float humi  = dht_sensor.readHumidity();
  Serial.println("Temperature: " + String(tempC) + " C");
  Serial.println("Humidity: " + String(humi) + "%");
  tb.sendTelemetryFloat("N1 Temperature", tempC);
  tb.sendTelemetryFloat("N1 Humidity", humi);
}


