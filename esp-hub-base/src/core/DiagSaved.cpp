/**
 * @file DiagSaved.cpp
 * @brief NVS persistence for the boot diagnostic snapshot.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include "DiagSaved.h"
#include "EspHubConfig.h"
#include "PanicTail.h"
#include "Checkpoint.h"

#include <Preferences.h>

namespace {

constexpr const char* kNvsNs = "ehdiag";
constexpr const char* kKeySnap = "snap";
constexpr size_t kMaxSnapBytes = 2048;

String buildSnapshot(const HardwareInfo* hw) {
    // Live boot/checkpoint JSON, then wrap with save metadata.
    String body = PanicTail::buildDiagJson();
    // PanicTail::buildDiagJson already includes checkpoint via appendDiagFields.

    String j = "{";
    j += "\"saved\":true";
    j += ",\"fw\":\"";
    j += FW_VERSION;
    j += "\",\"bootGen\":";
    j += String(PanicTail::bootGeneration());
    j += ",\"resetReason\":\"";
    j += PanicTail::resetReason();
    j += "\",\"restartCause\":\"";
    j += PanicTail::restartCause();
    j += "\"";
    if (hw && hw->probed()) {
        j += ",\"chip\":\"";
        j += hw->chipModel();
        j += "\",\"flashKb\":";
        j += String(hw->flashSizeKb());
        j += ",\"psramSize\":";
        j += String((uint32_t)hw->psramSize());
        j += ",\"hasPsram\":";
        j += hw->hasPsram() ? "true" : "false";
    }
    j += ",\"diag\":";
    j += body;
    j += "}";
    return j;
}

}  // namespace

void DiagSaved::persistBootSnapshot(const HardwareInfo* hw) {
    String snap = buildSnapshot(hw);
    if (snap.length() > kMaxSnapBytes) {
        Serial.printf("[DIAG] snapshot truncated %u -> %u\n",
                      (unsigned)snap.length(), (unsigned)kMaxSnapBytes);
        snap = snap.substring(0, kMaxSnapBytes);
    }
    Preferences prefs;
    if (!prefs.begin(kNvsNs, false)) {
        Serial.println("[DIAG] NVS begin FAIL");
        return;
    }
    const size_t n = prefs.putString(kKeySnap, snap);
    prefs.end();
    Serial.printf("[DIAG] saved snapshot %u bytes\n", (unsigned)n);
}

String DiagSaved::loadJson() {
    Preferences prefs;
    if (!prefs.begin(kNvsNs, true)) {
        return String(F("{\"saved\":false,\"error\":\"nvs\"}"));
    }
    String snap = prefs.getString(kKeySnap, "");
    prefs.end();
    if (snap.length() == 0) {
        return String(F("{\"saved\":false}"));
    }
    return snap;
}

bool DiagSaved::hasSaved() {
    Preferences prefs;
    if (!prefs.begin(kNvsNs, true)) return false;
    const String snap = prefs.getString(kKeySnap, "");
    prefs.end();
    return snap.length() > 0;
}

void DiagSaved::clear() {
    Preferences prefs;
    if (!prefs.begin(kNvsNs, false)) return;
    prefs.remove(kKeySnap);
    prefs.end();
    Serial.println("[DIAG] snapshot cleared");
}
