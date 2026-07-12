#pragma once

#include <cstdint>

namespace arduino {
namespace hal {

class ITimer {
public:
    virtual ~ITimer() = default;

    // Retrieve time elapsed since emulator start
    virtual uint64_t getMillis() const = 0;
    virtual uint64_t getMicros() const = 0;

    // High-resolution sleeping functions
    virtual void sleepMillis(uint32_t ms) = 0;
    virtual void sleepMicros(uint32_t us) = 0;
};

} // namespace hal
} // namespace arduino
