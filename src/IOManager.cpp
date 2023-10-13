//
// Created by damien on 08/10/23.
// modified bt Patrick 12/10/2023, tout ok avec buton ou message MQTT

#include "IOManager.h"
#include "Wire.h"// add for I2C
#include "mqttManager.h"

IOManager::IOManager() = default;

void IOManager::setup() {
	pinMode(LEDPin, OUTPUT);
	pinMode(PinRead, INPUT);
	digitalWrite(LEDPin, HIGH);// initial led OFF
	if (digitalRead(PinRead) == HIGH)
		switchState = true;
}

void IOManager::loop() {
	j = j + 1;
	if (j >= 500) {
		j = 0;
		if (digitalRead(PinRead) == HIGH) {
			if (!switchState && mqttManager) {
				mqttManager->senMessage("TopicESP/order", "On");
			}
			switchState = true;
		} else {
			if (switchState && mqttManager) {
				mqttManager->senMessage("TopicESP/order", "Off");
			}
			switchState = false;
		}
	}
}

void IOManager::attachMqttManager(MqttManager *mngr) {
	mqttManager = mngr;
}

void IOManager::setLEDState(bool on) {
	if (on) {
		digitalWrite(LEDPin, LOW);// led on
		Serial.println("Setting led on");
	} else {
		digitalWrite(LEDPin, HIGH);// led off
		Serial.println("Setting led off");
	}
}
