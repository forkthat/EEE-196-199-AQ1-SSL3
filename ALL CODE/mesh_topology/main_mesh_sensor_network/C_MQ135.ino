MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

String getReadings_MQ135(){
  StaticJsonDocument<128> doc;
  
  MQ135.update();

  MQ135.setA(605.18); MQ135.setB(-3.937);
  float CO = MQ135.readSensor();
  
  MQ135.setA(110.47); MQ135.setB(-2.862);
  float CO2 = MQ135.readSensor();
  
  doc["node_num"] = nodeNumber;
  doc["msg_success"] = msg_sent_success;
  doc["msg_fail"] = msg_sent_fail;

  doc["CO"] = CO;
  doc["CO2"] = CO2;

  return doc.as<String>();

  MQ135.serialDebug();
}

void setup_MQ135(){
  // Set math model to calculate the PPM concentration and the value of constants
  // PPM = a*ratio^b
  MQ135.setRegressionMethod(1); 
 
  MQ135.init(); 

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++){
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }

  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)){
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected)"); 
    while(1);
  }
  if(calcR0 == 0){
    Serial.println("Warning: Conection issue, R0 is zero (Analog pin with short circuit to ground)"); 
    while(1);
  }

  /*****************************  MQ CAlibration ********************************************/ 
  MQ135.serialDebug(true);
}

void loop_MQ135(){

}