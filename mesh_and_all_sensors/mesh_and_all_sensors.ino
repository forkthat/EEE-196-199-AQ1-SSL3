// ============= INITIALIZATION ====================
// ================== DHT ==========================
#include <DHT.h>
#define DHT_SENSOR_PIN  21        // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22

// ================== MQ135 ========================
#include <MQUnifiedsensor.h>
#define placa "ESP-32"
#define Voltage_Resolution 3.3
#define pin 34                    // Analog input 0 of your arduino
#define type "MQ-135"             // MQ135
#define ADC_Bit_Resolution 12     // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6    // RS / R0 = 3.6 ppm
double CO2 = (0);
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

// ================== SDS011 ========================
#include <SoftwareSerial.h>
#include <Sds011.h>
#define SDS_PIN_RX 16
#define SDS_PIN_TX 17
#ifdef ESP32
HardwareSerial& serialSDS(Serial2);
Sds011Async< HardwareSerial > sds011(serialSDS);
#else
SoftwareSerial serialSDS;
Sds011Async< SoftwareSerial > sds011(serialSDS);
#endif

// The example stops the sensor for 210s, then runs it for 30s, then repeats.
// At tablesizes 20 and below, the tables get filled during duty cycle
// and then measurement completes.
// At tablesizes above 20, the tables do not get completely filled
// during the 30s total runtime, and the rampup / 4 timeout trips,
// thus completing the measurement at whatever number of data points
// were recorded in the tables.
constexpr int pm_tablesize = 20;
int pm25_table[pm_tablesize];
int pm10_table[pm_tablesize];
bool is_SDS_running = true;

// ============ PAINLESS MESH DETAILS =================
#include "painlessMesh.h"
#include <Arduino_JSON.h>

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
void sendMessage_dht22_mq135() ;        // Prototype so PlatformIO doesn't complain
void sendMessage_sds011() ;             // Prototype so PlatformIO doesn't complain
String getReadings_dht22_mq135();       // Prototype for sending sensor readings
String getReadings_sds011();            // Prototype for sending sensor readings

// Create tasks: to send messages and get readings;
Task taskSendMessage( TASK_SECOND * 300 , TASK_FOREVER, &sendMessage_dht22_mq135 );
// Task taskSendMessage( TASK_SECOND * 300 , TASK_FOREVER, &sendMessage_sds011 );

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

// ==============================================================
//                          SDS011
// ==============================================================

int pm25;
int pm10;

void start_SDS() {
    Serial.println("Start wakeup SDS011");
    if (sds011.set_sleep(false)) { is_SDS_running = true; }
    Serial.println("End wakeup SDS011");
}

void stop_SDS() {
    Serial.println("Start sleep SDS011");
    if (sds011.set_sleep(true)) { is_SDS_running = false; }
    Serial.println("End sleep SDS011");
}

void loop_SDS(){
    constexpr uint32_t down_s = 210;

    stop_SDS();
    Serial.print("stopped SDS011 (is running = ");
    Serial.print(is_SDS_running);
    Serial.println(")");

    uint32_t deadline = millis() + down_s * 1000;
    while (static_cast<int32_t>(deadline - millis()) > 0) {
        delay(1000);
        Serial.println(static_cast<int32_t>(deadline - millis()) / 1000);
        sds011.perform_work();
    }

    constexpr uint32_t duty_s = 30;

    start_SDS();
    Serial.print("started SDS011 (is running = ");
    Serial.print(is_SDS_running);
    Serial.println(")");

    getReadings_sds011();    

    deadline = millis() + duty_s * 1000;
    while (static_cast<int32_t>(deadline - millis()) > 0) {
        delay(1000);
        Serial.println(static_cast<int32_t>(deadline - millis()) / 1000);
        sds011.perform_work();
    }
}

String getReadings_dht22_mq135(){
    JSONVar jsonReadings;
    jsonReadings["node"] = nodeNumber;

    // ============= GET SENSOR READINGS ================
    // ================== DHT ===========================
    jsonReadings["dht22_hum"] = dht_sensor.readHumidity();
    jsonReadings["dht22_tempC"] = dht_sensor.readTemperature();
    jsonReadings["dht22_tempF"] = dht_sensor.readTemperature(true);

    // ================== MQ135 ========================
    MQ135.setA(605.18);
    MQ135.setB(-3.937);
    jsonReadings["mq135_CO"] = (float) MQ135.readSensor() * 1000;

    readings = JSON.stringify(jsonReadings);
    return readings;
}

