#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <WiFiManager.h>
#include "EspHubConfig.h"

// WiFiManager portal + NVS name/hub settings + BOOT reset gesture.
class NetworkWifi {
public:
    void checkResetButton();
    void begin(String& deviceName, String& hubHost, int& hubPort);
    void maintain();  // reconnect helper (baseline: delay+reconnect)

    WiFiManager& manager() { return wifiManager_; }

private:
    Preferences prefs_;
    WiFiManager wifiManager_;
};
