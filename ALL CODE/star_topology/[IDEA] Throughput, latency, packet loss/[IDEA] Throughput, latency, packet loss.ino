#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ThingsBoard.h>

#define TOKEN "NODE1_TOKEN"

const char* ssid = "Experimental Network";
const char* password = "tanongmokayjaybie";
const char* serverIP = "10.158.66.45";
const int serverPort = 1234;

// const char* ssid = "PLDTHOMEFIBR648f8";
// const char* password = "PLDTWIFIxvxz3";
// const char* serverIP = "192.168.1.16";
// const int serverPort = 1234;

// const char* ssid = "PLDTHOMEFIBRwKJ3r";
// const char* password = "PLDTWIFIjwq5R";
// const char* serverIP = "192.168.1.72";
// const int serverPort = 1234;

WiFiClient wifiClient;

char thingsboardServer[] = "thingsboard.cloud";
ThingsBoard tb(wifiClient);

const int nodeID = 1;
const int numberOfTestingHours = 72; 
const int packetSize = 24; // each packet has 1024 bytes
const int numPackets = 12;  // Total number of packets to send per hour
const int totalSize = packetSize * numPackets * numberOfTestingHours;  // Total data size = 20,736 kilobytes whole 24 hours
unsigned int packetsReceived = 0;

WiFiUDP udpClient;
IPAddress serverAddress;
unsigned int packetCount = 0;
unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long packetSentTime;
unsigned long packetReceivedTime;
float totalPacketLatency = 0;

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
  // Prepare packet with dummy data
  byte packet[packetSize];
  packet[0] = nodeID;
  for (int i = 1; i < packetSize; i++) {
    packet[i] = random(255);
  }

  if (packetCount == 0) {
    // Start measuring
    startTime = millis();
  }

  packetCount++;

  if (packetCount == numPackets) {
    // Finished sending packets, stop measuring
    endTime = millis();
    float elapsedTime = endTime - startTime;  // milliseconds
    float throughput = (totalSize * 8) / (totalPacketLatency / 1000.0) / 1000.0;  // Mbps
    float averageLatency = totalPacketLatency / packetsReceived;  // milliseconds
    float packetLoss = ((numPackets - packetsReceived) / static_cast<float>(numPackets)) * 100.0;

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
        tb.sendTelemetryFloat("N1 Throughput", throughput);
        tb.sendTelemetryFloat("N1 Latency", averageLatency);
        tb.sendTelemetryFloat("N1 Packet Loss", packetLoss);
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
  // PACKET SENDING
  packetCount++;
  udpClient.beginPacket(serverAddress, serverPort);
  packetSentTime = millis();
  udpClient.write(packet, packetSize);
  udpClient.endPacket();
  Serial.print("PACKET SENT");



  bool checkReceivedPacket = udpClient.parsePacket();
  // PACKET RECEPTION CHECK
  if (checkReceivedPacket) {
    // A packet was received
    packetReceivedTime = millis();
    packetsReceived++;
  }
  float individualPacketLatency = packetReceivedTime - packetSentTime;
  totalPacketLatency += individualPacketLatency;
  Serial.print("Sent Time: ");
  Serial.print(packetSentTime);

  Serial.print("Received Time: ");
  Serial.print(packetReceivedTime);

  Serial.print("Packet Latency: ");
  Serial.print(individualPacketLatency);
  Serial.println(" ms");


  // Wait for 5 minutes before sending the next batch of packets
  delay(60000);
}
