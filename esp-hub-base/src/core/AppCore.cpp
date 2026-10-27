#include "AppCore.h"
#include "BootContext.h"
#include "Checkpoint.h"
#include "PanicTail.h"

#include <ESPmDNS.h>
#include <WiFi.h>

void AppCore::begin() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== ESP-Hub v" FW_VERSION " ===");

    BootContext::begin();

    hw_.probe();
    hw_.logToSerial();
    hub_.setHardware(&hw_);
    web_.setHardware(&hw_);

    net_.checkResetButton();

    Checkpoint::enter(Checkpoint::Wifi);
    net_.begin(deviceName_, hubHost_, hubPort_);
    Checkpoint::leave(Checkpoint::Wifi);
    PanicTail::breadcrumb(PanicTail::WifiUp);

    hub_.configure(deviceName_, hubHost_, hubPort_);

    String mdns = "esphub-" + WiFi.macAddress();
    mdns.replace(":", "");
    mdns.toUpperCase();
    mdns = "esphub-" + mdns.substring(6);
    if (MDNS.begin(mdns.c_str())) Serial.println("[mDNS] " + mdns + ".local");

    Checkpoint::enter(Checkpoint::Web);
    web_.begin(deviceName_, hubHost_, hubPort_, heartbeatIntervalMs_);
    Checkpoint::leave(Checkpoint::Web);
    PanicTail::breadcrumb(PanicTail::WebUp);

    lastSuccess_ = millis();
    Checkpoint::enter(Checkpoint::Hub);
    if (hub_.sendHeartbeat(lastSuccess_, heartbeatIntervalMs_, otaPending_, otaUrl_)) {
        Checkpoint::leave(Checkpoint::Hub);
        PanicTail::breadcrumb(PanicTail::HubOk);
    } else {
        Checkpoint::leave(Checkpoint::Hub);
    }
    web_.updateConfig(deviceName_, hubHost_, hubPort_, heartbeatIntervalMs_);
    lastHeartbeat_ = millis();
}

void AppCore::loop() {
    web_.handleClient();
    unsigned long now = millis();
    web_.pushSseIfDue(now);

    if (now - lastHeartbeat_ >= heartbeatIntervalMs_) {
        lastHeartbeat_ = now;
        Checkpoint::enter(Checkpoint::Hub);
        if (hub_.sendHeartbeat(lastSuccess_, heartbeatIntervalMs_, otaPending_, otaUrl_)) {
            Checkpoint::leave(Checkpoint::Hub);
            PanicTail::breadcrumb(PanicTail::HubOk);
        } else {
            Checkpoint::leave(Checkpoint::Hub);
        }
        web_.updateConfig(deviceName_, hubHost_, hubPort_, heartbeatIntervalMs_);
    }

    if (otaPending_) {
        otaPending_ = false;
        PanicTail::breadcrumb(PanicTail::OtaStart);
        Checkpoint::enter(Checkpoint::Ota);
        ota_.performFromUrl(otaUrl_);
        // On success, requestRestart clears open ckpt; on failure leave explicitly.
        Checkpoint::leave(Checkpoint::Ota);
        otaUrl_ = "";
    }

#if WATCHDOG_TIMEOUT_S > 0
    if (now - lastSuccess_ > (unsigned long)WATCHDOG_TIMEOUT_S * 1000UL) {
        Serial.println("[WDT] Neustart");
        PanicTail::breadcrumb(PanicTail::WdtFire);
        PanicTail::requestRestart(PanicTail::RestartWatchdog);
    }
#endif

    net_.maintain();
    delay(10);
}
