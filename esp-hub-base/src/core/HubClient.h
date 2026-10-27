#pragma once

/**
 * @file HubClient.h
 * @brief ESP-Hub register/heartbeat client.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include <Arduino.h>
#include "EspHubConfig.h"
#include "HardwareInfo.h"

/**
 * @brief ESP-Hub HTTP register/heartbeat client (POST /api/register).
 */
class HubClient {
public:
    void setHardware(const HardwareInfo* hw) { hw_ = hw; }
    void configure(const String& deviceName, const String& hubHost, int hubPort);

    // Returns true on HTTP 200. Updates lastSuccessMs on success.
    // May set otaPending/otaUrl when Hub requests OTA.
    bool sendHeartbeat(unsigned long& lastSuccessMs,
                       unsigned long& heartbeatIntervalMs,
                       bool& otaPending,
                       String& otaUrl);

    String buildHeartbeatJson() const;

private:
    String deviceName_ = DEVICE_NAME;
    String hubHost_ = HUB_HOST;
    int hubPort_ = HUB_PORT;
    const HardwareInfo* hw_ = nullptr;
};
