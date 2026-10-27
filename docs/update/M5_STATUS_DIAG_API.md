# M5 — Status / Diagnostics API

**FW:** 1.9.4  
**ADRs:** D-012, D-013  
**Maintainability:** Doxygen headers + [KOMMENTARSTANDARD.md](../KOMMENTARSTANDARD.md)

## Endpoint-Vertrag

| Endpoint | Inhalt |
|----------|--------|
| `GET /api/status` | Live operativ — Baseline-Pflichtfelder unverändert; additive Keys erlaubt |
| `GET /api/diag/boot` | Live PanicTail + Checkpoint |
| `GET /api/diag/saved` | **Persistierter** Boot/Checkpoint-Snapshot (NVS `ehdiag`) |

## DiagSaved

- Einmaliger Persist am Boot-Safe-Point nach `HardwareInfo::probe()`
- Keine NVS-Writes im Loop
- Payload: Metadaten (`fw`, `bootGen`, `resetReason`, …) + eingebettetes `diag`-Objekt

## Doxygen

```bash
cd esp-hub-base && doxygen Doxyfile
# → build/docs/html/index.html
```

Jeder File-Header enthält den Pflicht-Verweis:

`https://github.com/MPunktBPunkt/ESP32.esp-hub`

## Acceptance

- [x] Baseline-Status-Keys erhalten
- [x] `/api/diag/saved` liefert Boot/Checkpoint-Snapshot
- [x] Doxygen-Header + Repo-Link in Core-Files
- [ ] Device: nach Soft-Reset saved vs. boot vergleichen
