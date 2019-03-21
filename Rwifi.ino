#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "credentials.h"
#include "network_interfacer.h"
#include "serial_interfacer.h"

#define MSG_CONNECTED "#connected"


const int INITDELAY = 2000;            //initial delay, the module will startup after the entered value (ms)
const int TRYCOUNT = 2;                //how often should the Module try to connect when started up ?
const int TRYDELAY = 1000;             //how much delay should be between the reconnect-tries ?
const int SERIALTIMEOUT = 2000;        //set the serial timeout for the module (when config changed wia serial)
const int RECONNECTTRY = 30000;        //how often should be checked if the module is still connected ? (ms)
const int MAXRECONNECT = 10;           //maximal number of reconnect tries after the connection was lost



//initializing necessary variables
int rc_count = 0;
long loop_millis = 0;

//function to connect the station at startup
bool connect_sta() {
  //setting wifi mode
  WiFi.mode(WIFI_STA);
  //try to load credentials
  if(loadCredentials()) {
    WiFi.begin(ssid, password);
    int trycount = 0;
    //try to connect
    while(trycount <= TRYCOUNT) {
      if(WiFi.status() == WL_CONNECTED) {
        //print connected status
        Serial.println("#connected");
        return true;
      }
      //try after a certain delay
      delay(TRYDELAY);
      trycount++;
    }
    //output fail state, if connection not established
    Serial.println("#fail");
    return false;
  } else {
    //print fail state if no wifi credentials were found
    Serial.println("#nocredenitals");
    return false;
  }
}

//function to setup the serial adapter
void serial_setup() {
  Serial.begin(115200);
  Serial.setTimeout(SERIALTIMEOUT);
  Serial.println("#init");
}

//setup function, is executed one, when the module was powered
void setup() {
  delay(INITDELAY); //wait for the init delay to eceed before startup
  serial_setup();
  connect_sta();
}


//this function is looped to check if the module is still connected, when not, then reconnect
void wifi_loop() {
  if((millis() - loop_millis) >= RECONNECTTRY) {
    loop_millis = millis();
    if(WiFi.status() != WL_CONNECTED) {
      Serial.println("#fail");  //if not connected, then try to reconnect
      if(!connect_sta()) {
        rc_count++;
      }
    }
  }

  if(rc_count >= MAXRECONNECT) {
    Serial.println("#disabled");
  }
}

//this function runs repeadedly and loops all necessary functions
void loop() {
  serial_loop();
  wifi_loop();
  network_loop();
}
