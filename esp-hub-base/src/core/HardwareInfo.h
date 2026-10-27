#pragma once

#include <Arduino.h>

// Core hardware/capability snapshot (D-017, D-019). Pure getters — no NVS.
struct HardwareInfo {
    void logToSerial() const;

    String chipModel() const;
    uint8_t chipRevision() const;
    uint8_t chipCores() const;
    uint32_t flashSizeKb() const;
    size_t psramSize() const;
    size_t freePsram() const;
    uint32_t freeHeap() const;
    uint32_t freeSketchSpace() const;
};
