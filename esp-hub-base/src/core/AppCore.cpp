/**
 * @file AppCore.cpp
 * @brief AppCore lifecycle implementation wiring core services.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include "AppCore.h"
#include "BootContext.h"
#include "Checkpoint.h"
#include "DiagLog.h"
#include "DiagSaved.h"
#include "PanicTail.h"
#include "ProgressWatch.h"
#include "Recovery.h"

#include <ESPmDNS.h>
#include <WiFi.h>

void AppCore::begin() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== ESP-Hub v" FW_VERSION " ===");

    BootContext::begin();
    ProgressWatch::begin();

    hw_.probe();
    hw_.logToSerial();
    // Safe Point: structured diag snapshot + text log snapshot (D-008/D-013).
    DiagSaved::persistBootSnapshot(&hw_);
    DiagLog::persist("hw", false);
    hub_.setHardware(&hw_);
    web_.setHardware(&hw_);

    net_.checkResetButton();

    Checkpoint::enter(Checkpoint::Wifi);
    net_.begin(deviceName_, hubHost_, hubPort_);
    Checkpoint::leave(Checkpoint::Wifi);
    PanicTail::breadcrumb(PanicTail::WifiUp);
    ProgressWatch::note(ProgressWatch::Idle);

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
        ProgressWatch::note(ProgressWatch::Heartbeat);
    } else {
        Checkpoint::leave(Checkpoint::Hub);
    }
    ProgressWatch::note(ProgressWatch::Idle);
    web_.updateConfig(deviceName_, hubHost_, hubPort_, heartbeatIntervalMs_);
    lastHeartbeat_ = millis();
}

void AppCore::loop() {
    web_.handleClient();
    unsigned long now = millis();
    ProgressWatch::note(ProgressWatch::Idle);
    web_.pushSseIfDue(now);

    if (now - lastHeartbeat_ >= heartbeatIntervalMs_) {
        lastHeartbeat_ = now;
        Checkpoint::enter(Checkpoint::Hub);
        if (hub_.sendHeartbeat(lastSuccess_, heartbeatIntervalMs_, otaPending_, otaUrl_)) {
            Checkpoint::leave(Checkpoint::Hub);
            PanicTail::breadcrumb(PanicTail::HubOk);
            ProgressWatch::note(ProgressWatch::Heartbeat);
        } else {
            Checkpoint::leave(Checkpoint::Hub);
        }
        web_.updateConfig(deviceName_, hubHost_, hubPort_, heartbeatIntervalMs_);
    }

    if (otaPending_) {
        otaPending_ = false;
        PanicTail::breadcrumb(PanicTail::OtaStart);
        ProgressWatch::note(ProgressWatch::Ota);
        Checkpoint::enter(Checkpoint::Ota);
        ota_.performFromUrl(otaUrl_);
        // On success, requestRestart clears open ckpt; on failure leave explicitly.
        Checkpoint::leave(Checkpoint::Ota);
        ProgressWatch::note(ProgressWatch::Ota);
        otaUrl_ = "";
    }

    ProgressWatch::evaluate(now);

#if WATCHDOG_TIMEOUT_S > 0
    // Legacy time-since-last-heartbeat-success WDT — kept until ProgressWatch is proven (D-010).
    if (now - lastSuccess_ > (unsigned long)WATCHDOG_TIMEOUT_S * 1000UL) {
        Serial.println("[WDT] Neustart");
        DiagLog::printf("[WDT] legacy fire age_s=%lu prog_hb_stalled=%d\n",
                        (unsigned long)((now - lastSuccess_) / 1000UL),
                        ProgressWatch::stalled(ProgressWatch::Heartbeat, now) ? 1 : 0);
        PanicTail::breadcrumb(PanicTail::WdtFire);
        Recovery::requestRestart(Recovery::LegacyWatchdog, "heartbeat_timeout");
    }
#endif

    net_.maintain();
    delay(10);
}
