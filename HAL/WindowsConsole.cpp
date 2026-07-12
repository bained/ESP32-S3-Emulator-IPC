#include "WindowsConsole.h"
#include <iostream>

namespace arduino {
namespace hal {

void WindowsConsole::print(const std::string& str) {
    std::cout << str;
}

void WindowsConsole::write(uint8_t b) {
    std::cout.put(static_cast<char>(b));
}

void WindowsConsole::flush() {
    std::cout.flush();
}

bool WindowsConsole::available() const {
    return false; // Minimal/dummy logic for now
}

int WindowsConsole::read() {
    return -1; // Minimal/dummy logic for now
}

} // namespace hal
} // namespace arduino
