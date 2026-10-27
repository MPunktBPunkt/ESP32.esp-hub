/**
 * @file ExampleUptimeModule.cpp
 * @brief ExampleUptimeModule implementation.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include "ExampleUptimeModule.h"

void ExampleUptimeModule::begin() { loops_ = 0; }

void ExampleUptimeModule::loop() { loops_++; }

void ExampleUptimeModule::appendStatusJson(String& json) {
    json += ",\"exampleLoops\":";
    json += String(loops_);
    json += ",\"exampleUptimeMs\":";
    json += String((uint32_t)millis());
}

bool ExampleUptimeModule::selfTest(String& report) {
    report = loops_ > 0 ? "ok" : "no_loops_yet";
    return true;
}
