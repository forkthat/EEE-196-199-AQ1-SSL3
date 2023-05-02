/*
####################################################################
TODO: 
  on message receive, send data to thingsboard
####################################################################
*/

#include <ArduinoJson.h>

#define RXD2 16
#define TXD2 17

void setup(){
  Serial.begin(115200);
  while (!Serial) continue;

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while(!Serial2) continue;
}

void loop(){
  if (Serial2.available())
  {
    // Allocate the JSON document
    // This one must be bigger than the sender's because it must store the strings
    StaticJsonDocument<300> doc;

    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(doc, Serial2);

    if (err == DeserializationError::Ok)
    {
      // Print the values
      // (we must use as<T>() to resolve the ambiguity)
      // Serial.print("timestamp = ");
      // Serial.println(doc["timestamp"].as<long>());
      // Serial.print("value = ");
      // Serial.println(doc["value"].as<int>());

      int node = doc["node"];
      double tempC = doc["tempC"];
      double tempF = doc["tempF"];
      double hum = doc["hum"];
      double CO = doc["CO"];
      double CO2 = doc["CO2"];
      double pm25 = doc["PM 2.5"];
      double pm10 = doc["PM 10"];

      if (node != 0) { 
        Serial.printf("Node: %i \n", doc["node"].as<int>()); 
      }
      if ((tempC != 0) && (tempF != 0)) { 
        Serial.printf("Temperature: %f C; %f F \n", doc["tempC"].as<double>(), doc["tempF"].as<double>()); 
      }
      if (hum != 0) { 
        Serial.printf("Humidity: %f %% \n", doc["hum"].as<double>()); 
      }
      if (CO != 0) { 
        Serial.printf("CO: %f INSERT_UNITS_HERE \n", doc["CO"].as<double>()); 
      }
      if (CO2 != 0) { 
        Serial.printf("CO2: %f INSERT_UNITS_HERE \n", doc["CO2"].as<double>());
      }
      if (pm25 != 0) { 
        Serial.printf("PM 2.5: %f ug/m^3 \n", pm25); 
      }
      if (pm10 != 0) { 
        Serial.printf("PM 10: %f ug/m^3 \n", pm10); 
      }
    } 
    else 
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());
  
      // Flush all bytes in the "link" serial port buffer
      while (Serial2.available() > 0)
        Serial2.read();
    }
  }
}