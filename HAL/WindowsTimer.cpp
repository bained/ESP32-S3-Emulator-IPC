#include "WindowsTimer.h"
#include <thread>

namespace arduino {
namespace hal {

WindowsTimer::WindowsTimer() {
    m_startTime = std::chrono::steady_clock::now();
}

uint64_t WindowsTimer::getMillis() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
}

uint64_t WindowsTimer::getMicros() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - m_startTime).count();
}

void WindowsTimer::sleepMillis(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void WindowsTimer::sleepMicros(uint32_t us) {
    // High-resolution sleeping for microseconds
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

} // namespace hal
} // namespace arduino
