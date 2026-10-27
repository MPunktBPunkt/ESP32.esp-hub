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
| [M3_BOOTCONTEXT_PANICTAIL.md](M3_BOOTCONTEXT_PANICTAIL.md) | BootContext, PanicTail, `/api/diag/boot` |
| [M4_CHECKPOINT.md](M4_CHECKPOINT.md) | Checkpoint enter/leave/fail, last-open |
| [M5_STATUS_DIAG_API.md](M5_STATUS_DIAG_API.md) | Status vs. `/api/diag/saved`, Doxygen |
| [M6_DIAGLOG.md](M6_DIAGLOG.md) | DiagLog RAM/RTC/NVS, `/api/diag/log` |
| [M7_PROGRESSWATCH.md](M7_PROGRESSWATCH.md) | ProgressWatch parallel zum Legacy-WDT |
| [M8_RECOVERY.md](M8_RECOVERY.md) | Recovery-Schicht, `/api/diag/recovery` |
| [M9_OTA_LIFECYCLE.md](M9_OTA_LIFECYCLE.md) | OTA States/Progress (Hub + Web) |
| [M10_BASEMODULE.md](M10_BASEMODULE.md) | BaseModule / ModuleManager Contract |
| [M11_CLEANUP.md](M11_CLEANUP.md) | Cleanup; Legacy-WDT bewusst behalten |
| [ROLLOUT_DERIVED.md](ROLLOUT_DERIVED.md) | Abgeleitete Firmwares auf den Core bringen |
| [WEBUI_GZ.md](WEBUI_GZ.md) | Web-UI: `web/index.html` → gzip-PROGMEM (Ergo-Weg) |
| [VERIFICATION_CHECKLIST.md](VERIFICATION_CHECKLIST.md) | Feld-Regression (Stand 1.10.4 weitgehend PASS) |
| [AUDIT_BASE_A-B_2026-10-27.md](AUDIT_BASE_A-B_2026-10-27.md) | Vertrags-/Drift-Audit nach A/B-Rollout (Freeze) |

## Aktueller Stand

- **Baseline-Verträge:** eingefroren gegen **v1.8.0**
- **Code:** `esp-hub-base/` FW **v1.10.5** (M0–M11 + gzip Web-UI)
- **Phase:** Stufe A/B an Derived laborgeprüft — siehe [AUDIT_BASE_A-B_2026-10-27.md](AUDIT_BASE_A-B_2026-10-27.md); C/D (Recovery/Diag überall) bewusst später
- **Regel:** Bestehende Hub-/OTA-/Status-Verträge bleiben bis zur bewussten Erweiterung kompatibel

## Leitplanken (Kurz)

1. Kein separates Diagnostik-Repo — Core bleibt in diesem Repo.
2. Heartbeat bleibt schlank; Diagnose-Details gehören nicht in `POST /api/register`.
3. Flash/NVS für Diagnose nur an Safe Points.
4. Alter 300-s-Watchdog erst ersetzen, wenn ProgressWatch parallel bewährt ist (D-010: nach M11 deferred).
5. Diagnostics darf Timing/Blocking nicht verschärfen.
