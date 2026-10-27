# M8 — Recovery

**FW:** 1.9.7  
**ADRs:** D-011

## Ergebnis

`Recovery` ist der **einzige** Einstieg für geplante Restarts:

- Gründe (`wifi_portal_timeout`, `hub_ota`, `web_ota`, `legacy_watchdog`, `wifi_reset`)
- Level: `soft` / `degraded` / `hard`
- RTC-Historie (6 Events) + `hardCount` für Escalation-Hinweis
- Danach Delegation an `PanicTail::requestRestart` (Breadcrumb + Checkpoint-Clear)

## API / Sichtbarkeit

| Ort | Inhalt |
|-----|--------|
| `GET /api/diag/recovery` | Historie + hardCount + prevCause |
| `GET /api/diag/boot` | nested `recovery` |
| `/api/status` | `recoveryHard`, `recoveryHist`, `recoveryLast` |

## Call sites

| Trigger | Reason | Level |
|---------|--------|-------|
| BOOT 3 s Reset | `wifi_reset` | soft |
| Portal-Timeout | `wifi_portal_timeout` | degraded |
| Hub-OTA OK | `hub_ota` | soft |
| Web-OTA OK | `web_ota` | soft |
| Legacy WDT | `legacy_watchdog` | hard |

## Acceptance

- [x] Restarts über Recovery mit nachvollziehbarem Grund
- [x] Historie/Escalation im Diag-Kontext
- [ ] Device: WDT → `/api/diag/recovery` zeigt `legacy_watchdog`
