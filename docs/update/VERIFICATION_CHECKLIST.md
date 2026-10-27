# Verification Checklist — Regression gegen Baseline v1.8.0

Nach jedem Milestone (mindestens manuell auf Referenzboard) abhaken.  
Bezug: [M0_BASELINE_FREEZE.md](M0_BASELINE_FREEZE.md).

**Empfohlene Hardware:** ESP32 und/oder YD-ESP32-S3 N8R2 (QSPI-PSRAM-Build).  
**Hub:** laufendes `iobroker.esp-hub` (Register + optional OTA).

---

## A. Build & Boot

- [x] Sketch/Firmware baut ohne Fehler (ESP32 und/oder ESP32-S3) — 1.10.4
- [x] Serial: Banner mit Versionsstring — `.88` USB: `ESP-Hub v1.10.x`
- [x] Serial: `[HW] Chip=… Rev… Cores=… Flash=…`
- [x] Serial: PSRAM-Zeile (`PSRAM=…` oder `PSRAM=0 …`)
- [x] N8R2 + QSPI-Build: `PSRAM≈2048 KB` (nicht 0) — `.88` Status `psramSize=2097152`

## B. WLAN-Reset & Portal

- [x] Frisches Gerät / nach Reset: Portal-SSID `ESP-Hub-Setup` — `.88` nach NVS-Wipe
- [x] Portal akzeptiert Name, Hub-IP, Port — `.88` manuell konfiguriert
- [x] Nach Connect: Gerät im LAN, Serial zeigt IP/Name/Hub
- [ ] BOOT 3 s beim Einschalten: WLAN+`esphub`-NVS weg, Portal erscheint erneut
- [x] Keine SSID/PSK im Quellcode

## C. mDNS & Web-UI

- [x] IP erreichbar (`.87` / `.88`)
- [x] `/` zeigt Statusseite
- [x] SSE `/events` verbindet (`text/event-stream` + `data:`)
- [x] `/ota` lädt Upload-UI

## D. `/api/status`

- [x] HTTP 200, `application/json`
- [x] Pflichtkeys: `name`, `chip`, `mac`, `ip`, `rssi`, `uptime`, `heap`, `psram`, `psramSize`, `sketch`, `hub`, `interval`, `ios`
- [x] `mac` ohne `:`, Uppercase
- [x] `uptime` formatierter String (nicht nur Sekunden-Zahl)
- [x] Bei PSRAM=0: UI blendet PSRAM aus (`.87`); bei S3 sichtbar (`.88`)
- [x] Additive: `hwSummary`, `module`, `cpu`/`cpu0`/`cpu1`, OTA-/Recovery-/Prog-Felder

## E. Heartbeat / Hub

- [x] Gerät in Hub-Liste mit Name, IP, Version, chipModel
- [x] Payload enthält Baseline-Felder; Heartbeat schlank
- [x] Hub-Rename → `reply.name` → Gerät übernimmt (1.10.4 + Hub 0.5.15)
- [ ] Hub kann `interval` setzen (≥5 s) → Gerät übernimmt (nicht explizit getestet)
- [x] Heartbeat bleibt schlank (kein Diagnose-Dump)

## F. OTA

- [x] Hub-Push `otaUrl` → Download, Flash, Restart (`otaState=writing`, `restartCause=hub_ota`)
- [x] Web-Upload `.bin` über `/ota-upload` → Restart (`web_ota`)
- [x] Fehlgeschlagenes OTA / Recovery: Gerät nach WDT-Bug-Fix (1.10.1) wieder bedienbar

## G. Watchdog (Legacy, D-010 deferred)

- [x] Legacy-WDT-Code aktiv; während OTA pausiert
- [x] False-Trip nach Heartbeat gefixt (1.10.1, millis re-sample)
- [ ] Bewusster Hub-Ausfall → Restart nach `WATCHDOG_TIMEOUT_S` (Field, lang)

## H. IO-Hook

- [x] Leere `ioTable` → gültiger Heartbeat/Status (`ios` `{}`)
- [ ] Mit Test-Eintrag in `ioTable` + `updateIoValues()`: Werte in Status und Hub sichtbar

## I. NVS

- [x] Nach Portal: Name/Hub überleben Reboot (`.88` nach Portal; Hub-Rename persistiert)
- [x] Namespace `esphub`
- [ ] `/api/diag/saved` liefert Snapshot (aktuell oft `{"saved":false,"error":"nvs"}` — offen)

## J. Non-Regression Timing (ab M1 qualitativ)

- [x] Web-UI bleibt während Normalbetrieb bedienbar
- [x] Keine neuen langen Flash-Writes im Idle-Loop (beobachtet)
- [x] Diagnostics erhöhen Boot-/Loop-Latenz nicht spürbar

---

## Ergebnisprotokoll

```text
Milestone: M0–M11 / FW 1.10.4
Datum: 2026-10-27
Board: .87 ESP32-D0WDQ6 (Network Scanner MAC C8C9A3CB3794)
       .88 ESP32-S3 N8R2 (PiDrive-Lab MAC 68B6B329339C)
FQBN: esp32 min_spiffs / esp32s3 PSRAM=enabled FlashSize=8M min_spiffs
Hub-Version: iobroker.esp-hub 0.5.15
Ergebnis: PASS (mit offenen Punkten B-BOOT-Hold, E-interval, G-Field-WDT, H-IO-Test, I-diag/saved)
Notizen:
- USB merged.bin 0x0 wischt NVS/WLAN — App-only 0x10000 bevorzugen
- OTA-Slot-Frei bei min_spiffs oft gleich (4 vs 8 MiB Chip) — UI zeigt Flash gesamt extra
- Hub-Rename braucht pendingName (0.5.15) + Gerät apply reply.name (1.10.4)
```
