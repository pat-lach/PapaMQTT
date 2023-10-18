//
// Created by damien on 08/10/23.
// modified bt Patrick 12/10/2023, tout ok avec buton ou message MQTT

#include "IOManager.h"
#include "mqttManager.h"
#include <Adafruit_MCP23X17.h>// add for I2C & MCP23017
#include <Wire.h>             // add for I2C GPIO 5 (D1) (SCL), GPIO 4 (D2) (SDA)

constexpr uint64_t pulseTime = 250;


Adafruit_MCP23X17 mcp1;// Adafruit_MCP23X17 mcp1;

IOManager::IOManager() = default;

void IOManager::setup() {
	switchState = false;// all Switch off


	Wire.begin(4, 5);    // SDA d2, SCL d1, esp8266...Start I2C communication
	mcp1.begin_I2C(0x20);// Instantiate mcp1: objectaddress 0
	if (!mcp1.begin_I2C()) {
		Serial.println(" Error.mcp1 ");// if (!mcp1.begin_SPI(CS_PIN)) { --> if use a spi version
		while (1)
			;
	}

	for (int n = 0; n < 8; n++)// configure Port A: pins 0..7/ Port B: pins 8..15
	{
		mcp1.pinMode(n, OUTPUT);
		mcp1.digitalWrite(n, LOW);// all setting off
		delay(10);
	}
	//delay(15);
	for (int i = 8; i < 16; i++) {
		mcp1.pinMode(i, INPUT_PULLUP);
		delay(10);
	}
}

void IOManager::loop() {
	if (timing >= 0) {
		if ((millis() - pulseStop) >= pulseTime)
			setLEDState(timing, false);
		return;
	}
	for (int8_t id = 0; id < 8; ++id) {
		int32_t iEtatPB = mcp1.digitalRead(8 + id);
		if (iEtatPB == HIGH)
			MesEtats[id].release = true;
		bool change = iEtatPB != MesEtats[id].memoire;
		MesEtats[id].memoire = iEtatPB;
		if (iEtatPB == LOW && change){
			setLEDState(id, true);
			break;
		}	
	}



	// int32_t iEtatPB0 = mcp1.digitalRead(8);// lecture entrée PB0, front descendant
	// if ((iEtatPB0 != iEtatMemPB) && (iEtatPB0 == LOW) && (iNb == 0)) {
	// 	iNb++;
	// 	iEtatMemPB = iEtatPB0;
	// 	pulseStop = millis();
	// 	if (!switchState && (iNb >= 1)) {//if (!switchState && mqttManager) {
	// 		mqttManager->senMessage("TopicESP/bp1", "on");
	// 		//Serial.print(" send On (PB0");
	// 	}
	// 	switchState = true;
	// }

	// if (switchState && (millis() - pulseStop) >= pulseTime && mcp1.digitalRead(0) == HIGH) {// if more than 250 ms low --> send "off"
	// 	mqttManager->senMessage("TopicESP/bp1", "off");
	// 	mcp1.digitalWrite(0, LOW);// on force à off après 250 ms
	// 	iEtatMemPB = HIGH;
	// 	switchState = false;
	// }
	// if (iEtatPB0 == HIGH) {// init une fois le Bp relaché
	// 	iNb = 0;
	// }
}

void IOManager::attachMqttManager(MqttManager *mngr) {
	mqttManager = mngr;
}

void IOManager::setLEDState(int8_t Id, bool on) {
	if (on) {                      // digitalWrite(LEDPin, LOW);// led on
		timing = Id;
		pulseStop = millis();
		mcp1.digitalWrite(Id, HIGH);// led on
		Serial.print("  Setting on: ");
		Serial.println(Id);
	} else {
		timing = -1;
		mcp1.digitalWrite(Id, LOW); // led off
				
		// for (int n = 0; n < 8; n++)// set off Port A: pins 0..7
		{
			// mcp1.digitalWrite(n, LOW);// all setting off
			// delay(10);
			// Serial.print(mcp1.digitalRead(n + 8));
		}
		Serial.print("  Setting off: ");
		Serial.println(Id);
	}
}
