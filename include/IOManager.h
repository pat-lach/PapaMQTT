// Created by argawaen on 08/10/23.
//
#pragma once

#include <Arduino.h>

class MqttManager;
/**
 * @brief class managing the IO.
 */
class IOManager {
public:
	/**
	 * @brief Constructor.
	 */
	IOManager();
	/**
	 * @brief Initialization function.
	 */
	void setup();
	/**
	 * @brief Frame function.
	 */
	void loop();
	/**
	 * @brief Attach a messaging system
	 * @param mngr The new messaging system
	 */
	void attachMqttManager(MqttManager *mngr = nullptr);

	/**
	 * @brief Defines the LED State.
	 * @param on If the LED should be on.
	 */
	void setLEDState(int8_t Id, bool on);

private:
	/// The MqttManager
	MqttManager *mqttManager = nullptr;
	/// The pin to read
	//uint8_t PinRead = D6;
	/// The LED pin
	int AppsPos[3] = {1, 1, 3};
	///  The last state of the switch
	bool switchState = false;
	///  pour mise au point
	int iNb = 0;
	int iEtatMemPB = HIGH;
	uint64_t pulseStop = 0;

	int8_t timing = -1;
	struct EtatP {
		bool memoire = false;
		bool release = true;
	};
	EtatP MesEtats[8];
};
