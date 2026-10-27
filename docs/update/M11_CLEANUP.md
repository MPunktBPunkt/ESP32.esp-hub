# M11 — Cleanup / Legacy-Removal

**FW:** 1.10.0

## Entfernt / aufgeräumt

- OTA nicht mehr als loses `otaPending`+rohe `performOta`-Funktion ohne State
- Restarts nur noch über `Recovery` (seit M8)
- Module-Hook statt Sketch-Hack für Erweiterungen

## Bewusste Nicht-Entfernung (D-010)

Der **Legacy-Heartbeat-Zeit-Watchdog** (`WATCHDOG_TIMEOUT_S` / `lastSuccess`) bleibt aktiv.

Begründung: ProgressWatch ist parallel eingeführt (M7), aber noch ohne ausreichende
Feld-Bewährung. Entfernung erst nach explizitem ADR-Update / Field-Evidence.

Während OTA (`ota.active()`) wird der Legacy-WDT pausiert, damit lange Downloads
nicht fälschlich als Heartbeat-Timeout gewertet werden.

## Acceptance

- [x] Keine toten OTA-Globals ohne Lifecycle
- [x] D-010: Legacy-WDT bewusst behalten (dokumentiert)
- [ ] Device-Verification laut Checkliste
