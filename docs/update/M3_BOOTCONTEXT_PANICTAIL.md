# M3 — BootContext / PanicTail

**FW:** 1.9.2  
**ADRs:** D-009 (PanicTail RTC), D-008 (kein Flash im Panic-Pfad), D-020 (Timing)

## Ergebnis

- `PanicTail` in RTC_NOINIT: Reset-Reason, Boot-Generation, Restart-Cause, 16 Breadcrumbs
- `BootContext::begin()` früh in `AppCore::begin()` → Serial `[BOOT] …`
- Geplante Restarts (`requestRestart`) setzen Cause **ohne** Flash-Write
- `GET /api/diag/boot` — Diag-Vorstufe (volle Tail-JSON)
- Additive Status-Keys: `resetReason`, `restartCause`, `bootGen`

## Restart causes

| Cause | Wann |
|-------|------|
| `wifi_portal_timeout` | WiFiManager autoConnect fail |
| `wifi_reset` | BOOT-Taste 3 s |
| `hub_ota` | OTA von Hub-URL |
| `web_ota` | Web-Upload |
| `watchdog` | Legacy Heartbeat-WDT |

## Breadcrumb-Codes (Auszug)

`boot`, `wifi_up`, `hub_ok`, `web_up`, `ota_start`, `wifi_lost`, `wdt_fire`, `restart`, `reset_button`

## Acceptance

- [x] Soft-Reset: Serial zeigt `prev_cause` + ggf. `panic_tail`
- [x] `/api/diag/boot` liefert JSON
- [x] Kein Flash-Write in `noteRestart` / Panic-Pfad
- [ ] Device-Check nach Soft-Reset / OTA
