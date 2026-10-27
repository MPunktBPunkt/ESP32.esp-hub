/**
 * @file esp-hub-base.ino
 * @brief Sketch entry: IO table, updateIoValues, AppCore setup/loop.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

// ╔══════════════════════════════════════════════════════════════╗
// ║  esp-hub-base.ino — ESP-Hub Standard-Firmware für ESP32     ║
// ║  Version: 1.10.1                                            ║
// ╠══════════════════════════════════════════════════════════════╣
// ║  Bibliotheken (Arduino Library Manager):                    ║
// ║    - WiFiManager  von tablatronix / tzapu                   ║
// ║    - ArduinoJson  von bblanchon (v6 oder v7)                ║
// ║  Built-in:                                                  ║
// ║    - HTTPClient, Update, WebServer, Preferences, ESPmDNS    ║
// ╠══════════════════════════════════════════════════════════════╣
// ║  Quickstart:                                                ║
// ║    1. ProjectConfig.h anpassen (HUB_HOST!)                  ║
// ║    2. Gesamten Sketch-Ordner bauen (arduino-cli / IDE)      ║
// ║       — Core liegt unter src/core/ (nicht nur diese .ino)   ║
// ║    3. Hotspot "ESP-Hub-Setup" -> WLAN + Hub-IP eingeben     ║
// ║    4. Geraet erscheint im ESP-Hub Dashboard                 ║
// ║    5. Web-UI: http://<ESP-IP>/                              ║
// ╠══════════════════════════════════════════════════════════════╣
// ║  Optional: #define ESPHUB_ENABLE_EXAMPLE_MODULE 1           ║
// ║            vor dem AppCore-Include → Demo-BaseModule        ║
// ╚══════════════════════════════════════════════════════════════╝

#include "ProjectConfig.h"


// ================================================================
//  EIGENE HARDWARE  <-- Pins und Sensoren hier definieren
// ================================================================
//
//  #define RELAY1_PIN    12
//  #define DIN1_PIN      14


#include "src/core/IoTypes.h"
#include "src/core/AppCore.h"

#if ESPHUB_ENABLE_EXAMPLE_MODULE
#include "src/modules/ExampleUptimeModule.h"
static ExampleUptimeModule gExampleModule;
#endif


// ================================================================
//  IO-TABELLE  <-- Messwerte die im Dashboard angezeigt werden
// ================================================================

IoValue ioTable[] = {
    // { "temperature", "sensor", 0.0, "C"   },
};

const int IO_COUNT = sizeof(ioTable) / sizeof(ioTable[0]);


// ================================================================
//  MESSWERTE EINLESEN  <-- Eigene Sensor-Logik hier implementieren
// ================================================================

void updateIoValues() {
    // Eigene Sensor-Logik hier.
}


// ================================================================
//  AppCore Lifecycle  (intern — siehe src/core/)
// ================================================================

static AppCore app;

void setup() {
#if ESPHUB_ENABLE_EXAMPLE_MODULE
    app.modules().add(&gExampleModule);
#endif
    app.begin();
}

void loop() { app.loop(); }
