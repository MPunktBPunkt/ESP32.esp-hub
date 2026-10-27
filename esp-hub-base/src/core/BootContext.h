#pragma once

#include "PanicTail.h"
#include "Checkpoint.h"

/**
 * Boot-facing facade (D-003/D-006/D-009).
 * Order: PanicTail (reset/trail) → Checkpoint (last-open latch).
 */
namespace BootContext {
inline void begin() {
    PanicTail::begin();
    PanicTail::logToSerial();
    Checkpoint::begin();
    Checkpoint::logToSerial();
}
}  // namespace BootContext
