#include <Arduino.h>

void deviceLED_Setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	for (int i = 0; i < 6; i++) {
		digitalWrite(LED_BUILTIN, LOW);
		delay(15);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(15);
	}
}

void blinkLED(uint16_t on = 25, uint16_t off = 0) {
	digitalWrite(LED_BUILTIN, LOW);
	delay(on);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(off);
}