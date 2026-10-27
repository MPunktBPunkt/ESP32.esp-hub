#include "HardwareInfo.h"

void HardwareInfo::logToSerial() const {
    Serial.printf("[HW] Chip=%s Rev%d Cores=%d Flash=%u KB\n",
                  chipModel().c_str(), chipRevision(),
                  chipCores(), flashSizeKb());
    size_t psram = psramSize();
    if (psram > 0) {
        Serial.printf("[HW] PSRAM=%u KB frei=%u KB\n",
                      (unsigned)(psram / 1024U), (unsigned)(freePsram() / 1024U));
    } else {
        Serial.println("[HW] PSRAM=0 (Modul fehlt oder Build ohne QSPI PSRAM)");
    }
}

String HardwareInfo::chipModel() const { return String(ESP.getChipModel()); }
uint8_t HardwareInfo::chipRevision() const { return ESP.getChipRevision(); }
uint8_t HardwareInfo::chipCores() const { return ESP.getChipCores(); }
uint32_t HardwareInfo::flashSizeKb() const { return ESP.getFlashChipSize() / 1024U; }
size_t HardwareInfo::psramSize() const { return ESP.getPsramSize(); }
size_t HardwareInfo::freePsram() const { return ESP.getFreePsram(); }
uint32_t HardwareInfo::freeHeap() const { return ESP.getFreeHeap(); }
uint32_t HardwareInfo::freeSketchSpace() const { return ESP.getFreeSketchSpace(); }
