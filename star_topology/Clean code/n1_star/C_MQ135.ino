int counter = 0;
double arr_CRZero[10];
double arr_CPPM[10];
double sum_CRZero;
double sum_CPPM;
double MA_CRZero;
double MA_CPPM;

void setup_MQ135(){

}

void loop_MQ135() {
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(tempC, humi);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(tempC, humi);
  float correctedPPM_ugm3 = correctedPPM * 1000;

  // Serial.print("RZero: ");
  // Serial.print(rzero);
  // Serial.print("\t Corrected RZero: ");
  // Serial.print(correctedRZero);
  // Serial.print("\t Resistance: ");
  // Serial.print(resistance);
  // Serial.print("\t PPM: ");
  // Serial.print(ppm);
  // Serial.print("ppm");
  // Serial.print("\t Corrected PPM: ");
  // Serial.print(correctedPPM);
  // Serial.print("ppm");
  Serial.print("CO: ");
  Serial.print(correctedPPM_ugm3);
  Serial.print(" ug/m3 \n");
  if (isnan(correctedPPM_ugm3) || correctedPPM_ugm3 == 0.00) { //don't send to thingsboard for CO if 0 or no value
  }
  else {
    tb.sendTelemetryFloat("N1 CO", correctedPPM_ugm3);
  }
  
  
  arr_CRZero[counter] = correctedRZero;
  arr_CPPM[counter] = correctedPPM;

  sum_CRZero = 0;
  sum_CPPM = 0;
  for(int i = 0; i < 10; i++){
    sum_CRZero = sum_CRZero + arr_CRZero[i];
    sum_CPPM = sum_CPPM + arr_CPPM[i];
  }
  MA_CRZero = sum_CRZero / 10;
  MA_CPPM = sum_CPPM / 10;

  // Serial.print("\t MA_CRZero: ");
  // Serial.print(MA_CRZero);
  // Serial.print("\t MA_CPPM: ");
  // Serial.print(MA_CPPM);
  // Serial.println("ppm");

  counter++;
  if (counter >= 10){
    counter = 0;
  }

  delay(300);
  
}