# M7 — ProgressWatch (parallel)

**FW:** 1.9.6  
**ADRs:** D-010 (ProgressWatch ersetzt WDT erst langfristig)

## Verhalten

`ProgressWatch` beobachtet Fortschritt auf Kanälen:

| Kanal | Signal |
|-------|--------|
| `idle` | jeder `loop()`-Durchlauf |
| `heartbeat` | erfolgreicher Hub-Register |
| `ota` | OTA-Start / OTA-Ende (nur wenn OTA lief) |

Bei Stall: Log `[PROG] STALL … (legacy WDT still armed)` — **kein** eigener Restart.

Der alte `lastSuccess`-Watchdog (`WATCHDOG_TIMEOUT_S`) bleibt unverändert aktiv und loggt zusätzlich den ProgressWatch-Zustand beim Feuern.

## Config

- `PROGRESS_TIMEOUT_S` (Default = `WATCHDOG_TIMEOUT_S`)

## Status (additiv)

`progIdleAge`, `progHbAge`, `progOtaAge`, `progStalled.{idle,heartbeat,ota}`

## Nebenfix in 1.9.6

`DiagSaved` kürzt nicht mehr blind per `substring` — bei Budget-Überschreitung wird ein **gültiges** compact/minimal JSON gebaut (`truncated:true`).

## Acceptance

- [x] ProgressWatch parallel, Legacy-WDT bleibt
- [x] Signale Idle/Heartbeat/OTA
- [x] DiagSaved liefert immer gültiges JSON
- [ ] Device: Hub-Ausfall → PROG STALL + später WDT
