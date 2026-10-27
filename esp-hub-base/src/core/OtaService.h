#pragma once

#include <Arduino.h>

class OtaService {
public:
    // Blocking HTTP download + flash (baseline; lifecycle in M9).
    void performFromUrl(const String& url);
};
