# M10 — BaseModule / Application Contract

**FW:** 1.10.0  
**ADRs:** D-016

## Contract

`BaseModule`: `name`, `begin`, `loop`, `appendStatusJson`, `appendMetrics`,
`handleCommand`, `registerRoutes`, `selfTest`

`ModuleManager` (max. 6): `add` vor `AppCore::begin()`, dann automatisches begin/loop/routes/status.

## Beispiel

```cpp
#define ESPHUB_ENABLE_EXAMPLE_MODULE 1
#include "src/modules/ExampleUptimeModule.h"
static ExampleUptimeModule mod;
void setup() {
  app.modules().add(&mod);
  app.begin();
}
```

Ohne Module = Baseline-Verhalten.
