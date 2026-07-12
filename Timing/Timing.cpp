#include "Timing.h"
#include "../HAL/ITimer.h"
#include "../HAL/WindowsTimer.h"

namespace arduino {
namespace timing {

// Access the active HAL timer instance.
// Defaults to a WindowsTimer instance for standard Windows execution.
static hal::ITimer* getActiveTimer() {
    static hal::WindowsTimer defaultTimer;
    static hal::ITimer* activeTimer = &defaultTimer;
    return activeTimer;
}

// Function to allow hot-swapping or registering a different HAL timer (e.g. for unit tests)
void setHALTimer(hal::ITimer* timer) {
    // Can be used to inject custom mock timers in future tests
}

} // namespace timing
} // namespace arduino

// ==========================================
// Arduino Standard Timing API Implementation
// ==========================================

unsigned long millis() {
    return static_cast<unsigned long>(arduino::timing::getActiveTimer()->getMillis());
}

unsigned long micros() {
    return static_cast<unsigned long>(arduino::timing::getActiveTimer()->getMicros());
}

void delay(unsigned long ms) {
    arduino::timing::getActiveTimer()->sleepMillis(static_cast<uint32_t>(ms));
}

void delayMicroseconds(unsigned int us) {
    arduino::timing::getActiveTimer()->sleepMicros(static_cast<uint32_t>(us));
}
