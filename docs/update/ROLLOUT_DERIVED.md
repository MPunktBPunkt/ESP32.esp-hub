# Rollout — Abgeleitete Firmwares auf esp-hub-base

**Stand:** Base **1.10.5**, Migration M0–M11 + gzip Web-UI fertig.  
**A/B-Freeze:** [AUDIT_BASE_A-B_2026-10-27.md](AUDIT_BASE_A-B_2026-10-27.md) — Verträge, Drift, Labor-Matrix.  
**Ziel:** Produkt-Firmwares nutzen denselben Core (Hub/OTA/Recovery/HW/CPU/Rename), Projeklogik als Module oder klare App-Schicht.

## Reihenfolge

1. **esp32.rfmonitor** — **1.10.5** Stufe A/B validiert: `src/esphub/` (HardwareInfo, CpuLoad), Hub-Rename, `web/index.html` → gzip-PROGMEM — Labor `.87`/`.88`
2. **esp32.network** — **1.7.0** Stufe A/B validiert: Arduino-Monolith + `src/esphub/`, `web/*.html` → gzip — Labor `.87` Smoke OK (`.88` bleibt rfmonitor)
3. **esp32.io-control** — **1.7.0** Stufe A/B: Labor `.87` Smoke OK
4. **esp32.webradio** — **2.5.0** Stufe A/B: Labor `.88` Smoke OK
5. **esp32.communicator** — **1.9.0** Stufe A/B: Labor `.87` Smoke OK (danach Network wiederhergestellt)
6. **esp32.MeterMaster** — **0.5.0** Stufe A/B: Labor `.131` (OLED) Smoke OK
7. **esp32.heartrate** — **0.3.3** Stufe A/B: Labor `.87` Smoke OK
8. **esp32.ftmsprobe** — **0.1.5** Stufe A/B: Labor `.87` Smoke OK
9. **esp32.ergo** — **0.3.26-dev** Stufe A/B: Labor `.88` Smoke OK (gzip + PSRAM/`HwInfo` schon da; +CpuLoad)
10. Weitere (pidrive/hu-probe, …) projektspezifisch

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
- [x] Labor-Flash/Smoke `.87` (gzip, hwSummary/CPU)

### esp32.webradio 2.5.0 (2026-10-27)

- [x] Hub-Register + Rename neu (reply.name → NVS)
- [x] Web-/Hub-OTA; UI von B64-PROGMEM → `web/*.html` gzip
- [x] `hwSummary` / CPU / Flash in Status + Heartbeat
- [x] Bins `webradio.2.5.0.esp32.bin` / `.esp32s3.bin`
- [x] Labor-Flash/Smoke `.88` (gzip, hwSummary/CPU)

### esp32.communicator 1.9.0 (2026-10-27)

- [x] Hub-Register + Rename (trim)
- [x] Web-/Hub-OTA; UI `web/index|login|ota.html` → gzip
- [x] `hwSummary` / CPU / Flash in Status + Heartbeat
- [x] Bins `communicator.1.9.0.esp32.bin` / `.esp32s3.bin`
- [x] Labor-Flash/Smoke `.87` (gzip, hwSummary/N4/CPU; danach Network 1.7.0 restored)

### esp32.MeterMaster 0.5.0 (Labor 2026-10-27)

- [x] Hub-Register + Rename (reply.name → NVS `nname`)
- [x] Web-OTA `/update`; OLED-Pfad unverändert
- [x] `hwSummary` / Flash / CPU in `/api/sysinfo` + Heartbeat
- [x] `web/index.html` → `src/web/UiPagesGz.h` (gzip PROGMEM)
- [x] Labor-Flash/Smoke `.131` (gzip, Status/Meter, hwSummary/N4/CPU, registerOk/hubOk)
- [x] Bin `metermaster.0.5.0.esp32.bin` (+ Hub-`firmware/`)

### esp32.heartrate 0.3.3 (Labor 2026-10-27)

- [x] Hub-Register + Rename neu (reply.name → NVS)
- [x] Web-OTA; UI `web/index.html` → gzip-PROGMEM
- [x] `hwSummary` / CPU / Flash in Status + Heartbeat
- [x] Bins `heartrate.0.3.3.esp32.bin` / `.esp32s3.bin`
- [x] Labor-Flash/Smoke `.87` (gzip, hwSummary/N4/CPU; Network restored)

### esp32.ftmsprobe 0.1.5 (Labor 2026-10-27)

- [x] Hub-Register + Rename neu (reply.name → NVS)
- [x] Web-OTA; UI `web/index.html` → gzip-PROGMEM
- [x] `hwSummary` / CPU / Flash in Status + Heartbeat
- [x] Bins `ftmsprobe.0.1.5.esp32.bin` / `.esp32s3.bin`
- [x] Labor-Flash/Smoke `.87` (gzip, hwSummary/CPU; Network restored)

### esp32.ergo 0.3.26-dev (Labor 2026-10-27)

- [x] Hub-Register + Rename neu (reply.name → NVS)
- [x] Web-OTA; gzip-UI bereits Familien-Vorbild
- [x] PSRAM/`HwInfo` (N8R2, aktiv) bereits ab 0.3.25; dazu `CpuLoad` + `cpu*`/`module`/`hwSummary`
- [x] Bin `ergo.0.3.26-dev.esp32s3.bin`
- [x] Labor-Flash/Smoke `.88` (PSRAM aktiv ~2 MiB, cpu/hwSummary/N8R2; Webradio restored)

### Allgemein (weitere Nodes)

- [ ] Hub-Register + Rename wie Base
- [ ] Web- + Hub-OTA
- [ ] `hwSummary` / Flash gesamt vs OTA-Slot verständlich
- [ ] CPU-Last sichtbar (wo sinnvoll)
- [ ] Recovery/ProgressWatch parallel, Legacy-WDT bewusst
- [ ] Bins `{name}.{ver}.esp32.bin` / `.esp32s3.bin` + Hub-Upload
