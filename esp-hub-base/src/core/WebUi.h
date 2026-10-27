#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "EspHubConfig.h"
#include "HardwareInfo.h"

class WebUi {
public:
    void begin(const String& deviceName, const String& hubHost, int hubPort,
               unsigned long heartbeatIntervalMs);
    void updateConfig(const String& deviceName, const String& hubHost, int hubPort,
                      unsigned long heartbeatIntervalMs);
    void handleClient();
    void pushSseIfDue(unsigned long nowMs);

    String buildStatusJson() const;

private:
    void registerRoutes();
    void handleRoot();
    void handleStatus();
    void handleEvents();
    void handleOtaPage();
    void handleOtaUpload();
    void handleOtaUploadFinish();
    void handleNotFound();
    void sseSend(const String& data);

    WebServer webServer_{80};
    WiFiClient sseClient_;
    HardwareInfo hw_;

    String deviceName_ = DEVICE_NAME;
    String hubHost_ = HUB_HOST;
    int hubPort_ = HUB_PORT;
    unsigned long heartbeatIntervalMs_ = (unsigned long)DEFAULT_INTERVAL_S * 1000UL;
    unsigned long lastSseSend_ = 0;
};
