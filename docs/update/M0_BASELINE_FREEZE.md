# M0 — Contract & Baseline Freeze

**Firmware:** `esp-hub-base` **v1.8.0**  
**Quelle:** `esp-hub-base.ino` + `README.md` (Stand Release 1.8.0)  
**Zweck:** Referenz vor jedem Umbau (M1+). Regressionen werden gegen dieses Dokument gemessen.

Legende:

- **MUST** — harter Kompatibilitätsanker; Änderung nur mit bewusster Vertragsversionierung  
- **IS** — dokumentiertes Ist-Verhalten; Internals dürfen sich ändern, solange beobachtbares Verhalten gleich bleibt  
- **KNOWN-ISSUE** — bekanntes Defizit; darf bis zur geplanten Milestone nicht stillschweigend „anders kaputt“ werden

---

## 1. Build / Projektform

| ID | Klasse | Vertrag |
|----|--------|---------|
| B-01 | MUST | Arduino-Sketch `esp-hub-base` (Baseline 1.8.0: eine `.ino`; ab M1/1.9.0: Ordner `esp-hub-base/` + `src/core/`, kein PlatformIO-Zwang — D-018) |
| B-02 | MUST | Bibliotheken: WiFiManager (tzapu), ArduinoJson v6 oder v7; Built-ins: HTTPClient, Update, WebServer, Preferences, ESPmDNS |
| B-03 | IS | Benutzer-Abschnitte: KONFIGURATION, EIGENE HARDWARE, IO-TABELLE, `updateIoValues()`; danach interner Block „AB HIER NICHT VERÄNDERN“ |
| B-04 | IS | Vorkompilierte Bins: `esp-hub-base.1.8.0.esp32.bin`, `esp-hub-base.1.8.0.esp32s3.bin` |

---

## 2. Konfigurations-Defaults (Sketch)

| Symbol | Default (1.8.0) |
|--------|-----------------|
| `DEVICE_NAME` | `"Mein ESP32"` |
| `FW_VERSION` | `"1.8.0"` |
| `HUB_HOST` | Compile-Zeit-Default (Portal überschreibt) |
| `HUB_PORT` | `8093` |
| `WIFI_AP_NAME` | `"ESP-Hub-Setup"` |
| `WIFI_PORTAL_TIMEOUT_S` | `180` |
| `DEFAULT_INTERVAL_S` | `30` |
| `WATCHDOG_TIMEOUT_S` | `300` |
| `RESET_BUTTON_PIN` | `0` (BOOT) |
| `RESET_HOLD_SEC` | `3` |

**MUST:** Keine WLAN-SSID/PSK im Sketch (WiFiManager/NVS only).

---

## 3. Boot-Sequenz (beobachtbar)

Reihenfolge in `setup()`:

1. `Serial.begin(115200)`, kurze Pause, Banner `=== ESP-Hub v… ===`
2. Hardware-Log: Chip-Model, Rev, Cores, Flash-KB
3. PSRAM-Log: Größe/frei oder `PSRAM=0 …`
4. `checkResetButton()`
5. `setupWifi()` (Portal oder AutoConnect)
6. mDNS: `esphub-<mac6>.local` (`mac6` = letzte 6 Hex der MAC ohne `:`)
7. `setupWebServer()` auf Port 80
8. Erster `sendHeartbeat()`
9. `lastHeartbeat` / `lastSuccess` gesetzt

**IS:** Reset-Button-Check und WiFi-Portal können den Boot blockieren (KNOWN-ISSUE Timing).

---

## 4. WLAN-Reset

| ID | Klasse | Vertrag |
|----|--------|---------|
| R-01 | MUST | BOOT-Taste (`GPIO0`) ≥ 3 s beim Einschalten → `wifiManager.resetSettings()` + NVS-Namespace `esphub` clear + Restart |
| R-02 | IS | Während Halten: blockierende Poll-Schleife mit `delay(100)` |

---

## 5. WLAN / WiFiManager

| ID | Klasse | Vertrag |
|----|--------|---------|
| W-01 | MUST | Captive-Portal-SSID `ESP-Hub-Setup` |
| W-02 | MUST | Portal-Parameter: Gerätename, Hub-IP, Hub-Port |
| W-03 | MUST | Nach erfolgreichem Connect: Werte in NVS speichern |
| W-04 | IS | `autoConnect`-Timeout → Restart |
| W-05 | IS | Bei Disconnect in `loop`: `delay(5000)`, dann ggf. `WiFi.reconnect()` (KNOWN-ISSUE Blocking) |

---

## 6. NVS

| ID | Klasse | Vertrag |
|----|--------|---------|
| N-01 | MUST | Namespace: `esphub` |
| N-02 | MUST | Keys: `name` (String), `hub_host` (String), `hub_port` (Int) |
| N-03 | MUST | WLAN-Credentials liegen in WiFiManager/ESP-NVS, nicht im Git |

---

## 7. Hardware / Telemetrie-Felder

Gemessen zur Laufzeit (D-019):

- `ESP.getChipModel()`, Revision, Cores, Flash-Größe
- `ESP.getFreeHeap()`, `ESP.getFreeSketchSpace()`
- `ESP.getPsramSize()`, `ESP.getFreePsram()` — `0` wenn Modul/Build ohne PSRAM

**MUST:** N8R2-Verhalten laut README: QSPI PSRAM im Build; keine OPI-Annahme.

---

## 8. Web-Endpunkte

