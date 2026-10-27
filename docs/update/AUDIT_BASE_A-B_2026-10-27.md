# Audit — Base 1.10.5 & Stufe A/B Freeze

**Datum:** 2026-10-27  
**Scope:** Vertrags- und Drift-Audit nach Mehrfach-Rollout (nicht Security-/RE-Deep-Dive).  
**Quelle Base:** `esp-hub-base/` FW **1.10.5** (`ProjectConfig.h`).

## Urteil

**Stufe A/B ist einsatzfähig und laborgeprüft.** Shared Core `HardwareInfo`/`CpuLoad` ist in acht Node-Repos byte-identisch zur Base; Live-Geräte liefern `hwSummary`/`module`/`cpu*`.  

**Base-only** (DiagLog, Recovery, ProgressWatch, Checkpoint, OtaService-Statusfelder) ist bewusst **nicht** in Derived vendored — das ist die Grenze vor Stufe C/D, kein Regress.

## 1. Base-Verträge (Inventar)

### Hub-Reply (Gerät liest nur diese Keys)

| Key | Wirkung |
|-----|---------|
| `interval` | Sekunden → Heartbeat-Periode (≥ 5 s) |
| `otaUrl` | Hub-OTA anstoßen |
| `name` | Rename → NVS |

### `/api/status` — Baseline

`name`, `version`, `chip`, `mac`, `ip`, `rssi`, `uptime`, `heap`, `psram`, `psramSize`, `sketch`, `hub`, `interval`, `ios`

### `/api/status` — Additive (Base vollständig)

- **HW/CPU:** `chipRev`, `cores`, `flashKb`, `hasPsram`, `heapMax`, `sketchUsed`, `module`, `flashRealMb`, `flashVendor`, `psramEfuseMb`, `psramBus`, `psramVendor`, `hwSummary`, `cpu0`/`cpu1`/`cpu`
- **Boot/Diag/Recovery/OTA:** `resetReason`, `restartCause`, `bootGen`, `ckpt*`, `diag*`, `prog*`, `recovery*`, `otaState`/`otaProgress`/…

### Heartbeat (schlank)

`mac`, `name`, `hwType`, `chipModel`, `version`, `ip`, `rssi`, `uptime`, `freeHeap`, `freeSketch`, `freePsram`, `psramSize`, `ios`  
(+ in A/B-Nodes oft zusätzlich `hwSummary`/`module`/`cpu` — erlaubt, bleibt schlank)

### Diag-APIs (Base-only)

`GET /api/diag/boot|saved|log|recovery` — Derived A/B haben diese Pfade **nicht**.

## 2. Drift: HardwareInfo / CpuLoad

| Projekt | HardwareInfo | CpuLoad | Hinweis |
|---------|--------------|---------|---------|
| rfmonitor, network, io-control, webradio, communicator, MeterMaster, heartrate, ftmsprobe | **identisch** Base | **identisch** Base | SHA gleich |
| ergo | eigenes `core/HwInfo` (+PSRAM aktiv) | **identisch** Base | `hwSummary` manuell; API-Oberfläche anders, Semantik N8R2 ok |

**Sync-Regel bleibt:** Wahrheit = `ESP32.esp-hub/esp-hub-base/src/core/`. Ergo-`HwInfo` ist bewusste Parallelspur (hu-probe); langfristig angleichen oder als ADR dokumentieren.

## 3. Derived-Matrix (Code + Labor)

| Projekt | interval | name | otaUrl | hwSummary | cpu | gzip UI | diag/recovery | Labor A/B |
|---------|----------|------|--------|-----------|-----|---------|---------------|-----------|
| esp-hub-base | Y | Y | Y | Y | Y | Y | Y | Verification ~1.10.4/5 |
| rfmonitor 1.10.5 | Y | Y | Y | Y | Y | Y | — | `.87`/`.88` |
| network 1.7.0 | Y | Y | Y | Y | Y | Y | — | `.87` live |
| io-control 1.7.0 | Y | Y | Y | Y | Y | Y | — | `.87` |
| webradio 2.5.0 | Y | Y | Y | Y | Y | Y | — | `.88` live |
| communicator 1.9.0 | Y | Y | Y | Y | Y | Y | — | `.87` |
| MeterMaster 0.5.0 | **N**¹ | Y | **N**¹ | Y | Y | Y | — | `.131` live |
| heartrate 0.3.3 | Y | Y | Y | Y | Y | Y | — | `.87` |
| ftmsprobe 0.1.5 | Y | Y | Y | Y | Y | Y | — | `.87` |
| ergo 0.3.26-dev | Y | Y | Y | Y | Y | Y | — | `.88` |

¹ MeterMaster: Hub-Pfad nur Heartbeat/Rename; OTA über `/update` (Web) und GitHub — **kein** Hub-`otaUrl`/`interval` am ESP-Hub-Client. MeterMaster-Adapter (8089) hat eigene Intervalle (Carousel/Fetch).

### Live-Stichprobe 2026-10-27

| Gerät | FW | A/B Keys (`hwSummary`/`module`/`cpu*`) | Base-only Keys |
|-------|-----|----------------------------------------|----------------|
| `.87` Network 1.7.0 | ok | vorhanden | keine |
| `.88` Webradio 2.5.0 | ok | vorhanden | keine |
| `.131` MeterMaster 0.5.0 | ok (`/api/sysinfo`) | vorhanden | keine |

## 4. Feldlücken (Verification)

| Item | Stand |
|------|--------|
| Hub `interval` → Gerät | **Teil-PASS:** Hub sendet `interval` aus Adapter-Config (Default **30**); `.87` meldet `interval: 30`. Expliziter Config-Wechsel (z. B. 15 s) nicht gegen Admin-API gefahren — Codepfad in Base + fast allen Derived vorhanden. |
| BOOT 3 s WLAN-Reset | **offen** — braucht physischen Knopfdruck; nicht remote prüfbar. |
| Hub-WDT-Ausfall / ioTable-Test | unverändert offen (lang/manuell). |

## 5. Empfehlungen (priorisiert)

1. **Freeze A/B** — dieses Dokument + ROLLOUT als Referenz; keine weiteren Pflichtfelder ohne Minor-Bump.  
2. **MeterMaster** — optional Hub-`interval`/`otaUrl` nachziehen, wenn Hub-OTA gewünscht; sonst ADR „OTA nur Web/GitHub“.  
3. **Ergo** — ADR oder Migration `HwInfo` → `HardwareInfo` (PSRAM-Semantik behalten).  
4. **Vendor-Sync** — bei Base-Patch an HI/CpuLoad: Kopien in Nodes aktualisieren (aktuell driftfrei außer Ergo-HW).  
5. **Stufe C/D** — erst wenn Diag/Recovery bewusst in Produkt-FWs sollen; nicht als stilles A/B-Nachziehen.  
6. **BOOT-3s** — einmal manuell an Base abhaken, dann Checkliste schließen.

## 6. Nicht-Ziele dieses Audits

- Vollständiger Security-/Memory-Audit  
- pidrive / hu-probe Produktlogik  
- AppCore-Zwang für Monolithen
