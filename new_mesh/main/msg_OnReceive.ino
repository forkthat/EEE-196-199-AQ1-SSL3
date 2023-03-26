/*
####################################################################
TODO: 
  on receive, send data to thingsboard
####################################################################
*/

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("############################################\n");
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  
  // JSONVar myObject = JSON.parse(msg.c_str());
  DynamicJsonDocument doc(512);
  deserializeJson(doc, msg);

  // Serial.printf("myObject: %s\n", myObject);

  // int node = myObject["node"];
  // double tempC = myObject["tempC"];
  // double tempF = myObject["tempF"];
  // double hum = myObject["hum"];
  // double CO = myObject["CO"];
  // double CO2 = myObject["CO2"];

  int node = doc["node"];
  double tempC = doc["tempC"];
  double tempF = doc["tempF"];
  double hum = doc["hum"];
  double CO = doc["CO"];
  double CO2 = doc["CO2"];

  if (node != 0){ Serial.printf("Node: %i\n", node); }
  if ((tempC != 0) && (tempF != 0)){ Serial.printf("Temperature: %f C; %f F\n", tempC, tempF); }
  if (hum != 0){ Serial.printf("Humidity: %f \%\n", hum); }
  if (CO != 0){ Serial.printf("CO: %f ug/m^3\n", CO); }
  if (CO2 != 0){ Serial.printf("CO2: %f ug/m^3\n", CO2); }
}