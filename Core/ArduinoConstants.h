#pragma once

#include <cstdint>

// Pin Levels
constexpr uint8_t HIGH = 0x1;
constexpr uint8_t LOW  = 0x0;

// Pin Modes
constexpr uint8_t INPUT           = 0x0;
constexpr uint8_t OUTPUT          = 0x1;
constexpr uint8_t INPUT_PULLUP    = 0x2;
constexpr uint8_t INPUT_PULLDOWN  = 0x3;

// Constants
constexpr double PI          = 3.14159265358979323846;
constexpr double HALF_PI     = 1.57079632679489661923;
constexpr double TWO_PI      = 6.28318530717958647692;
constexpr double DEG_TO_RAD  = 0.017453292519943295769236907684886;
constexpr double RAD_TO_DEG  = 57.295779513082320876798154814105;
constexpr double EULER       = 2.718281828459045235360287471352;

// Built-in LED Pin for ESP32-S3 (typically pin 2 on many dev boards, though varies)
constexpr uint8_t LED_BUILTIN = 2;
