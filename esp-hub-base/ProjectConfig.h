#pragma once

// ================================================================
//  KONFIGURATION  <-- Diese Werte anpassen (gilt für gesamtes Core)
// ================================================================

#ifndef DEVICE_NAME
#define DEVICE_NAME            "Mein ESP32"
#endif
#ifndef FW_VERSION
#define FW_VERSION             "1.9.0"
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
#ifndef RESET_BUTTON_PIN
#define RESET_BUTTON_PIN       0
#endif
#ifndef RESET_HOLD_SEC
#define RESET_HOLD_SEC         3
#endif
