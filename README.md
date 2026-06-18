# ESP32.esp-hub — Basis-Firmware

![Version](https://img.shields.io/badge/version-1.7.0-blue)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Donate](https://img.shields.io/badge/Donate-PayPal-00457C.svg?logo=paypal)](https://www.paypal.com/donate/?business=martin%40bchmnn.de&currency_code=EUR)

> **Standard-Firmware für ESP32** — WiFi, Heartbeat, OTA und Web-UI als Ausgangspunkt für alle ESP-Hub-Projekte. Registriert sich beim [iobroker.esp-hub](https://github.com/MPunktBPunkt/iobroker.esp-hub) Adapter.

---

## Überblick

`esp-hub-base` ist die gemeinsame Basis aller MPunktBPunkt ESP32-Firmwares. Sie übernimmt WLAN-Konfiguration (WiFiManager), periodischen Heartbeat an ioBroker, OTA-Updates und eine einfache Status-Webseite. Eigene Sensoren und Logik werden in klar markierten Abschnitten ergänzt — der Rest bleibt unverändert.

---

## Features

- **WiFiManager** — Captive Portal `ESP-Hub-Setup`, kein Hardcoding nötig
- **ESP-Hub Heartbeat** — automatische Registrierung und Online-Status
- **IO-Werte** — eigene Messwerte im Dashboard (`ios`-Objekt)
- **OTA** — Push vom Hub oder Drag & Drop im Browser
- **mDNS** — Erreichbar als `http://<name>.local/`
- **Name-Sync** — Umbenennung im Hub wird übernommen

---

## Voraussetzungen

| Typ | Details |
|-----|---------|
| **Board** | ESP32 / ESP8266 |
| **WiFiManager** | tablatronix / tzapu |
| **ArduinoJson** | bblanchon v6 oder v7 |
| **ioBroker** | [iobroker.esp-hub](https://github.com/MPunktBPunkt/iobroker.esp-hub) |

---

## Quickstart

1. `esp-hub-base.ino` öffnen
2. Abschnitt **KONFIGURATION** anpassen:

```cpp
#define DEVICE_NAME  "Mein ESP32"
#define HUB_HOST     "192.168.178.113"
#define HUB_PORT     8093
```

3. Flashen → Hotspot **`ESP-Hub-Setup`** → WLAN + Hub-IP konfigurieren
4. Gerät erscheint im Dashboard: `http://<ioBroker-IP>:8093`

---

## Sketch-Struktur

| Abschnitt | Beschreibung | Anpassen? |
|-----------|--------------|-----------|
| **KONFIGURATION** | Name, Hub-IP, Intervalle | ✅ |
| **EIGENE HARDWARE** | Pin-Definitionen | ✅ |
| **IO-TABELLE** | Messwerte fürs Dashboard | ✅ |
| **MESSWERTE EINLESEN** | `updateIoValues()` | ✅ |
| **AB HIER NICHT VERÄNDERN** | WiFi, Heartbeat, OTA, Loop | ❌ |

---

## Abgeleitete Projekte

| Projekt | Beschreibung |
|---------|--------------|
| [esp32.network](https://github.com/MPunktBPunkt/esp32.network) | Netzwerk-Scanner |
| [esp32.webradio](https://github.com/MPunktBPunkt/esp32.webradio) | Web-Radio im Browser |
| [esp32.io-control](https://github.com/MPunktBPunkt) | GPIO-Controller |
| [esp32.communicator](https://github.com/MPunktBPunkt) | ESP-NOW Messenger |

---

## Lizenz

GNU General Public License v3.0 © MPunktBPunkt — siehe [LICENSE](LICENSE)

[![Donate](https://img.shields.io/badge/Donate-PayPal-00457C.svg?logo=paypal)](https://www.paypal.com/donate/?business=martin%40bchmnn.de&currency_code=EUR)
