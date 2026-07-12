#pragma once

#include <string>
#include <cstdint>

namespace arduino {
namespace hal {

class IConsole {
public:
    virtual ~IConsole() = default;

    virtual void print(const std::string& str) = 0;
    virtual void write(uint8_t b) = 0;
    virtual void flush() = 0;

    // Optional input support interface
    virtual bool available() const = 0;
    virtual int read() = 0;
};

} // namespace hal
} // namespace arduino
