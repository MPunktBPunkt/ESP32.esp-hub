# Verification Checklist — Regression gegen Baseline v1.8.0

Nach jedem Milestone (mindestens manuell auf Referenzboard) abhaken.  
Bezug: [M0_BASELINE_FREEZE.md](M0_BASELINE_FREEZE.md).

**Empfohlene Hardware:** ESP32 und/oder YD-ESP32-S3 N8R2 (QSPI-PSRAM-Build).  
**Hub:** laufendes `iobroker.esp-hub` (Register + optional OTA).

---

## A. Build & Boot

- [ ] Sketch/Firmware baut ohne Fehler (ESP32 und/oder ESP32-S3)
- [ ] Serial: Banner mit Versionsstring
- [ ] Serial: `[HW] Chip=… Rev… Cores=… Flash=…`
- [ ] Serial: PSRAM-Zeile (`PSRAM=…` oder `PSRAM=0 …`)
- [ ] N8R2 + QSPI-Build: `PSRAM≈2048 KB` (nicht 0)

## B. WLAN-Reset & Portal

- [ ] Frisches Gerät / nach Reset: Portal-SSID `ESP-Hub-Setup`
- [ ] Portal akzeptiert Name, Hub-IP, Port
- [ ] Nach Connect: Gerät im LAN, Serial zeigt IP/Name/Hub
- [ ] BOOT 3 s beim Einschalten: WLAN+`esphub`-NVS weg, Portal erscheint erneut
- [ ] Keine SSID/PSK im Quellcode

## C. mDNS & Web-UI

- [ ] `http://esphub-<mac6>.local/` oder IP erreichbar
- [ ] `/` zeigt Statusseite
- [ ] SSE verbindet (`Live`) oder Polling-Fallback über `/api/status` funktioniert
- [ ] `/ota` lädt Upload-UI

## D. `/api/status`

- [ ] HTTP 200, `application/json`
- [ ] Pflichtkeys: `name`, `chip`, `mac`, `ip`, `rssi`, `uptime`, `heap`, `psram`, `psramSize`, `sketch`, `hub`, `interval`, `ios`
- [ ] `mac` ohne `:`, Uppercase
- [ ] `uptime` formatierter String (nicht nur Sekunden-Zahl)
- [ ] Bei PSRAM=0: UI blendet PSRAM sinnvoll aus / Werte 0

## E. Heartbeat / Hub

- [ ] `POST /api/register` erscheint im Hub / Adapter-Log
- [ ] Gerät in Hub-Liste mit Name, IP, Version, chipModel
- [ ] Payload enthält: `mac`, `name`, `hwType=esp32`, `chipModel`, `version`, `ip`, `rssi`, `uptime` (Zahl), `freeHeap`, `freeSketch`, `freePsram`, `psramSize`, `ios`
- [ ] Hub kann `interval` setzen (≥5 s) → Gerät übernimmt
- [ ] Heartbeat bleibt schlank (kein Diagnose-Dump)

## F. OTA

- [ ] Hub-Push `otaUrl` → Download, Flash, Restart, neue Version im Heartbeat
- [ ] Web-Upload `.bin` über `/ota-upload` → Restart, neue Version
- [ ] Fehlgeschlagenes OTA lässt Gerät bedienbar (kein Soft-Brick ohne Serial-Recovery)

## G. Watchdog (Legacy, bis M11)

- [ ] Bei `WATCHDOG_TIMEOUT_S > 0`: nach langem Hub-Ausfall (kein HTTP 200) → Restart (Baseline-Verhalten)
- [ ] Bis M7/M11: Logik nicht stillschweigend entfernt

## H. IO-Hook

- [ ] Leere `ioTable` → gültiger Heartbeat/Status (`ios` leer/Objekt ok)
- [ ] Mit Test-Eintrag in `ioTable` + `updateIoValues()`: Werte in Status und Hub sichtbar

## I. NVS

- [ ] Nach Portal: `name` / `hub_host` / `hub_port` überleben Power-Cycle
- [ ] Namespace weiterhin `esphub` (oder dokumentierte Migrationsnotiz + Dual-Read)

## J. Non-Regression Timing (ab M1 qualitativ)

- [ ] Web-UI bleibt während Normalbetrieb bedienbar
- [ ] Keine neuen langen Flash-Writes im Idle-Loop
- [ ] Diagnostics (ab M3+) erhöhen Boot-/Loop-Latenz nicht spürbar (D-020)

---

## Ergebnisprotokoll (kopieren)

```text
Milestone: M_
Datum:
Board:
FQBN / PSRAM:
Hub-Version:
Ergebnis: PASS / FAIL
Notizen:
```
