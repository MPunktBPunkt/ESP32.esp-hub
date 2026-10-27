# M6 — Breadcrumb + DiagLog

**FW:** 1.9.5  
**ADRs:** D-007 (gestufte Persistenz), D-008 (Flash nur Safe Points), D-020

## Ebenen

| Ebene | Implementierung | Überlebt |
|-------|-----------------|----------|
| RAM | 4 KiB Ring (`malloc` / `ps_malloc`) | Soft-Reset: nein |
| RTC | 1 KiB `RTC_NOINIT` | Soft-Reset / Panic / WDT |
| Flash | NVS `ehlog` (2 Snapshot-Slots) | Power-On |

Breadcrumbs bleiben in `PanicTail` (RTC); neue Crumbs werden zusätzlich als `[BC] …` in DiagLog gespiegelt (Folds ohne Extra-Zeile).

## Safe Points für `persist()`

- Boot (`BootContext`, force)
- Nach HW-Probe (`hw`, rate-limited)
- Geplanter Restart (`PanicTail::noteRestart`, force)

## API

| Endpoint | Inhalt |
|----------|--------|
| `GET /api/diag/log` | `ram`, `prevTail`, `newestSnap`, Zähler |
| `GET /api/status` | additiv `diagRam`, `diagPrev`, `diagSnaps` |

## Acceptance

- [x] RAM + RTC + Flash-Snapshot-Ebenen
- [x] Flash/NVS nur über `persist()` an Safe Points
- [x] Breadcrumb-Spiegelung ohne Loop-Spam (Fold)
- [ ] Device: Soft-Reset → `prevTail` / Snapshots prüfen
