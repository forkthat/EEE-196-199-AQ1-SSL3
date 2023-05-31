#include <WiFiClient.h>
#include <ThingsBoard.h>
#include <SoftwareSerial.h>
#include <Sds011.h>
#include <MQUnifiedsensor.h>
#include <DHT.h>
#include <WiFi.h>

#define SDS_PIN_RX 16
#define SDS_PIN_TX 17
#define DHT_SENSOR_PIN  21 // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

#define placa "ESP-32"
#define Voltage_Resolution 3.3
#define pin 34 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 12 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

#ifdef ESP32
HardwareSerial& serialSDS(Serial2);
Sds011Async< HardwareSerial > sds011(serialSDS);
#else
SoftwareSerial serialSDS;
Sds011Async< SoftwareSerial > sds011(serialSDS);
#endif


#define WIFI_SSID "Thingsboard"
#define WIFI_PASSWORD "dennis@thingsboard"

#define TOKEN "NODE1_TOKEN"

char thingsboardServer[] = "thingsboard.cloud";

WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

constexpr int pm_tablesize = 20;
int pm25_table[pm_tablesize];
int pm10_table[pm_tablesize];

bool is_SDS_running = true;

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

void sendSensor()
{
#ifdef ESP32
    serialSDS.begin(9600, SERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX);
    delay(100);
#else
    serialSDS.begin(9600, SWSERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX, false, 192);
#endif

    MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
 
    MQ135.init(); 
    //Serial.print("Calibrating please wait.");
    float calcR0 = 0;
    for(int i = 1; i<=10; i ++)
    {
      MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
      calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    //Serial.print(".");
    }
    MQ135.setR0(calcR0/10);
  //Serial.println("  done!.");
  
    if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
    if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
    MQ135.serialDebug(false);

    //Serial.println("SDS011 start/stop and reporting sample");

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

    //LOOP
    constexpr uint32_t down_s = 30; //ORIG: 210

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

    constexpr uint32_t duty_s = 30; //ORIG: 30

    start_SDS();
    Serial.print("started SDS011 (is running = ");
    Serial.print(is_SDS_running);
    Serial.println(")");

    sds011.on_query_data_auto_completed([](int n) {
        Serial.println("Begin Handling SDS011 query data");
        int pm25;
        int pm10;
        //Serial.print("n = "); 
        //Serial.println(n);
        if (sds011.filter_data(n, pm25_table, pm10_table, pm25, pm10) &&
            !isnan(pm10) && !isnan(pm25)) {
            float humi  = dht_sensor.readHumidity();
            // read temperature in Celsius
            float tempC = dht_sensor.readTemperature();
            // read temperature in Fahrenheit
            //float tempF = dht_sensor.readTemperature(true);

            if ( isnan(tempC) || isnan(humi)) {
              Serial.println("Failed to read from DHT sensor!");
            } else {
              tb.sendTelemetryFloat("N5 Humidity", humi);
              Serial.println("Humidity: " + String(humi) + "%");
              tb.sendTelemetryFloat("N5 Temperature", tempC);
              Serial.println("Temperature: " + String(tempC) + " C");
            }
            MQ135.update();
            MQ135.setA(605.18); MQ135.setB(-3.937);
            float CO = MQ135.readSensor()*1000;

            tb.sendTelemetryFloat("N5 CO", CO);

            Serial.print("CO: ");
            Serial.print(CO);
            tb.sendTelemetryFloat("N5 PM10", float(pm10/10));
            Serial.println("PM10: " + String(float(pm10)/10) + " ug/m^3");
            tb.sendTelemetryFloat("N5 PM2.5", float(pm25/10));
            Serial.println("PM2.5: " + String(float(pm25)/10) + " ug/m^3");
        }
        Serial.println("End Handling SDS011 query data");
        });

    if (!sds011.query_data_auto_async(pm_tablesize, pm25_table, pm10_table)) {
        Serial.println("measurement capture start failed");
    }

    deadline = millis() + duty_s * 1000;
    while (static_cast<int32_t>(deadline - millis()) > 0) {
        delay(1000);
        Serial.println(static_cast<int32_t>(deadline - millis()) / 1000);
        sds011.perform_work();
    }



}




void setup() 
{
  Serial.begin(115200);
  if(Serial) Serial.println("Serial is open");

  dht_sensor.begin();
  delay(10);
  InitWiFi();
  lastSend = 0;


}


void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void loop() 
{
 if ( !tb.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    sendSensor();
    lastSend = millis();
  }

  tb.loop();
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}



