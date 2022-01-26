# Meteostation

Platformio project for ESP8266 with sensors:
* MH-Z19 for CO2
* DHT22 for Tempreture & Humidity
* OpenTSDB for storing metrics

WiFi and OpenTSDB credentials can be configured through web interface.
(Connect to METEOSTATION-* AP to setup device)
Tested on Wemos D1.

# useful commands
> pio run -t upload

> pio device monitor -b 115200
