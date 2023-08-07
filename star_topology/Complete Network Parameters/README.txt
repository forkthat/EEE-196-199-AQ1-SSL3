- May client and server communication para ma-measure throughput, latency, and packet loss
- Measures MTBF and includes deep sleep mode
- Every 5 minutes, 20 bytes of a packet (packetSize) is sent to the server. 
- For the duration of testing, it will be determined by the number of packets (numPackets). For 1 hour of testing, numPackets = 12 kasi 12 * 5 minutes is 1 hour.
- Sa example na to, total size of data to be transmitted = packetSize * numPackets * numberOfTestingHours = 20 bytes * 12 packets * 1 hr = 240 bytes


HOW TO USE:
- modify value of numberOfTestingHours (if gusto baguhin length of testing)
- IP address of client code depends on the IP address of server. 
- Run muna server, then makikita mo yung IP address nya. Then copy mo yung nakita mo na IP address to the client code.
- Run client
