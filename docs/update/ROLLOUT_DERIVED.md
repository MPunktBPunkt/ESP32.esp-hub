# Rollout — Abgeleitete Firmwares auf esp-hub-base

**Stand:** Base **1.10.5**, Migration M0–M11 + gzip Web-UI fertig.  
**Ziel:** Produkt-Firmwares nutzen denselben Core (Hub/OTA/Recovery/HW/CPU/Rename), Projeklogik als Module oder klare App-Schicht.

## Reihenfolge

1. **esp32.rfmonitor** — **1.10.5** Stufe A/B validiert: `src/esphub/` (HardwareInfo, CpuLoad), Hub-Rename, `web/index.html` → gzip-PROGMEM — Labor `.87`/`.88`
2. **esp32.network** — **1.7.0** Stufe A/B validiert: Arduino-Monolith + `src/esphub/`, `web/*.html` → gzip — Labor `.87` Smoke OK (`.88` bleibt rfmonitor)
3. **esp32.io-control** — **1.7.0** Stufe A/B: Shared Core + gzip-UI (Build ESP32/S3 OK)
4. **esp32.webradio** — **2.5.0** Stufe A/B: Shared Core + gzip-UI (B64-UI → `web/`), Hub-Rename ergänzt
5. Weitere (communicator, …) nach gleichem Muster

## Strategie (inkrementell)

Nicht sofort alles auf `AppCore`+Stock-WebUi zwingen (RF/Network haben eigene UIs).

| Stufe | Inhalt |
|-------|--------|
| A | Shared Core **vendorn/syncen**: HardwareInfo, CpuLoad, Recovery, ProgressWatch, Checkpoint, PanicTail/BootContext, DiagLog, OtaService, HubClient (Rename) |
| B | Eigene Web-UI behalten; Status um `hwSummary`/`cpu*`/Recovery-Felder erweitern |
| C | Domänenlogik als `BaseModule` (oder äquivalent) hinter klarem Contract |
| D | Optional später: AppCore-Lifecycle vollständig, Stock-UI nur wo passend |

## Sync-Regel

- Quelle der Wahrheit: `ESP32.esp-hub/esp-hub-base/src/core/`
- Abgeleitete Repos: Kopie oder `lib_extra_dirs` / `-I` auf die Base (kein stilles Divergieren)
- Header-Kommentar „Base repository…“ beibehalten

## Acceptance pro Firmware

### esp32.rfmonitor 1.10.5 (Labor 2026-10-27)

- [x] Hub-Register + Rename wie Base (reply.name → NVS)
- [x] Web-OTA (`.87`/`.88` → 1.10.5); Hub-OTA-Pfad unverändert
- [x] `hwSummary` / Flash / CPU in Status + UI
- [x] gzip Web-UI (`web/index.html` → `UiPagesGz.h`, `Content-Encoding: gzip`)
- [ ] Recovery/ProgressWatch parallel (noch nicht vendored — Stufe A nur HW/CPU; bewusst später)
- [x] Bins `rfmonitor.1.10.5.esp32.bin` / `.esp32s3.bin` im Hub-`firmware/`

### esp32.network 1.7.0 (2026-10-27)

- [x] Hub-Register + Rename (reply.name, trim) — Bestand beibehalten
- [x] Web- + Hub-OTA-Pfade unverändert; UI gzip
- [x] `hwSummary` / Flash / CPU in `/api/status` + Heartbeat
- [x] `web/index.html` + `web/ota.html` → `src/web/UiPagesGz.h`
- [x] Labor-Flash/Smoke `.87` (gzip, Status-Keys, hwSummary/CPU, N4)
- [x] Bins `network.1.7.0.esp32.bin` / `.esp32s3.bin`

### esp32.io-control 1.7.0 (2026-10-27)

- [x] Hub-Register + Rename (trim)
- [x] Web-/Hub-OTA-Pfade; UI gzip (`web/index.html`+`ota.html`)
- [x] `hwSummary` / CPU / Flash in Status + Heartbeat
- [x] Bins `io-control.1.7.0.esp32.bin` / `.esp32s3.bin`
- [ ] Labor-Flash (kein freies Lab-Board — nachholen)

### esp32.webradio 2.5.0 (2026-10-27)

- [x] Hub-Register + Rename neu (reply.name → NVS)
- [x] Web-/Hub-OTA; UI von B64-PROGMEM → `web/*.html` gzip
- [x] `hwSummary` / CPU / Flash in Status + Heartbeat
- [x] Bins `webradio.2.5.0.esp32.bin` / `.esp32s3.bin`
- [ ] Labor-Flash (nachholen)

### Allgemein (weitere Nodes)

- [ ] Hub-Register + Rename wie Base
- [ ] Web- + Hub-OTA
- [ ] `hwSummary` / Flash gesamt vs OTA-Slot verständlich
- [ ] CPU-Last sichtbar (wo sinnvoll)
- [ ] Recovery/ProgressWatch parallel, Legacy-WDT bewusst
- [ ] Bins `{name}.{ver}.esp32.bin` / `.esp32s3.bin` + Hub-Upload
