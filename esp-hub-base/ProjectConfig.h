#pragma once

/**
 * @file ProjectConfig.h
 * @brief User-editable device/hub/timeout configuration macros.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#ifndef DEVICE_NAME
#define DEVICE_NAME            "Mein ESP32"
#endif
#ifndef FW_VERSION
#define FW_VERSION             "1.10.0"
#endif
/** Set to 1 in the sketch to register ExampleUptimeModule (M10 demo). */
#ifndef ESPHUB_ENABLE_EXAMPLE_MODULE
#define ESPHUB_ENABLE_EXAMPLE_MODULE 0
#endif
#ifndef HUB_HOST
#define HUB_HOST               "192.168.178.113"
#endif
#ifndef HUB_PORT
#define HUB_PORT               8093
#endif
#ifndef WIFI_AP_NAME
#define WIFI_AP_NAME           "ESP-Hub-Setup"
#endif
#ifndef WIFI_PORTAL_TIMEOUT_S
#define WIFI_PORTAL_TIMEOUT_S  180
#endif
#ifndef DEFAULT_INTERVAL_S
#define DEFAULT_INTERVAL_S     30
#endif
#ifndef WATCHDOG_TIMEOUT_S
#define WATCHDOG_TIMEOUT_S     300
#endif
/** ProgressWatch stall timeout (seconds); defaults to WATCHDOG_TIMEOUT_S. */
#ifndef PROGRESS_TIMEOUT_S
#define PROGRESS_TIMEOUT_S     WATCHDOG_TIMEOUT_S
#endif
#ifndef RESET_BUTTON_PIN
#define RESET_BUTTON_PIN       0
#endif
#ifndef RESET_HOLD_SEC
#define RESET_HOLD_SEC         3
#endif
