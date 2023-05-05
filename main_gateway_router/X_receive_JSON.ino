#include <ArduinoJson.h>

void setup_receive_JSON()
{
  
}

void loop_receive_JSON()
{
  if (Serial2.available())
  {
    // Allocate the JSON document
    // This one must be bigger than the sender's because it must store the strings
    StaticJsonDocument<300> doc;

    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(doc, Serial2);

    if (err == DeserializationError::Ok)
    {
      uint32_t node = doc["node"];
      double tempC = doc["tempC"];
      double tempF = doc["tempF"];
      double hum = doc["hum"];
      double CO = doc["CO"];
      double CO2 = doc["CO2"];
      double pm25 = doc["PM 2.5"];
      double pm10 = doc["PM 10"];
      uint32_t msg_sent_success = doc["msg_success"].as<uint32_t>();
      uint32_t msg_sent_fail = doc["msg_fail"].as<uint32_t>();

      if (node > 0) { 
        Serial.printf("==========\nNode: %u \n", doc["node"].as<uint32_t>()); 
      }
      if (msg_sent_success > 0) { 
        Serial.printf("Success: %u \n", doc["msg_success"].as<uint32_t>()); 
      }
      if (msg_sent_fail > 0) { 
        Serial.printf("Fail: %u \n", doc["msg_fail"].as<uint32_t>()); 
      }

      double total_success_fail = msg_sent_success + msg_sent_fail;
      double packet_loss = msg_sent_fail / total_success_fail;
      Serial.printf("Packet loss: %lf%% \n", packet_loss*100); 

      /* 
      #######################################################
        Sensors
      #######################################################
      */

      if ((tempC > 0) && (tempF > 0)) { 
        Serial.printf("Temperature: %lf C; %lf F \n", doc["tempC"].as<double>(), doc["tempF"].as<double>()); 
      }
      if (hum > 0) { 
        Serial.printf("Humidity: %lf %% \n", doc["hum"].as<double>()); 
      }

      /* 
      #######################################################
        Identify the units of CO, CO2, PM2.5, and PM10
      #######################################################
      */

      if (CO > 0) { 
        Serial.printf("CO: %lf INSERT_UNITS_HERE \n", doc["CO"].as<double>()); 
      }
      if (CO2 > 0) { 
        Serial.printf("CO2: %lf INSERT_UNITS_HERE \n", doc["CO2"].as<double>());
      }
      if (pm25 > 0) { 
        Serial.printf("PM 2.5: %lf INSERT_UNITS_HERE \n", pm25); 
      }
      if (pm10 > 0) { 
        Serial.printf("PM 10: %lf INSERT_UNITS_HERE \n", pm10); 
      }
    } 
    else 
    {
      // Print error to the "debug" serial port
      // Serial.print("deserializeJson() returned ");
      // Serial.println(err.c_str());
  
      // Flush all bytes in the "link" serial port buffer
      while (Serial2.available() > 0)
        Serial2.read();
    }
  }
}