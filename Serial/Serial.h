#pragma once

#include "../Core/Core.h"
#include <string>

namespace arduino {

class HardwareSerial {
public:
    HardwareSerial();

    void begin(unsigned long baud);
    void end();

    size_t write(uint8_t b);
    size_t print(const String& s);
    size_t print(const char* s);
    size_t print(int val);
    size_t print(double val, int decimals = 2);

    size_t println(const String& s);
    size_t println(const char* s);
    size_t println(int val);
    size_t println();

    size_t printf(const char* format, ...);

    int available();
    int read();
    void flush();

    operator bool() const { return m_initialized; }

private:
    bool m_initialized;
};

} // namespace arduino

// Global Serial Object
extern arduino::HardwareSerial Serial;
