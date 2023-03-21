Task taskSendMessage_DHT22(TASK_SECOND * 2, TASK_FOREVER, &sendMessage_DHT22);
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

String getReadings_DHT22(){
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["hum"] = dht_sensor.readHumidity();
  jsonReadings["tempC"] = dht_sensor.readTemperature();
  jsonReadings["tempF"] = dht_sensor.readTemperature(true);
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void setup_DHT22(){
  dht_sensor.begin();
}

void loop_DHT22(){

}