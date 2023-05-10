#include <ThingsBoard.h>

#define TOKEN "NODE1_TOKEN"

char thingsboardServer[] = "thingsboard.cloud";

//WiFiClient wifiClient; -> #include <WiFiClient.h> ginamit

WiFi wifi;

ThingsBoard tb(wifi);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup_thingsboard()
{
  lastSend = 0;
}

void loop_thingsboard()
{
  if ( !tb.connected() 
    while (!tb.connected()) {
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

  //send data to thingsboard
  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds

    tb.sendTelemetryFloat("N" + String(node) + " Temperature", tempC);
    tb.sendTelemetryFloat("N" + String(node) + " Humidity", hum);
    tb.sendTelemetryFloat("N" + String(node) + " CO", CO);
    tb.sendTelemetryFloat("N" + String(node) + " PM2.5", float(pm25)/10);
    tb.sendTelemetryFloat("N" + String(node) + " PM10", float(pm10)/10);
    lastSend = millis();
  }
  

  tb.loop();
}