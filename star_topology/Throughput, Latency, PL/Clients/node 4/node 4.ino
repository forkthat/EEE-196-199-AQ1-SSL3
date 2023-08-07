#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ThingsBoard.h>

#define TOKEN "NODE4_TOKEN"

// const char* ssid = "Experimental Network";
// const char* password = "tanongmokayjaybie";
// const char* serverIP = "10.158.66.45";

// const char* ssid = "PLDTHOMEFIBR648f8";
// const char* password = "PLDTWIFIxvxz3";
// const char* serverIP = "192.168.1.16";
// const int serverPort = 1234;

const char* ssid = "PLDTHOMEFIBRwKJ3r";
const char* password = "PLDTWIFIjwq5R";
const char* serverIP = "192.168.1.72";
const int serverPort = 1234;

WiFiClient wifiClient;

char thingsboardServer[] = "thingsboard.cloud";
ThingsBoard tb(wifiClient);

const int numberOfTestingHours = 24; 
const int packetSize = 1024; // each packet has 1024 bytes
const int numPackets = 12;  // Total number of packets to send 
const int totalSize = packetSize * numPackets * numberOfTestingHours;  // Total data size in kilobytes (288 kb) 

WiFiUDP udpClient;
IPAddress serverAddress;
unsigned int packetCount = 0;
unsigned long startTime = 0;
unsigned long endTime = 0;

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

  serverAddress = IPAddress();
  if (!serverAddress.fromString(serverIP)) {
    Serial.println("Invalid server IP address");
    while (1) {
      delay(1000);
    }
  }

  udpClient.begin(1234);  // UDP port for client
}

void loop() {
  if (packetCount == 0) {
    // Start measuring
    startTime = millis();
  }

  // Prepare packet with dummy data
  byte packet[packetSize];
  for (int i = 0; i < packetSize; i++) {
    packet[i] = random(255);
  }

  // Send packet to server
  udpClient.beginPacket(serverAddress, serverPort);
  udpClient.write(packet, packetSize);
  udpClient.endPacket();

  packetCount++;

  if (packetCount == numPackets) {
    // Finished sending packets, stop measuring
    endTime = millis();
    float elapsedTime = endTime - startTime;  // milliseconds
    float throughput = (totalSize * 8) / (elapsedTime / 1000.0) / 1000.0;  // Mbps
    float averageLatency = elapsedTime / numPackets;  // milliseconds
    float packetLoss = ((numPackets - packetCount) / static_cast<float>(numPackets)) * 100.0;

    Serial.println("Measurement Results:");
    Serial.print("Throughput: ");
    Serial.print(throughput);
    Serial.println(" Mbps");
    Serial.print("Average Latency: ");
    Serial.print(averageLatency);
    Serial.println(" ms");
    Serial.print("Packet Loss: ");
    Serial.print(packetLoss);
    Serial.println("%");
    if (!tb.connected()) {
    while (!tb.connected()) {
      Serial.println("Connecting to ThingsBoard node ...");
      if (tb.connect(thingsboardServer, TOKEN)) {
        Serial.println("Successfuly connected to ThingsBoard!");
        tb.sendTelemetryFloat("N4 Throughput", throughput);
        tb.sendTelemetryFloat("N4 Latency", averageLatency);
        tb.sendTelemetryFloat("N4 Packet Loss", packetLoss);
      } else {
        Serial.print("[FAILED]");
        Serial.println(" : retrying in 5 seconds]");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }

    while (1) {
      delay(1000);
    }
  }

  // Wait for 5 minutes before sending the next batch of packets
  delay(5 * 60 * 1000);
}
