# M2 — Hardware Capabilities

**FW:** 1.9.1  
**ADRs:** D-017 (Hardware im Core), D-019 (keine PSRAM-Annahme)

## Ergebnis

`HardwareInfo` ist das **eine** Capability-Objekt in `AppCore`:

1. `probe()` einmal beim Boot — Chip, Rev, Cores, Flash-KB, PSRAM-Größe  
2. Live-Getter — `freeHeap` / `freePsram` / `freeSketchSpace`  
3. Dieselbe Instanz für Serial-Log, `/api/status`, Heartbeat (`HubClient` / `WebUi` via Zeiger)

## Additive Status-Felder (kompatibel)

Baseline-Keys unverändert. Neu optional in `/api/status`:

| Key | Bedeutung |
|-----|-----------|
| `chipRev` | Chip-Revision |
| `cores` | CPU-Kerne |
| `flashKb` | Flash-Größe in KB |
| `hasPsram` | `true`/`false` aus gemessenem `psramSize` |

Heartbeat bleibt schlank (keine neuen Pflichtfelder).

## Acceptance

- [x] Eine `HardwareInfo`-Instanz, kein paralleles Raten
- [x] PSRAM=0 weiterhin gültig / korrekt geloggt
- [x] Compile ESP32 + ESP32-S3
- [ ] Device-Checkliste (PSRAM-Anzeige N8R2)
