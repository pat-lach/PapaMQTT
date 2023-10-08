//
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
	void setLEDState(bool on = false);

private:
	/// The MqttManager
	MqttManager *mqttManager = nullptr;
	/// The pin to read
	uint8_t PinRead = D6;
	/// The LED pin
	uint8_t LEDPin = D5;
	///  The last state of the switch
	bool switchState = false;
};
