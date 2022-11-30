//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"
#include <Arduino_JSON.h>

// DHT
#include <DHT.h>
#define DHT_SENSOR_PIN  21 // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22

// PAINLESS MESH DETAILS
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

//Number for this node
int nodeNumber = 2;

//String to send to other nodes with sensor readings
String readings;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
String getReadings(); // Prototype for sending sensor readings

//Create tasks: to send messages and get readings;
Task taskSendMessage( TASK_SECOND * 5 , TASK_FOREVER, &sendMessage );

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["hum"] = dht_sensor.readHumidity();
  jsonReadings["tempC"] = dht_sensor.readTemperature();
  jsonReadings["tempF"] = dht_sensor.readTemperature(true);

  // check whether the reading is successful or not
  if ( isnan(jsonReadings["hum"]) || isnan(jsonReadings["tempC"]) || isnan(jsonReadings["tempF"])) {
    return "Failed to read from DHT sensor!";
  } else {
    readings = JSON.stringify(jsonReadings);
    return readings;
  }
}

void sendMessage() {
  String msg = getReadings();
  mesh.sendBroadcast( msg );
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  double hum = myObject["hum"];
  double tempC = myObject["tempC"];
  double tempF = myObject["tempF"];
  Serial.print("Node: ");
  Serial.println(node);
  
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.print(" C, ");
  Serial.print(tempF);
  Serial.println(" F");

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  // DHT
  dht_sensor.begin();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
