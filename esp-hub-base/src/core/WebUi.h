#pragma once

/**
 * @file WebUi.h
 * @brief Embedded status/SSE/OTA web server.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "EspHubConfig.h"
#include "HardwareInfo.h"

/**
 * @brief Status page, SSE, /api/status, /api/diag/*, and web OTA upload.
 */
class WebUi {
public:
    void setHardware(const HardwareInfo* hw) { hw_ = hw; }
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
    const HardwareInfo* hw_ = nullptr;

    String deviceName_ = DEVICE_NAME;
    String hubHost_ = HUB_HOST;
    int hubPort_ = HUB_PORT;
    unsigned long heartbeatIntervalMs_ = (unsigned long)DEFAULT_INTERVAL_S * 1000UL;
    unsigned long lastSseSend_ = 0;
};
