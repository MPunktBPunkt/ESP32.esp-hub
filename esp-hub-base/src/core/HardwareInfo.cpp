/**
 * @file HardwareInfo.cpp
 * @brief Hardware capability probe and status field helpers.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include "HardwareInfo.h"

void HardwareInfo::probe() {
    chipModel_ = String(ESP.getChipModel());
    chipRevision_ = ESP.getChipRevision();
    chipCores_ = ESP.getChipCores();
    flashSizeKb_ = ESP.getFlashChipSize() / 1024U;
    // Never assume PSRAM — measure only (D-019).
    psramSize_ = ESP.getPsramSize();
    probed_ = true;
}

void HardwareInfo::logToSerial() const {
    Serial.printf("[HW] Chip=%s Rev%d Cores=%d Flash=%u KB\n",
                  chipModel_.c_str(), chipRevision_,
                  chipCores_, flashSizeKb_);
    if (hasPsram()) {
        Serial.printf("[HW] PSRAM=%u KB frei=%u KB\n",
                      (unsigned)(psramSize_ / 1024U),
                      (unsigned)(freePsram() / 1024U));
    } else {
        Serial.println("[HW] PSRAM=0 (Modul fehlt oder Build ohne QSPI PSRAM)");
    }
}

size_t HardwareInfo::freePsram() const { return ESP.getFreePsram(); }
uint32_t HardwareInfo::freeHeap() const { return ESP.getFreeHeap(); }
uint32_t HardwareInfo::freeSketchSpace() const { return ESP.getFreeSketchSpace(); }

void HardwareInfo::appendStatusFields(String& json) const {
    // Additive keys only — do not rename baseline fields.
    json += ",\"chipRev\":" + String(chipRevision_);
    json += ",\"cores\":" + String(chipCores_);
    json += ",\"flashKb\":" + String(flashSizeKb_);
    json += ",\"hasPsram\":";
    json += hasPsram() ? "true" : "false";
}
