- May client and server communication para ma-measure throughput, latency, and packet loss
- Every 5 minutes, 1024 bytes of a packet (packetSize) is sent to the server. 
    - 1024 bytes per packet kasi yun yung normal
    - Every 5 min pag-send ng data kasi sinisimulate natin kung nag-memeasure sensors every 5 min
- For the duration of testing, it will determined by the number of packets (numPackets). For 1 hour of testing, numPackets = 12 kasi 12 * 5 minutes is 1 hour.
- Sa example na to, total size of data to be transmitted = packetSize * numPackets = 1024 bytes * 12 packets ≈ 12 kilobytes

HOW TO USE:
- modify value of numPackets or packetSize (if gusto baguhin size of data to be transmitted and length of testing)
- IP address of client code depends on the IP address of server. 
- Run muna server, then makikita mo yung IP address nya. Then copy mo yung nakita mo na IP address to the client code.
- Run client
