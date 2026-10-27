# Rollout — Abgeleitete Firmwares auf esp-hub-base

**Stand:** Base **1.10.4**, Migration M0–M11 fertig.  
**Ziel:** Produkt-Firmwares nutzen denselben Core (Hub/OTA/Recovery/HW/CPU/Rename), Projeklogik als Module oder klare App-Schicht.

## Reihenfolge

1. **esp32.rfmonitor** — **1.10.3** Stufe A/B: `src/esphub/` (HardwareInfo, CpuLoad), Hub-Rename, UI hwSummary/CPU/Flash — Labor `.87`/`.88`
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

- [ ] Hub-Register + Rename wie Base
- [ ] Web- + Hub-OTA
- [ ] `hwSummary` / Flash gesamt vs OTA-Slot verständlich
- [ ] CPU-Last sichtbar (wo sinnvoll)
- [ ] Recovery/ProgressWatch parallel, Legacy-WDT bewusst
- [ ] Bins `{name}.{ver}.esp32.bin` / `.esp32s3.bin` + Hub-Upload
