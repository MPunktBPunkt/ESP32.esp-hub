#pragma once

#include "PanicTail.h"

/**
 * Boot-facing facade over PanicTail (D-003/D-009).
 * Owns no storage — PanicTail holds RTC; this documents the boot contract.
 */
namespace BootContext {
inline void begin() {
    PanicTail::begin();
    PanicTail::logToSerial();
}
}  // namespace BootContext
