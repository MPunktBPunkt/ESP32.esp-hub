# ESP32.esp-hub — Update / Migration

Dieses Verzeichnis hält den Umbau von `esp-hub-base` zur modularen Basisplattform.

| Dokument | Zweck |
|----------|--------|
| [2026-10-27_update.txt](2026-10-27_update.txt) | Gesamtbericht Schritte 1–8 (Evidenz & Herleitung) |
| [ADR_D001-D020.md](ADR_D001-D020.md) | Verbindliche Architecture Decisions |
| [M0_BASELINE_FREEZE.md](M0_BASELINE_FREEZE.md) | Eingefrorene Ist-Verträge (v1.8.0) |
| [ROADMAP_M0-M11.md](ROADMAP_M0-M11.md) | Migrationsmeilensteine inkl. Acceptance |
| [VERIFICATION_CHECKLIST.md](VERIFICATION_CHECKLIST.md) | Regression gegen Baseline |
| [M1_STRUCTURE.md](M1_STRUCTURE.md) | Sketch-Layout und Build nach Strukturmigration |
| [M2_HARDWARE_CAPABILITIES.md](M2_HARDWARE_CAPABILITIES.md) | Gemeinsames HardwareInfo / Capabilities |

## Aktueller Stand

- **Baseline-Verträge:** eingefroren gegen **v1.8.0**
- **Code:** `esp-hub-base/` FW **v1.9.1** (M0–M2; Verträge = Baseline 1.8.0 + additive Status-Keys)
- **Phase:** M2 fertig → als Nächstes **M3** (BootContext / PanicTail)
- **Regel:** Bestehende Hub-/OTA-/Status-Verträge bleiben bis zur bewussten Erweiterung kompatibel

## Leitplanken (Kurz)

1. Kein separates Diagnostik-Repo — Core bleibt in diesem Repo.
2. Heartbeat bleibt schlank; Diagnose-Details gehören nicht in `POST /api/register`.
3. Flash/NVS für Diagnose nur an Safe Points.
4. Alter 300-s-Watchdog erst ersetzen, wenn ProgressWatch parallel bewährt ist.
5. Diagnostics darf Timing/Blocking nicht verschärfen.
