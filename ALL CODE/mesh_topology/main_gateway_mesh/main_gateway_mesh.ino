#include "3_NetConfig_MESH.ino"

#define RXD2 16
#define TXD2 17

void setup(){
  Serial.begin(19200);
  while(!Serial) continue;

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while(!Serial2) continue;

  setup_MESH();
}

void loop(){
  loop_MESH();
}