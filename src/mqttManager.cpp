//
// Created by damien on 08/10/23.
//

#include "mqttManager.h"

//static const String mqttUser = "papa";
//static const String mqttPassword = "papa";
static const String mqttTopicIn = "TopicESP/#";
//static const String mqtt_server = "pat-lach-pil";                //adress ip de là ou est le brocker c a d mon pc (athena.argawaen.net)
static const IPAddress mqtt_server = {192, 168, 1, 75};
constexpr uint16_t mqtt_server_port = 1883;

static IOManager *s_ioManager = nullptr;

/**
 * @brief Function called everytime a message is received from broker.
 * @param topic The topic received
 * @param payload The message received.
 * @param length The length of the message.
 */
static void callback(char *topic, byte *payload, unsigned int length) {
	String Topic(topic);
	String Payload;
	Payload.reserve(length);
	for (unsigned int i = 0; i < length; ++i) {
		Payload.concat((char) payload[i]);
	}
	Serial.print("Received message: ");
	Serial.print(Topic);
	Serial.print(" // ");
	Serial.println(Payload);
	if (Topic == "TopicESP/order") {
		if (Payload.equalsIgnoreCase("on")) {
			if (s_ioManager) {
				s_ioManager->setLEDState(true);
			}
		} else if (Payload.equalsIgnoreCase("off")) {
			if (s_ioManager) {
				s_ioManager->setLEDState(false);
			}
		} else {
			Serial.print("Invalid Payload: ");
			Serial.println(Payload);
		}
	}
}

MqttManager::MqttManager() : mqttClient(wifiClient) {}

void MqttManager::setup() {
	mqttClient.setServer(mqtt_server, mqtt_server_port);//mqtt_server.c_str() modif patrick en mqtt_server
	mqttClient.setKeepAlive(5);
	mqttClient.setCallback(callback);
}

void MqttManager::loop() {
	if (!mqttClient.connected()) {
		connect();
	}
	if (!mqttClient.loop()) {
	}
}

void MqttManager::connect() {

	while (!mqttClient.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Serial.print(mqtt_server.toString);//(mqtt_server.c_str()
		Serial.print("  ");
		Serial.print(mqtt_server_port);
		String mqttClientId = "";
		if (mqttClient.connect(mqttClientId.c_str())) {
			Serial.println("connected");
			mqttClient.subscribe(mqttTopicIn.c_str());
			Serial.print("subscribed to ");
			Serial.println(mqttTopicIn);
			senMessage("TopicESP/Welcome", "hello");
		} else {
			Serial.print("failed, rc=");
			Serial.print(mqttClient.state());
			Serial.println(" will try again in 5 seconds");
			Serial.print("remote ip");
			Serial.print(wifiClient.remoteIP().toString());
			Serial.print(":");
			Serial.println(wifiClient.remotePort());
			delay(5000);
		}
	}
}

void MqttManager::senMessage(String topic, String Payload) {
	Serial.print("Sending Message: ");
	Serial.print(topic);
	Serial.print(" // ");
	Serial.println(Payload);
	mqttClient.publish(topic.c_str(), Payload.c_str());
}

void MqttManager::attachIOManager(IOManager *mngr) {
	ioManager = mngr;
	s_ioManager = mngr;
}