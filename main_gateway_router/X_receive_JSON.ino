#include <ArduinoJson.h>

uint32_t node, msg_sent_success, msg_sent_fail;
float tempC, hum, CO, CO2, pm25, pm10;

char final_temp[] = "N1 Temp";
char final_hum[] = "N1 Humidity";
char final_CO[] = "N1 CO";
char final_CO2[] = "N1 CO2";
char final_PM25[] = "N1 PM2.5";
char final_PM10[] = "N1 PM10";

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
      /* 
      #######################################################
        Display on Serial
      #######################################################
      */

      if (doc["node"].as<uint32_t>() > 0) { 
        node = doc["node"].as<uint32_t>();
        Serial.printf("==========\nNode: %u \n", doc["node"].as<uint32_t>());

        if (doc["tempC"].as<double>() > 0) { 
          final_temp[1] = char(node);
          tempC = doc["tempC"].as<double>();
          Serial.printf("Temperature: %lf C \n", doc["tempC"].as<double>()); 
        }
        if (doc["hum"].as<double>() > 0) { 
          final_hum[1] = char(node);
          hum = doc["hum"].as<double>();
          Serial.printf("Humidity: %lf %% \n", doc["hum"].as<double>()); 
        }
        if (doc["CO"].as<double>() > 0) { 
          final_CO[1] = char(node);
          CO = doc["CO"].as<double>();
          Serial.printf("CO: %lf INSERT_UNITS_HERE \n", doc["CO"].as<double>()); 
        }
        if (doc["CO2"].as<double>() > 0) { 
          final_CO2[1] = char(node);
          CO2 = doc["CO2"].as<double>();
          Serial.printf("CO2: %lf INSERT_UNITS_HERE \n", doc["CO2"].as<double>());
        }
        if (doc["PM 2.5"] > 0) { 
          final_PM25[1] = char(node);
          pm25 = doc["PM 2.5"];
          Serial.printf("PM 2.5: %lf INSERT_UNITS_HERE \n", doc["PM 2.5"].as<double>()); 
        }
        if (doc["PM 10"] > 0) { 
          final_PM10[1] = char(node);
          pm10 = doc["PM 10"];
          Serial.printf("PM 10: %lf INSERT_UNITS_HERE \n", doc["PM 10"].as<double>()); 
        }
      }

      /* 
      #######################################################
        Packet Loss
      #######################################################
      */

      // if (doc["msg_success"].as<uint32_t>() > 0) { 
      //   msg_sent_success = doc["msg_success"].as<uint32_t>();
      //   Serial.printf("Success: %u \n", doc["msg_success"].as<uint32_t>()); 
      // }
      // if (doc["msg_fail"].as<uint32_t>() > 0) { 
      //   msg_sent_fail = doc["msg_fail"].as<uint32_t>();
      //   Serial.printf("Fail: %u \n", doc["msg_fail"].as<uint32_t>()); 
      // }

      // double total_success_fail = msg_sent_success + msg_sent_fail;
      // double packet_loss = msg_sent_fail / total_success_fail;
      // Serial.printf("Packet loss: %lf%% \n", packet_loss*100); 
      
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