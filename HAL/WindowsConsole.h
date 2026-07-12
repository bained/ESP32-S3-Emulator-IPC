#pragma once

#include "IConsole.h"

namespace arduino {
namespace hal {

class WindowsConsole : public IConsole {
public:
    WindowsConsole() = default;
    ~WindowsConsole() override = default;

    void print(const std::string& str) override;
    void write(uint8_t b) override;
    void flush() override;

    bool available() const override;
    int read() override;
};

} // namespace hal
} // namespace arduino
