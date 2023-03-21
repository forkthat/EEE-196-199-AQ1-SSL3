/*
####################################################################
TODO: 
  on receive, send data to thingsboard
####################################################################
*/

void receivedCallback_DHT22( uint32_t from, String &msg ) {
  Serial.printf("############################################");
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  
  JSONVar myObject = JSON.parse(msg.c_str());

  int node = myObject["node"];
  double hum = myObject["hum"];
  double tempC = myObject["tempC"];
  double tempF = myObject["tempF"];

  Serial.printf("Node: %i\n", node);
  Serial.printf("Temperature: %f C; %f F\n", tempC, tempF);
  Serial.printf("Humidity: %f \%\n", hum);
}

void receivedCallback_MQ135( uint32_t from, String &msg ) {
  Serial.printf("############################################");
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  
  JSONVar myObject = JSON.parse(msg.c_str());

  double CO = myObject["CO"];
  double CO2 = myObject["CO2"];

  Serial.print("CO: ");
  Serial.print(CO);
  Serial.println(" ug/m^3");
  Serial.print("CO2: ");
  Serial.print(CO2); //in microgram/m^3
  Serial.println(" ug/m^3");
}