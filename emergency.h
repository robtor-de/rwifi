ESP8266WiFiMulti wifiMulti;

const char *password_e = "060481405";
const char *ssid_e = "RWIFI-ESP";

void emergency() {
  Serial.println("#emergency");
  WiFi.disconnect();
  WiFi.softAP(password_e, ssid_e);
}
