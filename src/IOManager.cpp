
// Created by damien on 08/10/23.
// modified by Patrick 12/10/2023, tout ok avec buton ou message MQTT
// modified by Patrick 14/02/2024 modif wiring of MCP23017, One for Aig et ond for return position

#include "IOManager.h"
#include "mqttManager.h"
#include <Adafruit_MCP23X17.h>// add for I2C & MCP23017
#include <Wire.h>             // add for I2C GPIO 5 (D1) (SCL), GPIO 4 (D2) (SDA)

constexpr uint64_t pulseTime = 700;// 500

Adafruit_MCP23X17 mcp1;// Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;// Adafruit_MCP23X17 mcp2;

IOManager::IOManager() = default;

void IOManager::setup() {
	switchState = false;// all Switch off

	Wire.begin(4, 5);// SDA d2 GPIO4, SCL d1 GPIO5, esp8266...Start I2C communication

	/****************************************/
	//          I2C Scanner
	/********************************************************************/
	byte error, address;
	int nDevices;

	Serial.println("Scanning...");

	nDevices = 0;

	for (address = 1; address < 127; address++) {
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0) {
			Serial.print("I2C device found at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.print(address, HEX);
			Serial.println("  !");

			nDevices++;
		} else if (error == 4) {
			Serial.print("Unknown error at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0)
		Serial.println("No I2C devices found\n");
	else
		Serial.println("Scanning done\n");

	delay(1000);// wait 1 seconds for next scan


	/********************************************************************/
	//          Init mcp1  output & mcp2 input
	/********************************************************************/

	mcp1.begin_I2C(0x20);// Instantiate mcp1: objectaddress 0
	if (!mcp1.begin_I2C()) {
		Serial.println(" Error.mcp1 ");// if (!mcp1.begin_SPI(CS_PIN)) { --> if use a spi version
		while (1)
			;
	}
	Serial.println(" mcp1 Ok");

	mcp2.begin_I2C(0x21);// Instantiate mcp2: objectaddress 1
	if (!mcp2.begin_I2C()) {
		Serial.println(" Error.mcp2 ");// if (!mcp2.begin_SPI(CS_PIN)) { --> if use a spi version
		while (1)
			;
	}
	Serial.println(" mcp2 Ok");

	for (int n = 0; n < 16; n++)// configure Port A: pins 0..7/ Port B: pins 8..15
	{
		mcp1.pinMode(n, OUTPUT);
		mcp1.digitalWrite(n, HIGH);// all setting off
		delay(10);
	}
	delay(15);
	Serial.println(" setting mcp1 off (high) ");
	Serial.println(" mcp1 à low ");

	/*  for (int i = 0; i < 16; i++) {
		mcp2.pinMode(i, INPUT_PULLUP); // init all HIGH		
		int32_t iEtatPB = mcp2.digitalRead(0 + i);  // pour debug
		Serial.print(" | p");
		Serial.print( i);
		Serial.print("> " );
		Serial.print( iEtatPB );
		delay(10);		
		}
	Serial.println( " setting mcp2 input (high) "); */
}

void IOManager::loop() {
	if (timing >= 0) {
		if ((millis() - pulseStop) >= pulseTime) {
			setLEDState(timing, false);
			Serial.print(pulseTime);
			return;
		}
	}


	/*	for (int i = 0; i < 16; i++) {
															int32_t iEtatPB = mcp2.digitalRead(0 + i);
															Serial.print(" pin : ");
															Serial.print( i);
															Serial.print(" iEtat : " );
															Serial.println( iEtatPB );
															delay(10);
															}  */

	/* for (int8_t id = 0; id < 16; ++id) {
		int32_t iEtatPB = mcp2.digitalRead(0 + id);
		Serial.print(" id : ");
		Serial.print( id);
		Serial.print(" iEtat : " );
		Serial.println( iEtatPB );
		if (iEtatPB == HIGH)
			MesEtats[id].release = true;
		bool change = iEtatPB != MesEtats[id].memoire;
		MesEtats[id].memoire = iEtatPB;
		if (iEtatPB == LOW && change) {
			setLEDState(id, true);
			break;
		} 
	}  */

	/* int32_t iEtatPB0 = mcp2.digitalRead(0);// lecture entrée PB0, front descendant mcp2
	 if ((iEtatPB0 != iEtatMemPB) && (iEtatPB0 == LOW) && (iNb == 0)) {
	 	iNb++;
	 	iEtatMemPB = iEtatPB0;
	 	pulseStop = millis();
	 	if (!switchState && (iNb >= 1)) {//if (!switchState && mqttManager) {
	 		mqttManager->senMessage("TopicESP/bp1", "on");
	// 		//Serial.print(" send On (PB0");
	 	}
	 	switchState = true;
	 }
	if (switchState && (millis() - pulseStop) >= pulseTime && mcp1.digitalRead(0) == HIGH) {// if more than 250 ms low --> send "off"
	 	mqttManager->senMessage("TopicESP/bp1", "off");
	 	mcp1.digitalWrite(0, LOW);// on force à off après 250 ms
	 	iEtatMemPB = HIGH;
	 	switchState = false;
	 }
	 if (iEtatPB0 == HIGH) {// init une fois le Bp relaché
	 	iNb = 0;
	} */
}

void IOManager::attachMqttManager(MqttManager *mngr) {
	mqttManager = mngr;
}

void IOManager::setLEDState(int8_t Id, bool on) {

	if (on) {// digitalWrite(LEDPin, LOW);// led on
		timing = Id;
		Serial.print(" on: Id timing : ");
		Serial.print(Id);
		pulseStop = millis();
		Serial.print(millis());
		mcp1.digitalWrite(Id, LOW);// Relais on     LOW
		Serial.print("  mcp1 on (Low) Id: ");
		Serial.println(Id);


	} else {
		timing = -1;
		mcp1.digitalWrite(Id, HIGH);// Relais off    HIGH
		Serial.print(millis());
		Serial.print("  mcp1 off (High) Id: ");
		Serial.println(Id);

		/* for (int n = 0; n < 16; n++)  // set off Port A & B: pins 0..15  to secure all relais off
		{
		mcp1.digitalWrite(n, HIGH);   // all setting off
		}
	
		Serial.print(" off All Id mcp1: ");
		Serial.println(Id);
		// delay(15); */
	}
}