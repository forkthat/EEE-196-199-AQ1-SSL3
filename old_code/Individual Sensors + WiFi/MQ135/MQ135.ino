#include <MQUnifiedsensor.h>


//Definitions
#define placa "ESP-32"
#define Voltage_Resolution 3.3
#define pin 34 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 12 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  

double          CO2          =   (0);

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);


void setup() {
  Serial.begin(9600);
  if(Serial) Serial.println("Serial is open");

  //Set math model to calculate the PPM concentration and the value of constants
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
 
  MQ135.init(); 
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
  MQ135.serialDebug(false);

}

void loop() {
  MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
 
  MQ135.setA(110.47); MQ135.setB(-2.862);
  float CO2 = MQ135.readSensor()*1000;

  MQ135.setA(605.18); MQ135.setB(-3.937);
  float CO = MQ135.readSensor()*1000;
  
  Serial.print("CO2: ");
  Serial.print(CO2); //in microgram/m^3
  Serial.println(" ug/m^3");
  Serial.print("CO: ");
  Serial.print(CO);
  Serial.println(" ug/m^3");

 
  Serial.println(" Sending data...");
  
  
  
  delay(2000); //Sampling frequency

}
