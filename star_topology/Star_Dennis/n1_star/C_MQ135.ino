MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup_MQ135(){

}

void loop_MQ135(){
  // Set math model to calculate the PPM concentration and the value of constants
  // PPM = a*ratio^b
  MQ135.setRegressionMethod(1); 
 
  MQ135.init(); 
  ///Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++){
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    //Serial.print(".");
  }

  MQ135.setR0(calcR0/10);
  //Serial.println("  done!.");
  
  if(isinf(calcR0)){
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected)"); 
    while(1);
  }
  if(calcR0 == 0){
    Serial.println("Warning: Conection issue, R0 is zero (Analog pin with short circuit to ground)"); 
    while(1);
  }

  /*****************************  MQ CAlibration ********************************************/ 
  MQ135.serialDebug(false);

  MQ135.update();

  MQ135.setA(605.18); MQ135.setB(-3.937);
  float CO = MQ135.readSensor() * 1000;
  Serial.println("CO: " + String(CO) + " ug/m3");
  tb.sendTelemetryFloat("N1 CO", CO);
}


