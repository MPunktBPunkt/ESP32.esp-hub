# Kommentarstandard Firmware (Doxygen)

Gilt für alle `.h`/`.cpp` unter `esp-hub-base/src/` sowie `ProjectConfig.h` und die Sketch-`.ino`.
Kommentare sind **englisch**, Format **Doxygen (Javadoc-Stil)**.

Erzeugen: `cd esp-hub-base && doxygen Doxyfile` → `build/docs/html/index.html`.

## Datei-Kopf (Pflicht)

Direkt nach `#pragma once` (Header) bzw. am Dateianfang (`.cpp` / `.ino`):

```cpp
/**
 * @file Example.h
 * @brief One-sentence summary ending with a period.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */
```

Der **Base-repository-Link ist Pflicht** in jedem File-Header, damit abgeleitete/kopierte
Projekte die Herkunft der Basisplattform behalten.

## API-Trennung (Status vs. Diag)

| Endpoint | Rolle |
|----------|--------|
| `GET /api/status` | Operativer Live-Status (Baseline-Pflichtfelder + schlanke Additive) |
| `GET /api/diag/boot` | Live Boot/PanicTail/Checkpoint |
| `GET /api/diag/saved` | Persistierter Snapshot (NVS), getrennt von Status (D-013) |

## Kurzregeln

1. `@brief` Pflicht, ein Satz, endet mit Punkt.
2. `@param` / `@return` an öffentlichen API-Deklarationen im Header.
3. Keine doppelten Blockkommentare in `.cpp`, wenn der Header bereits dokumentiert.
4. Triviale Getter dürfen einzeilig sein: `/** @brief Free internal heap in bytes. */`
