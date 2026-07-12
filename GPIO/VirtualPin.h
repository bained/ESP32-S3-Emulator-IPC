#pragma once

#include "../Core/Core.h"
#include <string>

namespace arduino {

class VirtualPin {
private:
    uint8_t m_pinNumber;
    uint8_t m_mode;         // INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN
    uint8_t m_digitalValue; // LOW or HIGH
    int m_analogValue;      // 0 - 4095 (12-bit ADC on ESP32-S3)
    double m_pwmDutyCycle;  // PWM value / duty cycle
    std::string m_name;     // Name of the pin, e.g. "GPIO2"

public:
    VirtualPin();
    VirtualPin(uint8_t pinNumber, const std::string& name = "");

    // Getters and Setters
    uint8_t getPinNumber() const { return m_pinNumber; }
    
    uint8_t getMode() const { return m_mode; }
    void setMode(uint8_t mode) { m_mode = mode; }

    uint8_t getDigitalValue() const { return m_digitalValue; }
    void setDigitalValue(uint8_t value) { m_digitalValue = value; }

    int getAnalogValue() const { return m_analogValue; }
    void setAnalogValue(int value) { m_analogValue = value; }

    double getPwmDutyCycle() const { return m_pwmDutyCycle; }
    void setPwmDutyCycle(double duty) { m_pwmDutyCycle = duty; }

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
};

} // namespace arduino
