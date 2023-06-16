#define RXD2 16
#define TXD2 17

void setup(){
  Serial.begin(19200);
  while (!Serial) continue;

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while(!Serial2) continue;

  setup_receive_JSON();
  setup_WIFI();
  setup_thingsboard();
}

void loop(){
  loop_receive_JSON();
  loop_WIFI();
  loop_thingsboard();
}