void sendMessage_dht22_mq135() {
    String msg = getReadings_dht22_mq135();
    mesh.sendBroadcast( msg );
}

String getReadings_sds011(){
    JSONVar jsonReadings;
    
    sds011.on_query_data_auto_completed([](int n) {
        Serial.println("Begin Handling SDS011 query data");
        if (sds011.filter_data(n, pm25_table, pm10_table, pm25, pm10) &&
            !isnan(pm10) && !isnan(pm25)) {
            jsonReadings["sds011_pm10"] = float(pm10);
            jsonReadings["sds011_pm25"] = float(pm25);
        }
        Serial.println("End Handling SDS011 query data");
    });

    if (!sds011.query_data_auto_async(pm_tablesize, pm25_table, pm10_table)) {
        Serial.println("Measurement capture start failed");
    }

    readings = JSON.stringify(jsonReadings);
    return readings;
}

void sendMessage_sds011() {
    String msg = getReadings_sds011();
    mesh.sendBroadcast( msg );
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
    Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
    JSONVar myObject = JSON.parse(msg.c_str());

    int node = myObject["node"];
    Serial.print("Node: ");
    Serial.print(node);

    // ===== PRINT SENSOR READINGS FROM OTHER NODES =====
    // ================== DHT ===========================
    double dht22_tempC = myObject["dht22_tempC"];
    double dht22_tempF = myObject["dht22_tempF"];
    double dht22_hum = myObject["dht22_hum"];
    Serial.print("\nTemperature: ");
    Serial.print(dht22_tempC);
    Serial.print(" C, ");
    Serial.print(dht22_tempF);
    Serial.print(" F");
    Serial.print("\nHumidity: ");
    Serial.print(dht22_hum);
    Serial.print(" %");

    // ================== MQ135 ========================
    float mq135_CO = myObject["mq135_CO"];
    Serial.print("\nCO: ");
    Serial.print(mq135_CO);
    Serial.print(" ug/m^3");

    // ================ SDS011 ==========================
    float sds011_pm25 = myObject["sds011_pm25"];
    float sds011_pm10 = myObject["sds011_pm10"];
    Serial.print("\nPM2.5: ");
    Serial.print(sds011_pm25);
    Serial.print(" ug/m^3");
    Serial.print("\nPM10: ");
    Serial.print(sds011_pm10);
    Serial.print(" ug/m^3");
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

    // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

    mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    userScheduler.addTask( taskSendMessage );
    taskSendMessage.enable();

    // ============= SETUP OF SENSORS =================
    // ================== DHT =========================
    dht_sensor.begin();

    // ================== MQ135 ========================
    //Set math model to calculate the PPM concentration and the value of constants
    MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
    MQ135.setA(110.47); 
    MQ135.setB(-2.862); 
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

    if(isinf(calcR0)) {
        Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); 
        while(1);
    }
    if(calcR0 == 0){
        Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply");
        while(1);
    }
    MQ135.serialDebug(false);

    // ====================== SDS011 =======================
    #ifdef ESP32
        serialSDS.begin(9600, SERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX);
        delay(100);
    #else
        serialSDS.begin(9600, SWSERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX, false, 192);
    #endif

    Serial.println("SDS011 start/stop and reporting sample");

    Sds011::Report_mode report_mode;
    if (!sds011.get_data_reporting_mode(report_mode)) {
        Serial.println("Sds011::get_data_reporting_mode() failed");
    }
    if (Sds011::REPORT_ACTIVE != report_mode) {
        Serial.println("Turning on Sds011::REPORT_ACTIVE reporting mode");
        if (!sds011.set_data_reporting_mode(Sds011::REPORT_ACTIVE)) {
            Serial.println("Sds011::set_data_reporting_mode(Sds011::REPORT_ACTIVE) failed");
        }
    }
}

void loop() {
    // it will run the user scheduler as well
    mesh.update();
    MQ135.update();
    
    loop_SDS();
}
