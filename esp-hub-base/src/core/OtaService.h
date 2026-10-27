#pragma once

/**
 * @file OtaService.h
 * @brief HTTP firmware download and flash (blocking baseline OTA).
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include <Arduino.h>

/**
 * @brief Blocking HTTP firmware download and flash (baseline; lifecycle in M9).
 */
class OtaService {
public:
    // Blocking HTTP download + flash (baseline; lifecycle in M9).
    void performFromUrl(const String& url);
};
