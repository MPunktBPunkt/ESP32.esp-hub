#pragma once

/**
 * @file ExampleUptimeModule.h
 * @brief Optional demo BaseModule (enable with ESPHUB_ENABLE_EXAMPLE_MODULE).
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include "../core/BaseModule.h"

/**
 * @brief Example module that exposes loop-count / uptime extras in /api/status.
 *
 * Not registered by default — see README / M10 docs.
 */
class ExampleUptimeModule : public BaseModule {
public:
    const char* name() const override { return "example_uptime"; }
    void begin() override;
    void loop() override;
    void appendStatusJson(String& json) override;
    bool selfTest(String& report) override;

private:
    uint32_t loops_ = 0;
};
