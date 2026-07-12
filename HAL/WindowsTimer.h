#pragma once

#include "ITimer.h"
#include <chrono>

namespace arduino {
namespace hal {

class WindowsTimer : public ITimer {
private:
    std::chrono::steady_clock::time_point m_startTime;

public:
    WindowsTimer();

    uint64_t getMillis() const override;
    uint64_t getMicros() const override;

    void sleepMillis(uint32_t ms) override;
    void sleepMicros(uint32_t us) override;
};

} // namespace hal
} // namespace arduino
