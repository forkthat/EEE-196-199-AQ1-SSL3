/*
####################################################################
TODO: 
  on message receive, send data to thingsboard
####################################################################
*/

HardwareSerial &hSerial = Serial2;

void setup(){
  Serial.begin(9600);
  while(!Serial) continue;

  hSerial.begin(9600);
  while(!hSerial) continue;
}

void loop(){
  if (hSerial.available()) {
    auto received = hSerial.readStringUntil('}');
    Serial.println(received);
  }
}