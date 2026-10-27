// ╔══════════════════════════════════════════════════════════════╗
// ║  esp-hub-base.ino — ESP-Hub Standard-Firmware für ESP32     ║
// ║  Version: 1.9.2                                             ║
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
// ║  WLAN zuruecksetzen:                                        ║
// ║    BOOT-Taste 3s beim Einschalten gedrueckt halten          ║
// ║  ESP32-S3: USB-Port "COM" (CH343) + RST vor dem Flashen     ║
// ║  YD-ESP32-S3 N8R2 (8MB Flash + 2MB Quad-PSRAM):             ║
// ║    Board ESP32S3 Dev Module | PSRAM: QSPI | Flash: 8MB      ║
// ║    Partition: Minimal SPIFFS | USB: COM, nicht OTG          ║
// ║    Hub-FQBN: ...esp32s3:PSRAM=enabled,FlashSize=8M,         ║
// ║              PartitionScheme=min_spiffs                     ║
// ║    (OPI PSRAM / qio_opi nur fuer R8/Octal — NICHT N8R2)     ║
// ╚══════════════════════════════════════════════════════════════╝

#include "ProjectConfig.h"


// ================================================================
//  EIGENE HARDWARE  <-- Pins und Sensoren hier definieren
// ================================================================
//
//  #define RELAY1_PIN    12
//  #define DIN1_PIN      14
//  #define USE_TEMPERATURE  1
//  #define TEMP_PIN         4


#include "src/core/IoTypes.h"
#include "src/core/AppCore.h"


// ================================================================
//  IO-TABELLE  <-- Messwerte die im Dashboard angezeigt werden
// ================================================================

IoValue ioTable[] = {
    // { "temperature", "sensor", 0.0, "C"   },
    // { "humidity",    "sensor", 0.0, "%"   },
    // { "relay1",      "output", 0.0, ""    },
};

const int IO_COUNT = sizeof(ioTable) / sizeof(ioTable[0]);


// ================================================================
//  MESSWERTE EINLESEN  <-- Eigene Sensor-Logik hier implementieren
// ================================================================

void updateIoValues() {
    // Eigene Sensor-Logik hier:
    // for (int i = 0; i < IO_COUNT; i++) {
    //     if (ioTable[i].key == "temperature") ioTable[i].value = readTemp();
    // }
}


// ================================================================
//  AppCore Lifecycle  (intern — siehe src/core/)
// ================================================================

static AppCore app;

void setup() { app.begin(); }
void loop()  { app.loop(); }
