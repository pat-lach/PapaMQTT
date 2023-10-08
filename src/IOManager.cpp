//
// Created by damien on 08/10/23.
//

#include "IOManager.h"
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
	if (digitalRead(PinRead) == HIGH) {
		if (!switchState && mqttManager) {
			mqttManager->senMessage("TopicESP/e1", "Switch On");
		}
		switchState = true;
	} else {
		if (switchState && mqttManager) {
			mqttManager->senMessage("TopicESP/e1", "Switch Off");
		}
		switchState = false;
	}
}

void IOManager::attachMqttManager(MqttManager *mngr) {
	mqttManager = mngr;
}

void IOManager::setLEDState(bool on) {
	if (on) {
		digitalWrite(LEDPin, LOW);
		Serial.println("Setting led on");
	} else {
		digitalWrite(LEDPin, HIGH);
		Serial.println("Setting led off");
	}
}
