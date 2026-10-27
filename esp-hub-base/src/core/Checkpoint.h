#pragma once

/**
 * Nested ENTER/LEAVE/FAIL checkpoints (D-006), generalized from hu-probe PhaseGate.
 * RTC (CRC) for soft-reset/panic; NVS mirror so power_on still sees last-open.
 */

#include <Arduino.h>

class Checkpoint {
public:
    enum Phase : uint8_t {
        None = 0,
        Wifi = 1,
        Web = 2,
        Hub = 3,
        Ota = 4,
    };

    /** Call once early after PanicTail::begin(). Latches last-open from previous boot. */
    static void begin();

    static void enter(Phase phase, uint32_t arg = 0);
    static void leave(Phase phase);   // PASS — must match stack top
    static void fail(Phase phase, uint32_t arg = 0);

    /** Clear open stack without latching crash (planned restart / OTA). */
    static void clearOpenForPlannedRestart();

    static const char* name(Phase phase);
    static Phase lastCrashPhase();
    static uint32_t lastCrashArg();
    static bool hasLastCrash();

    static void logToSerial();
    static void appendStatusFields(String& json);
    static void appendDiagFields(String& json);  // richer object for /api/diag/boot

private:
    Checkpoint() = delete;
};
