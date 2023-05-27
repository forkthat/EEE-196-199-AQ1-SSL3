#include <ArduinoJson.h>
#include <sstream>

#define MAX_STRING_LENGTH 10

uint32_t node_num, msg_sent_success, msg_sent_fail;
double tempC, hum, CO, CO2, pm25, pm10;

// char key_name_temp[] = "N# Temp";
// char key_name_hum[]  = "N# Hum";
// char key_name_CO[]   = "N# CO";
// char key_name_CO2[]  = "N# CO2";
// char key_name_PM25[] = "N# PM2.5";
// char key_name_PM10[] = "N# PM10";

String node_N = "N";
String str_key_name_temp;
String str_key_name_hum;
String str_key_name_CO;
String str_key_name_CO2;
String str_key_name_PM25;
String str_key_name_PM10;

char key_name_temp[10];
char key_name_hum[10];
char key_name_CO[10];
char key_name_CO2[10];
char key_name_PM25[10];
char key_name_PM10[10];

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

      if (doc["node_num"].as<uint32_t>() > 0) { 
        node_num = doc["node_num"].as<uint32_t>();
        Serial.printf("==========\nNode: %u \n", node_num);

        if (doc["tempC"].as<double>() > 0) { 
          str_key_name_temp = node_N + node_num + " Temp";
          str_key_name_temp.toCharArray(key_name_temp, 10);

          tempC = doc["tempC"].as<double>();
          Serial.printf("Temperature: %lf C \n", doc["tempC"].as<double>()); 
        }
        if (doc["hum"].as<double>() > 0) { 
          str_key_name_hum = node_N + node_num + " Hum";
          str_key_name_hum.toCharArray(key_name_hum, 10);

          hum = doc["hum"].as<double>();
          Serial.printf("Humidity: %lf %% \n", doc["hum"].as<double>()); 
        }
        if (doc["CO"].as<double>() > 0) { 
          str_key_name_CO = node_N + node_num + " CO";
          str_key_name_CO.toCharArray(key_name_CO, 10);

          CO = doc["CO"].as<double>();
          Serial.printf("CO: %lf INSERT_UNITS_HERE \n", doc["CO"].as<double>()); 
        }
        if (doc["CO2"].as<double>() > 0) { 
          str_key_name_CO2 = node_N + node_num + " CO2";
          str_key_name_CO2.toCharArray(key_name_CO2, 10);

          CO2 = doc["CO2"].as<double>();
          Serial.printf("CO2: %lf INSERT_UNITS_HERE \n", doc["CO2"].as<double>());
        }
        if (doc["PM_2.5"] > 0) { 
          str_key_name_PM25 = node_N + node_num + " PM2.5";
          str_key_name_PM25.toCharArray(key_name_PM25, 10);

          pm25 = doc["PM_2.5"];
          Serial.printf("PM 2.5: %lf INSERT_UNITS_HERE \n", doc["PM_2.5"].as<double>()); 
        }
        if (doc["PM_10"] > 0) { 
          str_key_name_PM10 = node_N + node_num + " PM10";
          str_key_name_PM10.toCharArray(key_name_PM10, 10);

          pm10 = doc["PM_10"];
          Serial.printf("PM 10: %lf INSERT_UNITS_HERE \n", doc["PM_10"].as<double>()); 
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