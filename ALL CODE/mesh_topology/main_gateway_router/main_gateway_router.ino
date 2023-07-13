#define RXD2 16
#define TXD2 17

// ####################################################################
// RECEIVE JSON
// ####################################################################

#include <ArduinoJson.h>
#include <sstream>

#define MAX_STRING_LENGTH 10

uint32_t node_num, msg_sent_success, msg_sent_fail;
double tempC, hum, CO, CO2, pm25, pm10, total, packet_loss, latency, throughput, MTBF;

String node_N = "N";
String str_key_name_temp;
String str_key_name_hum;
String str_key_name_CO;
String str_key_name_CO2;
String str_key_name_PM25;
String str_key_name_PM10;
String str_key_name_packet_loss;
String str_key_name_latency;
String str_key_name_throughput;
String str_key_name_MTBF;

char key_name_temp[MAX_STRING_LENGTH];
char key_name_hum[MAX_STRING_LENGTH];
char key_name_CO[MAX_STRING_LENGTH];
char key_name_CO2[MAX_STRING_LENGTH];
char key_name_PM25[MAX_STRING_LENGTH];
char key_name_PM10[MAX_STRING_LENGTH];
char key_name_packet_loss[MAX_STRING_LENGTH];
char key_name_latency[MAX_STRING_LENGTH];
char key_name_throughput[MAX_STRING_LENGTH];
char key_name_MTBF[MAX_STRING_LENGTH];

// ####################################################################
// THINGSBOARD
// ####################################################################

#include <ThingsBoard.h>
#include <WiFiClient.h>
#define TOKEN "Gateway_Router_TOKEN"

WiFiClient wifiClient;
ThingsBoard tb(wifiClient);

char thingsboardServer[] = "thingsboard.cloud";
unsigned long lastSend;
unsigned long interval_TB = 1000 * 5;

// ####################################################################
// WIFI
// ####################################################################

#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Experimental Network";
const char* password = "tanongmokayjaybie";
// const char* ssid = "Thingsboard";
// const char* password = "dennis@thingsboard";

unsigned long previousMillis = 0;
unsigned long interval_WiFi = 1000 * 30;

// ####################################################################
// SETUP AND LOOP
// ####################################################################

void setup(){
  Serial.begin(19200);
  while (!Serial) continue;

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while(!Serial2) continue;

  Serial.println("======================================================");
  setup_receive_JSON();
  setup_WIFI();
  setup_thingsboard();
}

void loop(){
  loop_receive_JSON();
  loop_WIFI();
  loop_thingsboard();
}