| Methode | Pfad | Vertrag |
|---------|------|---------|
| GET | `/` | Status-HTML (SSE + Polling-Fallback auf `/api/status`) |
| GET | `/events` | Server-Sent Events, JSON-Payload = Status |
| GET | `/api/status` | `application/json`, CORS `*` |
| GET | `/ota` | OTA-Upload-UI |
| POST | `/ota-upload` | Multipart Firmware-Upload → Restart bei Erfolg |
| * | sonst | 302 → `/` |

SSE-Push-Intervall: **IS** alle 4 s wenn Client verbunden.

### `/api/status` JSON-Felder (MUST Keys)

| Key | Typ / Bedeutung |
|-----|-----------------|
| `name` | Gerätename |
| `chip` | Chip-Model |
| `mac` | MAC ohne `:`, Uppercase |
| `ip` | lokale IP |
| `rssi` | Zahl |
| `uptime` | formatierter String (`Ns` / `Nmin Ns` / `Nh Nmin`) |
| `heap` | free heap (Bytes) |
| `psram` | free PSRAM |
| `psramSize` | PSRAM total |
| `sketch` | free sketch space |
| `hub` | `"host:port"` |
| `interval` | Heartbeat-Intervall in Sekunden |
| `ios` | Objekt: `key → { v, u, t }` |

Additive (ab 1.9.1, optional): `chipRev`, `cores`, `flashKb`, `hasPsram`.  
Additive (ab 1.9.2, optional): `resetReason`, `restartCause`, `bootGen`.  
Neu ab 1.9.2: `GET /api/diag/boot` (Diag-Vorstufe, nicht Teil der 1.8.0-Baseline-Pflichtrouten).

---

## 9. Heartbeat (Hub-Vertrag)

| ID | Klasse | Vertrag |
|----|--------|---------|
| H-01 | MUST | `POST http://<hubHost>:<hubPort>/api/register` |
| H-02 | MUST | `Content-Type: application/json` |
| H-03 | MUST | Timeout 8 s (Ist) |
| H-04 | MUST | Bei HTTP 200: `lastSuccess` aktualisieren; optional `interval` (≥5 s) und `otaUrl` aus Response |
| H-05 | IS | Synchron/blockierend (KNOWN-ISSUE) |

### Heartbeat-Payload Keys (MUST)

| Key | Bemerkung |
|-----|-----------|
| `mac` | wie Status |
| `name` | |
| `hwType` | `"esp32"` |
| `chipModel` | |
| `version` | `FW_VERSION` |
| `ip` | |
| `rssi` | |
| `uptime` | Sekunden (Zahl), nicht formatierter String |
| `freeHeap` | |
| `freeSketch` | |
| `freePsram` | additiv |
| `psramSize` | additiv |
| `ios` | `key → { type, value, unit? }` |

**MUST (D-014):** Keine Diagnose-Dumps im Heartbeat. Spätere additive Felder nur schlank.

---

## 10. OTA

| ID | Klasse | Vertrag |
|----|--------|---------|
| O-01 | MUST | Hub-OTA: `otaUrl` in Register-Response → Download + `Update` + Restart |
| O-02 | MUST | Web-OTA: Upload `.bin` über `/ota-upload` |
| O-03 | IS | Hub-OTA blockierend (`HTTPClient` GET, Timeout 30 s) — KNOWN-ISSUE / Ziel D-015 |
| O-04 | IS | Flag `otaPending` + `otaUrl` global |

---

## 11. Loop / Timing (Ist)

| Verhalten | Klasse |
|-----------|--------|
| `webServer.handleClient()` | MUST (Erreichbarkeit) |
| Heartbeat nach `heartbeatInterval` | MUST |
| Watchdog: kein erfolgreicher Heartbeat seit `WATCHDOG_TIMEOUT_S` → Restart | IS / D-010 (ersetzt erst nach ProgressWatch) |
| `delay(10)` am Loop-Ende | IS |
| WiFi-Reconnect mit `delay(5000)` | KNOWN-ISSUE |

---

## 12. IO-Erweiterungspunkt

| ID | Klasse | Vertrag |
|----|--------|---------|
| I-01 | MUST | `IoValue { key, type, value, unit }` + `ioTable[]` + `IO_COUNT` |
| I-02 | MUST | `updateIoValues()` vor Status- und Heartbeat-Serialisierung |
| I-03 | IS | Leere Tabelle in Base-Firmware ist gültig |

---

## 13. Was intern geändert werden darf (ohne Vertragsbruch)

- Interne Struktur (Funktionen → Klassen/Dateien), solange Endpunkte/Payloads/Boot-Nutzerverhalten gleich bleiben
- Implementierung von Status-JSON (z. B. ArduinoJson statt String-Konkatenation), solange Keys/Semantik gleich
- Zusätzliche **additive** Status-/Diag-Endpunkte (D-013), ohne bestehende Keys zu entfernen
- Logging-Verbesserungen ohne neue Blocking-Writes im Hotpath (D-020)

## 14. Was nicht ohne Versions-/ADR-Schritt geändert werden darf

- Heartbeat-URL, Pflichtfelder, `hwType`
- NVS-Namespace/Keys `name` / `hub_host` / `hub_port`
- Portal-SSID und Reset-Geste
- Entfernen von `/`, `/events`, `/api/status`, `/ota`, `/ota-upload`
- Entfernen des alten Watchdogs vor erfolgreichem M7/M11 (D-010)
