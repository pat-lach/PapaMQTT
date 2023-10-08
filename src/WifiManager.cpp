//
// Created by damien on 08/10/23.
//
#include "WifiManager.h"

static const String ssid = "";
static const String password = "";

WifiManager::WifiManager() : timeClient(ntpUDP) {
}

void WifiManager::setup() {
	checkWifiConnexion();
	timeClient.begin();
}

void WifiManager::loop() {
	checkWifiConnexion();
	timeClient.update();
}

void WifiManager::checkWifiConnexion() {
	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("Wifi reconnecting.");
		uint32_t countdown = millis() + 15000u;
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid, password);
		Serial.print("Connecting ");
		while (WiFi.status() != WL_CONNECTED) {
			if (countdown > millis()) {
				delay(500);
				Serial.print(".");
			} else {
				Serial.println("Timed out! Restarting...");
				EspClass::reset();
			}
		}
		Serial.println(" connected.");
		Serial.print("Address: ");
		Serial.println(WiFi.localIP().toString());
		Serial.print("Strength: ");
		Serial.println(WiFi.RSSI());
	}
}
