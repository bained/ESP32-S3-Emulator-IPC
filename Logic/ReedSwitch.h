#pragma once

#include "../Core/Core.h"

namespace arduino {

namespace hal {
class IExternalSensor;
}

class ReedSwitch {
private:
    uint8_t m_pin;
    hal::IExternalSensor* m_sensor;

public:
    ReedSwitch(uint8_t pin, hal::IExternalSensor* sensor);

    // Returns true if the switch is 'closed' (magnet is near, reading is LOW)
    bool isClosed();

    // Returns true if the switch is 'open' (magnet is away, reading is HIGH)
    bool isOpen();

    uint8_t getPin() const { return m_pin; }
};

} // namespace arduino
