// ============================================================
// config.h — ESP-Hub Base Firmware Konfiguration
// Anpassen bevor du den Sketch kompilierst!
// ============================================================

#pragma once

// ── Gerätename ──────────────────────────────────────────────
// Angezeigt im ESP-Hub Dashboard
#define DEVICE_NAME   "Mein ESP32"

// ── Firmware-Version ────────────────────────────────────────
#define FW_VERSION    "1.0.0"

// ── ESP-Hub ioBroker Adapter ────────────────────────────────
// IP-Adresse des ioBroker-Hosts (muss mit "Adapter-Host" in den Adapter-Einstellungen übereinstimmen)
#define HUB_HOST      "192.168.178.1"
#define HUB_PORT      8093

// ── Heartbeat-Intervall ─────────────────────────────────────
// Standard-Intervall in Sekunden (wird vom Server überschrieben)
#define DEFAULT_INTERVAL_S  30

// ── WiFiManager AP-Name ─────────────────────────────────────
// WLAN-Netzwerk das beim Erststart erscheint (Captive Portal)
#define WIFI_AP_NAME  "ESP-Hub-Setup"

// ── Timeout WiFiManager ─────────────────────────────────────
// Sekunden bis Captive Portal abbricht (0 = kein Timeout)
#define WIFI_PORTAL_TIMEOUT_S  180

// ── Watchdog ─────────────────────────────────────────────────
// Sekunden ohne Heartbeat-Erfolg bis Neustart (0 = deaktiviert)
#define WATCHDOG_TIMEOUT_S  300

// ── Beispiel-IOs (anpassen für deine Hardware) ───────────────
// Definiere hier deine Pins. Die Werte werden im Heartbeat gemeldet.
// Kommentiere aus was du nicht brauchst.

// Temperatursensor (DS18B20 am Pin D4)
// #define USE_TEMPERATURE   1
// #define TEMP_PIN          4

// DHT22 Temperatur + Feuchtigkeit
// #define USE_DHT           1
// #define DHT_PIN           5
// #define DHT_TYPE          DHT22

// Relais-Ausgänge
// #define RELAY1_PIN        12
// #define RELAY2_PIN        13

// Digitale Eingänge
// #define DIN1_PIN          14
// #define DIN2_PIN          15
