#pragma once

#include <Arduino.h>
#include "EspHubConfig.h"
#include "HardwareInfo.h"

class HubClient {
public:
    void configure(const String& deviceName, const String& hubHost, int hubPort);

    // Returns true on HTTP 200. Updates lastSuccessMs on success.
    // May set *otaUrlOut and return via out-params when Hub requests OTA.
    bool sendHeartbeat(unsigned long& lastSuccessMs,
                       unsigned long& heartbeatIntervalMs,
                       bool& otaPending,
                       String& otaUrl);

    String buildHeartbeatJson() const;

private:
    String deviceName_ = DEVICE_NAME;
    String hubHost_ = HUB_HOST;
    int hubPort_ = HUB_PORT;
    HardwareInfo hw_;
};
