#include "GPIO.h"
#include "GPIOManager.h"
#include "VirtualPin.h"
#include "../HAL/IExternalSensor.h"

namespace arduino {

// ==========================================
// VirtualPin Implementation
// ==========================================

VirtualPin::VirtualPin()
    : m_pinNumber(0), m_mode(INPUT), m_digitalValue(LOW), m_analogValue(0), m_pwmDutyCycle(0.0), m_name("") {}

VirtualPin::VirtualPin(uint8_t pinNumber, const std::string& name)
    : m_pinNumber(pinNumber), m_mode(INPUT), m_digitalValue(LOW), m_analogValue(0), m_pwmDutyCycle(0.0), m_name(name) {
    if (m_name.empty()) {
        m_name = "GPIO" + std::to_string(pinNumber);
    }
}

// ==========================================
// GPIOManager Implementation
// ==========================================

GPIOManager::GPIOManager() {
    m_pins.reserve(NUM_PINS);
    for (uint8_t i = 0; i < NUM_PINS; ++i) {
        m_pins.emplace_back(i);
        m_sensorMap[i] = nullptr;
    }
}

GPIOManager& GPIOManager::getInstance() {
    static GPIOManager instance;
    return instance;
}

void GPIOManager::pinMode(uint8_t pin, uint8_t mode) {
    if (isValidPin(pin)) {
        m_pins[pin].setMode(mode);
    }
}

void GPIOManager::digitalWrite(uint8_t pin, uint8_t val) {
    if (isValidPin(pin)) {
        m_pins[pin].setDigitalValue(val);
        if (m_sensorMap[pin] != nullptr) {
            std::string cmd = (val == HIGH) ? "SET_HIGH" : "SET_LOW";
            m_sensorMap[pin]->sendCommand(cmd);
        }
    }
}

int GPIOManager::digitalRead(uint8_t pin) {
    if (isValidPin(pin)) {
        if (m_sensorMap[pin] != nullptr) {
            uint32_t val = m_sensorMap[pin]->readValue();
            uint8_t dVal = (val > 0) ? HIGH : LOW;
            m_pins[pin].setDigitalValue(dVal);
            return dVal;
        }
        return m_pins[pin].getDigitalValue();
    }
    return LOW;
}

int GPIOManager::analogRead(uint8_t pin) {
    if (isValidPin(pin)) {
        if (m_sensorMap[pin] != nullptr) {
            uint32_t val = m_sensorMap[pin]->readValue();
            m_pins[pin].setAnalogValue(static_cast<int>(val));
            return static_cast<int>(val);
        }
        return m_pins[pin].getAnalogValue();
    }
    return 0;
}

void GPIOManager::analogWrite(uint8_t pin, int val) {
    if (isValidPin(pin)) {
        m_pins[pin].setAnalogValue(val);
        m_pins[pin].setPwmDutyCycle(static_cast<double>(val));
    }
}

void GPIOManager::registerExternalSensor(uint8_t pin, hal::IExternalSensor* sensor) {
    if (isValidPin(pin)) {
        m_sensorMap[pin] = sensor;
    }
}

VirtualPin& GPIOManager::getPin(uint8_t pin) {
    if (isValidPin(pin)) {
        return m_pins[pin];
    }
    static VirtualPin dummyPin;
    return dummyPin;
}

const VirtualPin& GPIOManager::getPin(uint8_t pin) const {
    if (isValidPin(pin)) {
        return m_pins[pin];
    }
    static const VirtualPin dummyPin;
    return dummyPin;
}

} // namespace arduino

// ==========================================
// Arduino Standard API Core Implementation
// ==========================================

void pinMode(uint8_t pin, uint8_t mode) {
    arduino::GPIOManager::getInstance().pinMode(pin, mode);
}

void digitalWrite(uint8_t pin, uint8_t val) {
    arduino::GPIOManager::getInstance().digitalWrite(pin, val);
}

int digitalRead(uint8_t pin) {
    return arduino::GPIOManager::getInstance().digitalRead(pin);
}

int _analogRead_temp(uint8_t pin) {
    return arduino::GPIOManager::getInstance().analogRead(pin);
}

// Resolve standard function mapping
#undef analogRead
int analogRead(uint8_t pin) {
    return _analogRead_temp(pin);
}

void analogWrite(uint8_t pin, int val) {
    arduino::GPIOManager::getInstance().analogWrite(pin, val);
}
