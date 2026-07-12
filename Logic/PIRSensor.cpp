#include "PIRSensor.h"
#include "../Core/Core.h"

namespace arduino {

PIRSensor::PIRSensor(hal::IExternalSensor* parentPipe)
    : m_parentPipe(parentPipe), m_motionDetected(false) {}

bool PIRSensor::isDataAvailable() {
    if (m_parentPipe) {
        return m_parentPipe->isDataAvailable();
    }
    return false;
}

uint32_t PIRSensor::readValue() {
    if (m_parentPipe) {
        // Read directly from the parent pipe.
        // If there's new data, it will be consumed and returned.
        // If there's no new data, it returns the cached last value, ensuring all sub-sensors see the broadcast.
        uint32_t val = m_parentPipe->readValue();
        
        // Protocol: 
        // 2 = Motion Detected (HIGH)
        // 3 = No Motion (LOW)
        if (val == 2) {
            m_motionDetected = true;
        } else if (val == 3) {
            m_motionDetected = false;
        }
    }
    return m_motionDetected ? HIGH : LOW;
}

void PIRSensor::sendCommand(const std::string& cmd) {
    if (m_parentPipe) {
        m_parentPipe->sendCommand(cmd);
    }
}

} // namespace arduino
