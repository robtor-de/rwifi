void ser_config(String confstring) {
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

void get_functions(String querystring) {
  if(querystring == "ip") {
    Serial.println(WiFi.localIP());
  } else if (querystring == "disconnect") {
    Serial.println("#disconnected");
    WiFi.disconnect();
  } else if (querystring == "server") {
    startserver();
  } else if (querystring == "closeserver") {
    closeserver();
  } else {
    Serial.println("#error");
  }
}




//Serial Event function, is called, wehen serial data is received
void serialEvent() {
  String input = Serial.readStringUntil('\n');

  if(input == "help") {
    Serial.println("Command reference");
    Serial.println("Set wifi-config: $<SSID>:<PASSWORD>");
    Serial.println("Get ip-address &ip");
    Serial.println("Reconnect WiFi &disconnect");
    Serial.println("Begin udp server &server");
    Serial.println("Close udp server &closeserver");
  }

  switch(input.charAt(0)) {
    case '$': ser_config(input.substring(1)); break;
    case '&': get_functions(input.substring(1)); break;
    default: Serial.println("#unknown"); break;
  }
}




void serial_loop() {
  if(Serial.available() > 0) {
    serialEvent();
  }
}
