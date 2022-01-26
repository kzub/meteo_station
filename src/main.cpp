#include <Arduino.h>

#include "dhtSensor.h"
#include "mhSensor.h"
#include "configPortal.h"
#include "openTSDB.h"
#include "msTime.h"

//------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("initializing devices...");

  configPortalSetup(); // wifi/tsdb web-config
  dhtSetup(); // dht sensor
  ntpSetup(); // network time protocol
  mhSetup();  // co2 sensor, last in order (will pause execution for warming up)
}

//------------------------------------------------------------
#define REPEAT_INTERVAL 60 * 1000 // ms
static uint32_t lastimage = millis() - REPEAT_INTERVAL;

void loop() {
  configPortalHandleClient();
  ntpLoop(); // update time from ntp servers

  // execute code not very often
  uint32_t now = millis();
  if (now < lastimage + REPEAT_INTERVAL) {
    return;
  }
  lastimage = now;

  //-------------------------------
  // code below runs every (REPEAT_INTERVAL) ms
  //-------------------------------

  Serial.print(getFormatetTime());
  Serial.println(" read sensors...");

  auto s1 = dhtRead();
  if(s1.ok) {
    Serial.print("Humidity: ");
    Serial.print(s1.h);
    Serial.print("%,  Temperature: ");
    Serial.print(s1.t);
    Serial.print("°C, Heat index: ");
    Serial.print(s1.hi);
    Serial.println("°C ");

    tsdbPut("temp", String(s1.t));
    tsdbPut("humidity", String(s1.h));
    tsdbPut("heatIndex", String(s1.hi));
  }

  auto s2 = ppmRead();
  if (s2.ok) {
    Serial.print("CO2: ");
    Serial.print(s2.co2ppm);
    Serial.print(", temp: ");
    Serial.println(s2.temp);

    tsdbPut("co2", String(s2.co2ppm));
    tsdbPut("temp2", String(s2.temp));
  }
}
