

// Necesaary Libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define DHT_SENSOR_PIN  21 // ESP32 pin GIOP21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLvAHw_AQB"
#define BLYNK_DEVICE_NAME "Mesh"
#define BLYNK_AUTH_TOKEN "dY-71M4kS5u3dFcsizKTAZM1vztxq1GU"

char ssid[] = "PLDTHOMEFIBRwKJ3r";
char pass[] = "PLDTWIFIjwq5R";

// Serial2 pins of ESP32
#define RXD2 16
#define TXD2 17

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

void checkBlynkStatus()
{ // called every 3 seconds by SimpleTimer
 
  bool isconnected = Blynk.connected();
  if (isconnected == false)
  {
    //wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    //digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true)
  {
    //wifiFlag = 0;
    //digitalWrite(wifiLed, HIGH);
    Serial.println("Blynk Connected");
  }
}

void sendSensor()
{
  float humi  = dht_sensor.readHumidity();
  // read temperature in Celsius
  float tempC = dht_sensor.readTemperature();
  // read temperature in Fahrenheit
  float tempF = dht_sensor.readTemperature(true);

  // check whether the reading is successful or not
  if ( isnan(tempC) || isnan(tempF) || isnan(humi)) {
    Serial.println("Failed to read from DHT sensor!");
  } 
  else {
    Blynk.virtualWrite(V0, tempC);
    Blynk.virtualWrite(V1, tempF);
    Blynk.virtualWrite(V2, humi);
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print(" C  ~  ");
    Serial.print(tempF);
    Serial.println(" F");

  }
}
void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80); 
  timer.setInterval(2000L, sendSensor);
  dht_sensor.begin();
  //Blynk.config(auth);
  //delay(1000);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates SimpleTimer

  // wait a 2 seconds between readings
  //delay(2000);
}

 