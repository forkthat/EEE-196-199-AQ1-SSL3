String getReadings_DHT22(){
  StaticJsonDocument<128> doc;
  doc["node"] = nodeNumber;
  doc["msg_s"] = msg_sent_success;
  doc["msg_f"] = msg_sent_fail;
  doc["hum"] = dht_sensor.readHumidity();
  doc["temp"] = dht_sensor.readTemperature();
  // doc["tempF"] = dht_sensor.readTemperature(true);
  return doc.as<String>();
}

void setup_DHT22(){
  dht_sensor.begin();
}

void loop_DHT22(){

}