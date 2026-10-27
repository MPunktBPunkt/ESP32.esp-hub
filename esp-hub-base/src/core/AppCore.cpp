#include "AppCore.h"

#include <ESPmDNS.h>
#include <WiFi.h>

void AppCore::begin() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== ESP-Hub v" FW_VERSION " ===");
    hw_.logToSerial();

    net_.checkResetButton();
    net_.begin(deviceName_, hubHost_, hubPort_);

    hub_.configure(deviceName_, hubHost_, hubPort_);

    String mdns = "esphub-" + WiFi.macAddress();
    mdns.replace(":", "");
    mdns.toUpperCase();
    mdns = "esphub-" + mdns.substring(6);
    if (MDNS.begin(mdns.c_str())) Serial.println("[mDNS] " + mdns + ".local");

    web_.begin(deviceName_, hubHost_, hubPort_, heartbeatIntervalMs_);

    lastSuccess_ = millis();
    hub_.sendHeartbeat(lastSuccess_, heartbeatIntervalMs_, otaPending_, otaUrl_);
    web_.updateConfig(deviceName_, hubHost_, hubPort_, heartbeatIntervalMs_);
    lastHeartbeat_ = millis();
}

void AppCore::loop() {
    web_.handleClient();
    unsigned long now = millis();
    web_.pushSseIfDue(now);

    if (now - lastHeartbeat_ >= heartbeatIntervalMs_) {
        lastHeartbeat_ = now;
        hub_.sendHeartbeat(lastSuccess_, heartbeatIntervalMs_, otaPending_, otaUrl_);
        web_.updateConfig(deviceName_, hubHost_, hubPort_, heartbeatIntervalMs_);
    }

    if (otaPending_) {
        otaPending_ = false;
        ota_.performFromUrl(otaUrl_);
        otaUrl_ = "";
    }

#if WATCHDOG_TIMEOUT_S > 0
    if (now - lastSuccess_ > (unsigned long)WATCHDOG_TIMEOUT_S * 1000UL) {
        Serial.println("[WDT] Neustart");
        delay(500);
        ESP.restart();
    }
#endif

    net_.maintain();
    delay(10);
}
