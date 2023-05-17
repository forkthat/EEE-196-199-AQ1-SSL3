DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

String getReadings_DHT22(){
  StaticJsonDocument<128> doc;

  doc["node"] = nodeNumber;
  doc["msg_success"] = msg_sent_success;
  doc["msg_fail"] = msg_sent_fail;

  doc["hum"] = dht_sensor.readHumidity();
  doc["tempC"] = dht_sensor.readTemperature();
  // doc["tempF"] = dht_sensor.readTemperature(true);

  return doc.as<String>();
}

void setup_DHT22(){
  dht_sensor.begin();
}

void loop_DHT22(){

}