DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

String getReadings_DHT22(){
  DynamicJsonDocument doc(512);

  doc["node"] = nodeNumber;
  doc["hum"] = dht_sensor.readHumidity();
  doc["tempC"] = dht_sensor.readTemperature();
  doc["tempF"] = dht_sensor.readTemperature(true);

  return doc.as<String>();
}

void setup_DHT22(){
  dht_sensor.begin();
}

void loop_DHT22(){

}