//
// Created by damien on 08/10/23.
// modified bt Patrick 12/10/2023, tout ok avec buton ou message MQTT

#include "IOManager.h"
#include "mqttManager.h"
#include <Adafruit_MCP23X17.h>// add for I2C & MCP23017
#include <Wire.h>             // add for I2C GPIO 5 (D1) (SCL), GPIO 4 (D2) (SDA)

unsigned long pulseTime = 250;
unsigned long pulseStop = 0;

Adafruit_MCP23X17 mcp1;// Adafruit_MCP23X17 mcp1;

IOManager::IOManager() = default;

void IOManager::setup() {
	// pinMode(LEDPin, OUTPUT);
	// pinMode(PinRead, INPUT);
	// digitalWrite(LEDPin, HIGH); // initial led OFF
	// if (digitalRead(PinRead) == HIGH)

	switchState = false;// all Switch off

	Wire.begin(4, 5);    // SDA d2, SCL d1, esp8266...Start I2C communication
	mcp1.begin_I2C(0x20);// Instantiate mcp1: objectaddress 0
	if (!mcp1.begin_I2C()) {
		Serial.println(" Error.mcp1 ");// if (!mcp1.begin_SPI(CS_PIN)) { --> if use a spi version
		while (1)
			;
	}

	for (int n = 0; n < 8; n++)// configure Port A & Port B ; Port A: pins 0..7/ Port B: pins 8..15
	{
		mcp1.pinMode(n, OUTPUT);
		mcp1.digitalWrite(n, HIGH);
		delay(50);
	}
	delay(15);
	for (int i = 8; i < 16; i++) {
		mcp1.pinMode(i, INPUT_PULLUP);
		delay(50);
	}
}

void IOManager::loop() {
	static int iNb = 0, iEtatPB0, iEtatMemPB = HIGH;
	iEtatPB0 = mcp1.digitalRead(8); // lecture entrée PB0, front descendant
	if ((iEtatPB0 != iEtatMemPB) && (iEtatPB0 == LOW) && (iNb == 0)) {
		iNb++;
		// Serial.println(iNb);
		iEtatMemPB = iEtatPB0;
		pulseStop = millis();
		Serial.print(" pulseStart PB0: ");
		// Serial.print(iEtatMemPB);
		// Serial.print(" " );
			if (!switchState && (iNb >=1)) {  //if (!switchState && mqttManager) {
				mqttManager->senMessage("TopicESP/bp1", "On");
				Serial.print(" send On (PB0): ");
			}	
			switchState = true;	
		 }
	
	if ((millis() - pulseStop) >= pulseTime && mcp1.digitalRead(0) == LOW) {// if more than 250 ms low --> send "off"
		Serial.print(" pulse: ");
		Serial.print(millis() - pulseStop); // durée Pulse
		Serial.print(" ms / ");
		mqttManager->senMessage("TopicESP/order", "Off");
		mcp1.digitalWrite(0,HIGH); 
		pulseStop = 0;
		iEtatMemPB = HIGH;
		switchState = false;
		}
		if (iEtatPB0 == HIGH){ // init une fois le Bp relaché
		iNb = 0;
		}
	}

void IOManager::attachMqttManager(MqttManager *mngr) {
	mqttManager = mngr;
}

void IOManager::setLEDState(bool on) {
	if (on) {
		// digitalWrite(LEDPin, LOW);// led on
		mcp1.digitalWrite(0, LOW);// led on
		Serial.print(".");
		pulseStop = millis();
		Serial.println("Setting led on:> ");
	} else {
		// digitalWrite(LEDPin, HIGH);// led off
		mcp1.digitalWrite(0, HIGH);// led off
		Serial.println("Setting led off:> ");
	}
}
