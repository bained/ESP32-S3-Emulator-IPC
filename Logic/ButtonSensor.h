#pragma once

#include "../HAL/IExternalSensor.h"
#include "../Core/Core.h"

namespace arduino {

class ButtonSensor : public hal::IExternalSensor {
private:
    hal::IExternalSensor* m_parentPipe;
    bool m_buttonPressed; // true = Pressed (LOW), false = Released (HIGH)

public:
    ButtonSensor(hal::IExternalSensor* parentPipe)
        : m_parentPipe(parentPipe), m_buttonPressed(false) {}

    // IExternalSensor API implementations
    bool isDataAvailable() override {
        return m_parentPipe ? m_parentPipe->isDataAvailable() : false;
    }

    uint32_t readValue() override {
        if (m_parentPipe) {
            // Read directly from the parent pipe.
            // If there's new data, it will be consumed and returned.
            // If there's no new data, it returns the cached last value, ensuring all sub-sensors see the broadcast.
            uint32_t val = m_parentPipe->readValue();
            
            // Protocol:
            // 0 = Button Pressed (LOW)
            // 1 = Button Released (HIGH)
            if (val == 0) {
                m_buttonPressed = true;
            } else if (val == 1) {
                m_buttonPressed = false;
            }
        }
        return m_buttonPressed ? LOW : HIGH;
    }

    void sendCommand(const std::string& cmd) override {
        if (m_parentPipe) {
            m_parentPipe->sendCommand(cmd);
        }
    }
};

} // namespace arduino
