void setup_receive_JSON() {
  
}

void loop_receive_JSON() {
  if (Serial2.available()) {
    // Allocate the JSON document
    // This one must be bigger than the sender's because it must store the strings
    DynamicJsonDocument doc(2048);

    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(doc, Serial2);

    if (err == DeserializationError::Ok) { 
      /* 
      #######################################################
        Sensor Values
      #######################################################
      */
      node_num = doc["node"].as<uint32_t>();

      if (node_num > 0) { 
        Serial.printf("============\nNode: %u \n", node_num);

        tempC = doc["temp"].as<double>();
        hum = doc["hum"].as<double>();
        pm25 = doc["PM_2.5"].as<double>();
        pm10 = doc["PM_10"].as<double>();

        if (tempC > 0 && tempC < 100) { 
          str_key_name_temp = node_N + node_num + " Temp";
          str_key_name_temp.toCharArray(key_name_temp, 10);
          Serial.printf("Temperature: %lf C \n", tempC); 
        }
        if (hum > 0 && hum < 100) { 
          str_key_name_hum = node_N + node_num + " Hum";
          str_key_name_hum.toCharArray(key_name_hum, 10);
          Serial.printf("Humidity: %lf %% \n", hum); 
        }
        if (doc["CO"].as<String>() == "null") {
          CO = 0;
          str_key_name_CO = node_N + node_num + " CO";
          str_key_name_CO.toCharArray(key_name_CO, 10);
          Serial.printf("CO: %lf ug/m^3 \n", 0); 
        } else if (doc["CO"].as<double>() > 0) {
          CO = doc["CO"].as<double>();
          str_key_name_CO = node_N + node_num + " CO";
          str_key_name_CO.toCharArray(key_name_CO, 10);
          Serial.printf("CO: %lf ug/m^3 \n", CO); 
        }
        // if (CO2 == "null") {
        //   CO2 = 0;
        //   str_key_name_CO2 = node_N + node_num + " CO2";
        //   str_key_name_CO2.toCharArray(key_name_CO2, 10);
        //   Serial.printf("CO2: %lf ug/m^3 \n", 0); 
        // }
        // if (CO2 > 0) { 
        //   str_key_name_CO2 = node_N + node_num + " CO2";
        //   str_key_name_CO2.toCharArray(key_name_CO2, 10);
        //   Serial.printf("CO2: %lf ug/m^3 \n", CO2);
        // }
        if (pm25 > 0) { 
          str_key_name_PM25 = node_N + node_num + " PM2.5";
          str_key_name_PM25.toCharArray(key_name_PM25, 10);
          Serial.printf("PM 2.5: %lf ug/m^3 \n", pm25); 
        }
        if (pm10 > 0) { 
          str_key_name_PM10 = node_N + node_num + " PM10";
          str_key_name_PM10.toCharArray(key_name_PM10, 10);
          Serial.printf("PM 10: %lf ug/m^3 \n", pm10); 
        }

        /* 
        #######################################################
          Packet Loss
        #######################################################
        */

        msg_sent_success = doc["msg_s"].as<uint32_t>();
        msg_sent_fail = doc["msg_f"].as<uint32_t>();

        if (msg_sent_success > 0) { 
          Serial.printf("Success: %u \n", msg_sent_success); 
        }
        if (msg_sent_fail > 0) { 
          Serial.printf("Fail: %u \n", msg_sent_fail); 
        }

        total = msg_sent_success + msg_sent_fail;
        if (total != 0) {
          packet_loss = (msg_sent_fail*100) / total;
          if (packet_loss > 0 && packet_loss <= 100){
            str_key_name_packet_loss = node_N + node_num + " Packet_Loss";
            str_key_name_packet_loss.toCharArray(key_name_packet_loss, 10);
            Serial.printf("Packet loss: %lf%% \n", packet_loss);
          }
        }

        /* 
        #######################################################
          Latency
        #######################################################
        */

        latency = doc["lat"].as<double>();
        if (latency > 0) { 
          str_key_name_latency = node_N + node_num + " Latency";
          str_key_name_latency.toCharArray(key_name_latency, 10);
          Serial.printf("Latency: %lf s \n", latency);
        }

        /* 
        #######################################################
          Throughput
        #######################################################
        */

        throughput = doc["thr_bps"].as<double>();
        if (throughput > 0) { 
          str_key_name_throughput = node_N + node_num + " Throughput";
          str_key_name_throughput.toCharArray(key_name_throughput, 10);
          Serial.printf("Throughput: %lf bps \n", throughput);
        }

        /* 
        #######################################################
          MTBF
        #######################################################
        */

        MTBF = doc["MTBF"].as<long double>();
        if (MTBF > 0) { 
          str_key_name_MTBF = node_N + node_num + " MTBF";
          str_key_name_MTBF.toCharArray(key_name_MTBF, 10);
          Serial.printf("MTBF: %Lf sec \n", MTBF);
        }

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