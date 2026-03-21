# esp32.EspHub — ESP32/ESP8266 Standard-Firmware für iobroker.esp-hub

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Donate](https://img.shields.io/badge/Donate-PayPal-00457C.svg?logo=paypal)](https://www.paypal.com/donate/?business=martin%40bchmnn.de&currency_code=EUR)

> Standard-Firmware für den [iobroker.esp-hub](https://github.com/MPunktBPunkt/iobroker.esp-hub) Adapter.

---

## Inhalt

```
esp32.EspHub/
└── esp-hub-base/
    ├── esp-hub-base.ino   Haupt-Sketch
    ├── config.h           Benutzer-Konfiguration
    └── README.md
```

---

## Quickstart

### 1. Bibliotheken installieren (Arduino Library Manager)

- **WiFiManager** von tablatronix (tzapu) 
- **ArduinoJson** von bblanchon (v6 oder v7)

### 2. Board konfigurieren

Arduino IDE → Board: **ESP32 Dev Module** (oder dein spezifisches Board)

### 3. `config.h` anpassen

```cpp
#define DEVICE_NAME   "Mein ESP32"         // Angezeigter Name
#define HUB_HOST      "192.168.178.1"      // ioBroker IP
#define HUB_PORT      8093                 // ESP-Hub Port
#define FW_VERSION    "1.0.0"
```

### 4. Flashen & Verbinden

1. Sketch auf ESP32 flashen
2. ESP startet als WLAN-Hotspot **"ESP-Hub-Setup"**
3. Mit Smartphone/PC mit diesem WLAN verbinden
4. Captive Portal öffnet sich automatisch (oder `192.168.4.1` im Browser)
5. WLAN-Zugangsdaten + ESP-Hub IP eingeben
6. ESP verbindet sich → erscheint im Dashboard

---

## Eigene IOs hinzufügen

In `esp-hub-base.ino`:

1. `ioTable[]` erweitern:
```cpp
IoValue ioTable[] = {
    { "temperature", "sensor", 0.0, "°C" },
    { "humidity",    "sensor", 0.0, "%" },
    { "relay1",      "output", 0.0, "" },
};
```

2. `updateIoValues()` mit eigenem Code füllen:
```cpp
void updateIoValues() {
    ioTable[0].value = readTemperature();   // eigene Funktion
    ioTable[1].value = readHumidity();
    ioTable[2].value = digitalRead(RELAY1_PIN);
}
```

Die Werte erscheinen automatisch in der Device-Card des Dashboards.

---

## OTA-Updates

1. Neue Firmware als `.bin` kompilieren (Arduino IDE → Sketch → Exportiere kompilierte Binärdatei)
2. Im ESP-Hub Dashboard → System → Firmware hochladen
3. Gerät auswählen + Firmware wählen + "OTA" klicken
4. Beim nächsten Heartbeat (max. 30s) führt der ESP das Update durch

---

## Watchdog

Der Software-Watchdog (`WATCHDOG_TIMEOUT_S = 300`) startet den ESP neu, wenn 5 Minuten kein Heartbeat erfolgreich war. Das verhindert hängende Geräte.

---

## Lizenz

GPL-3.0 © Martin Buchmann

[![Donate](https://img.shields.io/badge/Donate-PayPal-00457C.svg?logo=paypal)](https://www.paypal.com/donate/?business=martin%40bchmnn.de&currency_code=EUR)
