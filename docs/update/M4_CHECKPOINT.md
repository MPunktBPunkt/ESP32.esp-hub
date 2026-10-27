# M4 — Checkpoint

**FW:** 1.9.3  
**ADRs:** D-006 (Checkpoint), D-007/D-008 (RTC + NVS-Mirror an Safe Points)

## Ergebnis

`Checkpoint` (generalisiert aus hu-probe `PhaseGate`):

- API: `enter` / `leave` / `fail`
- RTC_NOINIT + CRC; NVS-Namespace `ehckpt` für Power-On-Recovery
- `last_open` nach Reset (Serial + Status + `/api/diag/boot`)
- Geplante Restarts (`PanicTail::noteRestart`) räumen den Open-Stack → kein False-Positive

## Core-Phasen

| Phase | Verwendung |
|-------|------------|
| `wifi` | um `NetworkWifi::begin` |
| `web` | um `WebUi::begin` |
| `hub` | um jeden Heartbeat |
| `ota` | um Hub-OTA-Download |

## Status / Diag (additiv)

- `/api/status`: `ckptOpen`, `ckptLastOpen`
- `/api/diag/boot`: Objekt `checkpoint` (open stack, lastOpen, lastLeave, lastFail)

## Acceptance

- [x] Core-Phasen wifi/web/hub setzen Checkpoints
- [x] Boot meldet letzten offenen Checkpoint (RTC/NVS)
- [x] Kein Flash außer NVS-Mirror an enter/leave (Safe Point)
- [ ] Device: Hang in Hub-HTTP → Soft-Reset → `ckptLastOpen.phase=hub`
