int status = WL_IDLE_STATUS;
unsigned long lastSend;

unsigned long previousMillis = 0;
unsigned long interval = 30000;

void reconnect_WIFI()
{
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

void init_WIFI() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println(WiFi.localIP());
}

void setup_WIFI()
{
  init_WIFI();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop_WIFI()
{
  reconnect_WIFI();
}
