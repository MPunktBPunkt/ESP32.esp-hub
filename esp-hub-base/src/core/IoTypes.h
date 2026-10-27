#pragma once

#include <Arduino.h>

// Project-editable IO row (defined in the sketch).
struct IoValue {
    String key;
    String type;
    float  value;
    String unit;
};

// Provided by esp-hub-base.ino (user section).
extern IoValue ioTable[];
extern const int IO_COUNT;
void updateIoValues();
