#pragma once

#include "VirtualPin.h"
#include <vector>

// Forward declaration of the HAL external sensor interface to keep dependencies clean
namespace arduino {
namespace hal {
class IExternalSensor;
}
}

namespace arduino {

class GPIOManager {
private:
    static constexpr int NUM_PINS = 45; // ESP32-S3 has 45 programmable GPIOs (0 to 44)
    std::vector<VirtualPin> m_pins;
    hal::IExternalSensor* m_sensorMap[NUM_PINS];

    // Private constructor for Singleton pattern
    GPIOManager();

public:
    static GPIOManager& getInstance();

    // Prevent copy/assignment
    GPIOManager(const GPIOManager&) = delete;
    GPIOManager& operator=(const GPIOManager&) = delete;

    // Standard Arduino API implementation interfaces
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t val);
    int digitalRead(uint8_t pin);
    int analogRead(uint8_t pin);
    void analogWrite(uint8_t pin, int val);

    // Register an external sensor/actuator IPC channel for a specific pin
    void registerExternalSensor(uint8_t pin, hal::IExternalSensor* sensor);

    // Emulator diagnostic/inspector methods
    VirtualPin& getPin(uint8_t pin);
    const VirtualPin& getPin(uint8_t pin) const;
    bool isValidPin(uint8_t pin) const { return pin < NUM_PINS; }
};

} // namespace arduino
