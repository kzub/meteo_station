#include <MHZ19_uart.h>
#include "mhSensor.h"

MHZ19_uart mhz19;

#define MH_Z19_TX D7             // Pin where MH-Z19 TX connected (sensor will return -1 if this pins messed)
#define MH_Z19_RX D6             // Pin where MH-Z19 RX connected (sensor will return -1 if this pins messed)

//------------------------------------------------------------
void mhSetup() {
  mhz19.begin(MH_Z19_RX, MH_Z19_TX);
  mhz19.setAutoCalibration(false);

  Serial.println("MH-Z19 is warming up now (10 sec).");
  delay(10 * 1000);
  Serial.println("MH-Z19 ok");
}

//------------------------------------------------------------
mhInfo ppmRead() {
  int co2ppm = mhz19.getCO2PPM();
  int temp = mhz19.getTemperature();

  if (co2ppm == -1) {
    Serial.println("MHZ19 Failed to read co2 and temp values!");
    return { 0, 0, false };
  }

  return {
    co2ppm,
    temp,
    true
  };
}
