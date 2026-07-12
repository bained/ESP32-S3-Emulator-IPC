#include "Serial.h"
#include "../HAL/IConsole.h"
#include "../HAL/WindowsConsole.h"
#include <cstdarg>
#include <vector>

namespace arduino {

// Access the active HAL console instance.
// Defaults to WindowsConsole for terminal output on Windows.
static hal::IConsole* getActiveConsole() {
    static hal::WindowsConsole defaultConsole;
    static hal::IConsole* activeConsole = &defaultConsole;
    return activeConsole;
}

HardwareSerial::HardwareSerial() : m_initialized(false) {}

void HardwareSerial::begin(unsigned long baud) {
    m_initialized = true;
}

void HardwareSerial::end() {
    m_initialized = false;
}

size_t HardwareSerial::write(uint8_t b) {
    if (!m_initialized) return 0;
    getActiveConsole()->write(b);
    return 1;
}

size_t HardwareSerial::print(const String& s) {
    if (!m_initialized) return 0;
    getActiveConsole()->print(s);
    return s.length();
}

size_t HardwareSerial::print(const char* s) {
    if (!m_initialized || s == nullptr) return 0;
    std::string str(s);
    getActiveConsole()->print(str);
    return str.length();
}

size_t HardwareSerial::print(int val) {
    return print(std::to_string(val));
}

size_t HardwareSerial::print(double val, int decimals) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.*f", decimals, val);
    return print(buf);
}

size_t HardwareSerial::println(const String& s) {
    size_t n = print(s);
    n += print("\r\n");
    return n;
}

size_t HardwareSerial::println(const char* s) {
    size_t n = print(s);
    n += print("\r\n");
    return n;
}

size_t HardwareSerial::println(int val) {
    size_t n = print(val);
    n += print("\r\n");
    return n;
}

size_t HardwareSerial::println() {
    return print("\r\n");
}

size_t HardwareSerial::printf(const char* format, ...) {
    if (!m_initialized || format == nullptr) return 0;

    va_list args;
    va_start(args, format);

    // Calculate formatted string length
    va_list args_copy;
    va_copy(args_copy, args);
    int len = std::vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (len <= 0) {
        va_end(args);
        return 0;
    }

    std::vector<char> buf(len + 1);
    std::vsnprintf(buf.data(), buf.size(), format, args);
    va_end(args);

    std::string str(buf.data());
    getActiveConsole()->print(str);
    return str.length();
}

int HardwareSerial::available() {
    if (!m_initialized) return 0;
    return getActiveConsole()->available() ? 1 : 0;
}

int HardwareSerial::read() {
    if (!m_initialized) return -1;
    return getActiveConsole()->read();
}

void HardwareSerial::flush() {
    if (!m_initialized) return;
    getActiveConsole()->flush();
}

} // namespace arduino

// Global instance of the HardwareSerial class
arduino::HardwareSerial Serial;
