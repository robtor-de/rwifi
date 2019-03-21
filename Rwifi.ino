#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <ESP8266WiFiMulti.h>
#include "credentials.h"
#include "network_interfacer.h"
#include "serial_interfacer.h"
#include "emergency.h"


const int TRYCOUNT = 2;
const int TRYDELAY = 1000;
const int SERIALTIMEOUT = 2000;
const int RECONNECTTRY = 500; //5000
const int MAXRECONNECT = 3;  //20


int rc_count = 0;

long loop_millis = 0;

bool connect_sta() {
  WiFi.mode(WIFI_STA);
  if(loadCredentials()) {
    WiFi.begin(ssid, password);
    int trycount = 0;
    while(trycount <= TRYCOUNT) {
      if(WiFi.status() == WL_CONNECTED) {
        Serial.println("#connected");
        return true;
      }
      delay(TRYDELAY);
      trycount++;
    }
    Serial.println("#fail");
    return false;
  } else {
    Serial.println("#nocredenitals");
    return false;
  }
}

void serial_setup() {
  Serial.begin(115200);
  Serial.setTimeout(SERIALTIMEOUT);
  Serial.println("#init");
}

void setup() {
  // put your setup code here, to run once:
  serial_setup();
  connect_sta();
}

void wifi_loop() {
  if((millis() - loop_millis) >= RECONNECTTRY) {
    loop_millis = millis();
    if(WiFi.status() != WL_CONNECTED) {
      connect_sta();
      Serial.println("#reconnect");
      rc_count++;
    }
  }

  if(rc_count >= MAXRECONNECT) {
    emergency();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  serial_loop();
  wifi_loop();
  network_loop();
}
