#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

#define INTERACTIVE //remove this line when module is in builtin mode


//network config
#define UDP_PORT 80

//constants for serial communication messages
#define MSG_CONNECTED "#connected"
#define MSG_FAIL "#fail"
#define MSG_MISSING_CREDENTIALS "#nocredentials"
#define MSG_INIT "#init"
#define MSG_UNIT_DISABLED "#disabled"
#define MSG_DISCONNECTED "#disconnected"
#define MSG_SAVED "#saved"
#define MSG_SERVER_ENABLED "#listening"
#define MSG_SERVER_CLOSED "#closed"

#include "credentials.h"
#include "network_interfacer.h"
#include "serial_interfacer.h"

//constant values for connection control
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
        Serial.println(MSG_CONNECTED);
        return true;
      }
      //try after a certain delay
      delay(TRYDELAY);
      trycount++;
    }
    //output fail state, if connection not established
    Serial.println(MSG_FAIL);
    return false;
  } else {
    //print fail state if no wifi credentials were found
    Serial.println(MSG_MISSING_CREDENTIALS);
    return false;
  }
}

//function to setup the serial adapter
void serial_setup() {
  Serial.begin(115200);
  Serial.setTimeout(SERIALTIMEOUT);
  Serial.println(MSG_INIT);
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
      Serial.println(MSG_FAIL);  //if not connected, then try to reconnect
      if(!connect_sta()) {
        rc_count++;
      }
    }
  }

  if(rc_count >= MAXRECONNECT) {
    Serial.println(MSG_UNIT_DISABLED);
  }
}

//this function runs repeadedly and loops all necessary functions
void loop() {
  serial_loop();
  wifi_loop();
  network_loop();
}
