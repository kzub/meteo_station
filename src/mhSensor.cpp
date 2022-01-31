#include <MHZ19_uart.h>
#include "mhSensor.h"

MHZ19_uart mhz19;

#define MH_Z19_TX D7             // Pin where MH-Z19 TX connected (sensor will return -1 if this pins messed)
#define MH_Z19_RX D6             // Pin where MH-Z19 RX connected (sensor will return -1 if this pins messed)

bool justStarted = true;
uint32_t startMs;
//------------------------------------------------------------
void mhSetup() {
  mhz19.begin(MH_Z19_RX, MH_Z19_TX);
  mhz19.setAutoCalibration(false); // when indoor used
  startMs = millis();
  Serial.println("MH-Z19 ok");
}

//------------------------------------------------------------
#define WARMPUP_PERIOD 10000
mhInfo ppmRead() {
  if (justStarted) {
    auto now = millis();
    if ((now > startMs && startMs + WARMPUP_PERIOD > now) ||
        (now < startMs && startMs + WARMPUP_PERIOD < now) ) { // handle turn value over zero
      // still warming up
      Serial.println("MHZ19 Failed to read co2, device is warming up");
      return { 0, 0, false };
    }
    justStarted = false;
  }

  int co2ppm = mhz19.getCO2PPM();
  int temp = mhz19.getTemperature();

  if (co2ppm == -1 || co2ppm > 2500) {
    Serial.println("MHZ19 Failed to read co2 and temp values!");
    return { 0, 0, false };
  }

  return {
    co2ppm,
    temp,
    true
  };
}
