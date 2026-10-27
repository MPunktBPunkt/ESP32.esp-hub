#pragma once

/**
 * @file BootContext.h
 * @brief Boot facade that starts PanicTail and Checkpoint.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include "PanicTail.h"
#include "Checkpoint.h"

/**
 * @brief Boot-facing facade over PanicTail and Checkpoint (D-003/D-006/D-009).
 *
 * Call order: PanicTail (reset/trail) then Checkpoint (last-open latch).
 */
namespace BootContext {
/** @brief Start PanicTail and Checkpoint and print the boot summary. */
inline void begin() {
    PanicTail::begin();
    PanicTail::logToSerial();
    Checkpoint::begin();
    Checkpoint::logToSerial();
}
}  // namespace BootContext
