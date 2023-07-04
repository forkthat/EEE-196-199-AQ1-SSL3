#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Experimental Network";
const char* password = "tanongmokayjaybie";
const int serverPort = 1234;

// // const char* ssid = "PLDTHOMEFIBR648f8";
// // const char* password = "PLDTWIFIxvxz3";
// // const int serverPort = 1234;

// const char* ssid = "PLDTHOMEFIBRwKJ3r";
// const char* password = "PLDTWIFIjwq5R";
// const int serverPort = 1234; 

const int packetSize = 20;  // Adjust packet size as needed, should be the same with packetSize of clients

int packetCount = 0;

WiFiUDP udpServer;
byte packetBuffer[packetSize];

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the assigned IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  udpServer.begin(serverPort);  // UDP port for server
  Serial.println("Server started, listening to port " + String(serverPort));
}

void loop() {
  int packetSize = udpServer.parsePacket();
  
  if (packetSize) {
    udpServer.read(packetBuffer, packetSize);
    // Process the received packet here
    packetCount++;
    Serial.print("Packet number " + String(packetCount) + ": ");

    // Print received data
    for (int i = 0; i < packetSize; i++) {
      Serial.print(packetBuffer[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}
