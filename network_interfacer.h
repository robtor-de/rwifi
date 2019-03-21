const int port = 80;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

WiFiUDP server;

void udp_config(String confstring) {
  int d = confstring.indexOf(':');

  if(d != -1) {
    confstring.substring(0, d).toCharArray(ssid, 32);
    confstring.substring(d+1).toCharArray(password, 32);
    Serial.println(ssid);
    Serial.println(password);
    saveCredentials();
    Serial.println("#saved");
  } else {
    Serial.println("#error");
  }
}

void startserver() {
  server.begin(port);
  Serial.println("#listening");
}

void closeserver() {
  server.stop();
  Serial.println("#closed");
}

void network_loop() {
  if(server.parsePacket()) {
    int len = server.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    if (len > 0) {
      packetBuffer[len] = 0;
    }

    String buf = packetBuffer;

    if(buf.charAt(0) == '$') {
      udp_config(buf.substring(1));
    } else {
      Serial.println(buf);
    }
  }
}
