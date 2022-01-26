#include "DHT.h" // https://github.com/adafruit/DHT-sensor-library
#include "dhtSensor.h"

#define DHTPIN  D8               // Pin where DHT 22 data connected
#define DHTTYPE DHT22            // DHT 22 (AM2302), AM2321
#define CELSIUS_DEGREES

DHT dht(DHTPIN, DHTTYPE);

void dhtSetup() {
  Serial.println("DHT22 init");
  dht.begin(); // pullup here
}

dhtInfo dhtRead() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float current_h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float current_t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(current_h) || isnan(current_t)) {
    Serial.println("DHT22 Failed to read from DHT sensor!");
    return { 0, 0, 0, false }; // return old value
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float current_hi = dht.computeHeatIndex(current_h, current_t, false);

  return { current_h, current_t, current_hi, true };
}
