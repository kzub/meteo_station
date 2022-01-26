#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "msTime.h"

WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

//------------------------------------------------------------
void ntpSetup(){
  timeClient.begin();
}

//------------------------------------------------------------
void ntpLoop() {
  timeClient.update();
}

//------------------------------------------------------------
uint32_t getEpochTimeSec() {
  return timeClient.getEpochTime();
}

//------------------------------------------------------------
String getFormatetTime() {
  return timeClient.getFormattedTime();
}
