#pragma once

#include <Arduino.h>

// Core hardware capabilities (D-017, D-019).
// Static board traits are probed once; free* values stay live.
// Single AppCore-owned instance is the source for Serial, Status, Heartbeat.
class HardwareInfo {
public:
    // Capture chip/flash/PSRAM traits. Safe to call again after OTA/restart only.
    void probe();

    void logToSerial() const;

    bool probed() const { return probed_; }

    const String& chipModel() const { return chipModel_; }
    uint8_t chipRevision() const { return chipRevision_; }
    uint8_t chipCores() const { return chipCores_; }
    uint32_t flashSizeKb() const { return flashSizeKb_; }
    uint32_t flashSizeBytes() const { return flashSizeKb_ * 1024U; }

    // Total PSRAM reported by IDF/Arduino (0 if module missing or build without QSPI).
    size_t psramSize() const { return psramSize_; }
    bool hasPsram() const { return psramSize_ > 0; }

    // Live runtime counters — always read from ESP APIs.
    size_t freePsram() const;
    uint32_t freeHeap() const;
    uint32_t freeSketchSpace() const;

    // Optional additive status fields (baseline keys unchanged).
    void appendStatusFields(String& json) const;

private:
    bool probed_ = false;
    String chipModel_;
    uint8_t chipRevision_ = 0;
    uint8_t chipCores_ = 0;
    uint32_t flashSizeKb_ = 0;
    size_t psramSize_ = 0;
};
