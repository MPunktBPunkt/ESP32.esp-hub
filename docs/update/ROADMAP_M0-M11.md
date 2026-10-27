# Migrations-Roadmap M0–M11

Strategie: **verhaltenskonservierend, inkrementell, überprüfbar.**  
Nach jedem Milestone: [VERIFICATION_CHECKLIST.md](VERIFICATION_CHECKLIST.md) gegen [M0_BASELINE_FREEZE.md](M0_BASELINE_FREEZE.md).

```text
M0  Contract + Baseline ins Repo          ✓
M1  Build-/Strukturmigration              ✓ (Compile ESP32/S3; Device-Checkliste offen)
M2  Hardware Capabilities                 ✓ (1.9.1; Device-Checkliste offen)
M3  BootContext / PanicTail               ✓ (1.9.2; Device-Checkliste offen)
M4  Checkpoint                            ← als Nächstes
M5  Status / Diagnostics API
M6  Breadcrumb + DiagLog
M7  ProgressWatch parallel zum alten WDT
M8  Recovery
M9  OTA Lifecycle
M10 BaseModule / Application Contract
M11 Cleanup / Removal of legacy paths
```

---

## M0 — Contract + Baseline Freeze

**Ziel:** Ist-Verträge und ADRs im Repo, messbar für Regressionen.

**Deliverables:**

- `docs/update/M0_BASELINE_FREEZE.md`
- `docs/update/ADR_D001-D020.md`
- `docs/update/ROADMAP_M0-M11.md`
- `docs/update/VERIFICATION_CHECKLIST.md`
- `docs/update/README.md`

**Acceptance:** Dokumente decken Heartbeat, OTA, Web, NVS, Reset, Timing-KNOWN-ISSUEs und D-001…D-020 ab. Kein Code-Umbau erforderlich.

**Status:** erledigt mit diesem Dokumentensatz.

---

## M1 — Build-/Strukturmigration

**Ziel:** Code in klare Schichten zerlegen, ohne äußeres Verhalten zu ändern.

**Deliverables (Richtung):**

- `AppCore`-ähnlicher Einstieg (`begin`/`loop`)
- Core-Ordner/Dateien für Network, Hub, Web, OTA, Config, Hardware
- Sketch bleibt Einstieg; konfigurierbare Abschnitte bleiben nutzerfreundlich

**Ist (in Arbeit / Repo):**

- Sketch-Ordner `esp-hub-base/` (Arduino-Namenskonvention)
- `ProjectConfig.h` für shared Defines über alle TUs
- `src/core/`: `AppCore`, `HardwareInfo`, `NetworkWifi`, `HubClient`, `WebUi`, `OtaService`, `IoTypes`
- Nutzer: IO-Tabelle + `updateIoValues()` weiterhin in der `.ino`
- FW **1.9.0** (Struktur-Minor; Verträge = Baseline 1.8.0)
- Hinweis: Hub-„nur .ino Upload“ reicht nicht mehr — gesamter Sketch-Tree nötig

**Nicht in M1:** neue Diag-Features, PlatformIO-Zwang (D-018), Watchdog-Entfernung.

**Acceptance:** Verification-Checkliste grün; FW-Version kann Patch/Minor tragen; Verhalten = Baseline.

---

## M2 — Hardware Capabilities

**Ziel:** Explizites Capability-Objekt (Chip, Flash, PSRAM, …) im Core (D-017, D-019).

**Ist:** siehe [M2_HARDWARE_CAPABILITIES.md](M2_HARDWARE_CAPABILITIES.md) — `HardwareInfo::probe()` + shared pointer; additive Status-Keys.

**Acceptance:** Status/Heartbeat/HW-Log beziehen sich auf dieselbe Quelle; PSRAM=0 weiterhin korrekt.

---

## M3 — BootContext / PanicTail

**Ziel:** Reset-Reason, Boot-Count/Kontext; RTC-residenter PanicTail (D-009).

**Ist:** siehe [M3_BOOTCONTEXT_PANICTAIL.md](M3_BOOTCONTEXT_PANICTAIL.md) — RTC-Tail, `/api/diag/boot`, additive Status-Keys.

**Acceptance:** Nach Soft-Reset sichtbar (Serial und/oder Diag-API-Vorstufe); kein Flash-Write im Panic-Pfad.

---

## M4 — Checkpoint

**Ziel:** Checkpoint-API (enter/leave/fail), last-open nach Reset (D-006).

**Acceptance:** Mindestens Core-Phasen (z. B. wifi, hub, web) setzen Checkpoints; Boot meldet letzten offenen Checkpoint.

---

## M5 — Status / Diagnostics API

**Ziel:** `/api/status` unverändert in Pflichtfeldern; neues `/api/diag/saved` (D-012, D-013).

**Acceptance:** Baseline-Status-Keys erhalten; Diag-Endpoint liefert Boot/Checkpoint-Snapshot.

---

## M6 — Breadcrumb + DiagLog

**Ziel:** Breadcrumb-Trail + DiagLog mit Ebenen RAM / RTC Tail / Flash Snapshot (D-007, D-008).

**Acceptance:** Flash-Writes nur an Safe Points; Loop-Timing nicht messbar verschlechtert (D-020).

---

## M7 — ProgressWatch parallel

**Ziel:** ProgressWatch eingeführt; **alter Watchdog bleibt aktiv** (D-010).

**Acceptance:** Progress-Signale für Heartbeat/OTA/Idle; beide Mechanismen loggen; kein Entfernen von `WATCHDOG_TIMEOUT_S`-Logik.

---

## M8 — Recovery

**Ziel:** Explizite Recovery-Schicht (Gründe, Escalation, Historie) (D-011).

**Acceptance:** Restarts haben nachvollziehbaren Recovery-Grund im Diag-Kontext.

---

## M9 — OTA Lifecycle

**Ziel:** OTA als Operation mit States/Progress statt rein blockierendem One-Shot (D-015).

**Acceptance:** Hub- und Web-OTA funktionieren; Progress beobachtbar; Wechselwirkung mit Watchdog/ProgressWatch definiert.

---

## M10 — BaseModule / Application Contract

**Ziel:** Module-API für Projektlogik (D-016); IO/Status-Hooks über Contract.

**Acceptance:** Base-Firmware ohne Extra-Module = Baseline-Verhalten; Beispiel-Modul dokumentiert.

---

## M11 — Cleanup / Legacy-Removal

**Ziel:** Legacy-Pfade entfernen, die durch M1–M10 ersetzt sind — **inkl.** Entscheidung zum alten Zeit-Watchdog nur wenn ProgressWatch bewährt.

**Acceptance:** Keine toten Globals; D-010 erfüllt oder bewusst als ADR-Update verschoben; Verification + manuelle Hub-Regression grün.

---

## Querschnittliche Verbote bis zur jeweiligen Milestone

| Thema | Frühestens |
|-------|------------|
| Alter WDT entfernen | M11 (nach M7-Bewährung) |
| Heartbeat mit Diag-Dump füllen | nie (D-014); additive Mini-Felder erst nach Hub-Abstimmung |
| Separates Diag-Repo | nie (D-001) |
| PlatformIO als Pflicht | erst nach ADR-Update zu D-018 |
