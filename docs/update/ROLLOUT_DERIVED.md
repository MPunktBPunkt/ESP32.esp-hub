# Rollout — Abgeleitete Firmwares auf esp-hub-base

**Stand:** Base **1.10.5**, Migration M0–M11 + gzip Web-UI fertig.  
**Ziel:** Produkt-Firmwares nutzen denselben Core (Hub/OTA/Recovery/HW/CPU/Rename), Projeklogik als Module oder klare App-Schicht.

## Reihenfolge

1. **esp32.rfmonitor** — **1.10.5** Stufe A/B validiert: `src/esphub/` (HardwareInfo, CpuLoad), Hub-Rename, `web/index.html` → gzip-PROGMEM, UI hwSummary/CPU/Flash — Labor `.87`/`.88`
2. **esp32.network** — Arduino-Monolith, nah an Base-Verträgen ← als Nächstes
3. Weitere (io-control, webradio, communicator, …) nach gleichem Muster

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

### Allgemein (weitere Nodes)

- [ ] Hub-Register + Rename wie Base
- [ ] Web- + Hub-OTA
- [ ] `hwSummary` / Flash gesamt vs OTA-Slot verständlich
- [ ] CPU-Last sichtbar (wo sinnvoll)
- [ ] Recovery/ProgressWatch parallel, Legacy-WDT bewusst
- [ ] Bins `{name}.{ver}.esp32.bin` / `.esp32s3.bin` + Hub-Upload
