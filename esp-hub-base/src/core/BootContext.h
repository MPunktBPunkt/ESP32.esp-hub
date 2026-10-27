#pragma once

/**
 * @file BootContext.h
 * @brief Boot facade that starts PanicTail and Checkpoint.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include "DiagLog.h"
#include "PanicTail.h"
#include "Checkpoint.h"

/**
 * @brief Boot-facing facade over DiagLog, PanicTail and Checkpoint (D-003/D-006/D-007/D-009).
 *
 * Call order: DiagLog → PanicTail → Checkpoint, then a forced boot snapshot.
 */
namespace BootContext {
/** @brief Start diagnostic layers and print the boot summary. */
inline void begin() {
    DiagLog::begin();
    PanicTail::begin();
    PanicTail::logToSerial();
    Checkpoint::begin();
    Checkpoint::logToSerial();
    DiagLog::persist("boot", true);
}
}  // namespace BootContext
