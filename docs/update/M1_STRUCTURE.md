# M1 — Strukturmigration (Notiz)

**FW:** 1.9.0  
**Verträge:** unverändert gegenüber Baseline 1.8.0 ([M0_BASELINE_FREEZE.md](M0_BASELINE_FREEZE.md))

## Neue Layout

```text
ESP32.esp-hub/
├── esp-hub-base/                 ← Arduino-Sketch-Ordner
│   ├── esp-hub-base.ino          ← IO-Tabelle, updateIoValues(), setup/loop
│   ├── ProjectConfig.h           ← Name/Hub/Timeouts (alle TUs)
│   └── src/core/
│       ├── AppCore.*             ← begin()/loop() Lifecycle
│       ├── HardwareInfo.*
│       ├── NetworkWifi.*
│       ├── HubClient.*
│       ├── WebUi.*
│       ├── OtaService.*
│       ├── IoTypes.h
│       └── EspHubConfig.h        ← include ProjectConfig.h
├── docs/update/                  ← M0+ Roadmap/ADR/Baseline
└── esp-hub-base.1.8.0.*.bin      ← letzte vorkompilierte Baseline-Bins
```

## Build

```bash
arduino-cli compile --fqbn 'esp32:esp32:esp32:PartitionScheme=min_spiffs' esp-hub-base
# N8R2:
arduino-cli compile --fqbn 'esp32:esp32:esp32s3:PSRAM=enabled,FlashSize=8M,PartitionScheme=min_spiffs' esp-hub-base
```

**Hinweis:** Der Hub-„Sketch-Upload“ einer einzelnen `.ino` reicht ab M1 nicht mehr — der gesamte Ordner `esp-hub-base/` muss gebaut werden (arduino-cli lokal, PIO-Pfad im Hub, oder Git-Checkout).

## Bewusst unverändert (Baseline)

- Heartbeat-URL und Pflichtfelder
- Web-Routen `/`, `/events`, `/api/status`, `/ota`, `/ota-upload`
- NVS `esphub` / Keys `name`, `hub_host`, `hub_port`
- Portal `ESP-Hub-Setup`, BOOT-Reset 3 s
- Alter `WATCHDOG_TIMEOUT_S`-Pfad
- Blocking-Verhalten von OTA / WiFi-Reconnect (bekannt, später M9/M11)
