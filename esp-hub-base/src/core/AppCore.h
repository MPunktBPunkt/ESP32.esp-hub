#pragma once

#include "EspHubConfig.h"
#include "HardwareInfo.h"
#include "NetworkWifi.h"
#include "HubClient.h"
#include "WebUi.h"
#include "OtaService.h"

// Central lifecycle (D-005). begin()/loop() replace monolithic setup/loop bodies.
class AppCore {
public:
    void begin();
    void loop();

private:
    HardwareInfo hw_;
    NetworkWifi net_;
    HubClient hub_;
    WebUi web_;
    OtaService ota_;

    String deviceName_ = DEVICE_NAME;
    String hubHost_ = HUB_HOST;
    int hubPort_ = HUB_PORT;

    unsigned long lastHeartbeat_ = 0;
    unsigned long heartbeatIntervalMs_ = (unsigned long)DEFAULT_INTERVAL_S * 1000UL;
    unsigned long lastSuccess_ = 0;
    bool otaPending_ = false;
    String otaUrl_;
};
