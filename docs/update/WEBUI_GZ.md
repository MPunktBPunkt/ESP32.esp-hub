# Web-UI — gzip-PROGMEM (Ergo-Weg)

**Stand:** Base **1.10.5**

## Pfad der Wahrheit

```
web/index.html  →  tools/pack_ui.py  →  src/core/UiPagesGz.h  →  PROGMEM
```

- Eine HTML-Seite, clientseitige Tabs (Status / OTA).
- Auslieferung: `Content-Encoding: gzip` + `send_P`.
- Version/Name kommen aus `/api/status` bzw. SSE — keine String-Splices im C++.

## Workflow

1. UI in `esp-hub-base/web/index.html` bearbeiten.
2. `python3 tools/pack_ui.py` (Arduino-CLI hat keinen Pre-Hook).
3. Generierte `UiPagesGz.h` mit committen.
4. Sketch wie gewohnt bauen.

Abgeleitete Firmwares können denselben Packer/Ordner-Konventionen folgen (`web/` + gzip-Header), eigene Tabs in derselben HTML-Datei.
