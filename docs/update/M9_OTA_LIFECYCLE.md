# M9 — OTA Lifecycle

**FW:** 1.10.0  
**ADRs:** D-015

## States

`idle` → `scheduled` → `fetching` → `writing` → `success` | `failed`

- Hub-OTA: `scheduleUrl` + `poll()` (Stream mit Progress + Web-Yield)
- Web-OTA: `webBegin` / `webWrite` / `webFinish`
- ProgressWatch erhält `Ota`-Notes während des Transfers
- Legacy-WDT greift **nicht**, solange `ota.active()` (lange Downloads)

## Status

`otaState`, `otaProgress`, `otaBytes`, `otaTotal`, `otaActive`, `otaError`
