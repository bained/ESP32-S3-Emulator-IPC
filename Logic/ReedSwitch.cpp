#include "ReedSwitch.h"
#include "../GPIO/GPIO.h"
#include "../HAL/IExternalSensor.h"

namespace arduino {

ReedSwitch::ReedSwitch(uint8_t pin, hal::IExternalSensor* sensor)
    : m_pin(pin), m_sensor(sensor) {}

bool ReedSwitch::isClosed() {
    // Under INPUT_PULLUP: LOW (0) means magnet is near, closed circuit
    return digitalRead(m_pin) == LOW;
}

bool ReedSwitch::isOpen() {
    // Under INPUT_PULLUP: HIGH (1) means magnet is away, open circuit
    return digitalRead(m_pin) == HIGH;
}

} // namespace arduino
