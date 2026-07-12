#pragma once

#include "../HAL/IExternalSensor.h"

namespace arduino {

class PIRSensor : public hal::IExternalSensor {
private:
    hal::IExternalSensor* m_parentPipe;
    bool m_motionDetected;

public:
    PIRSensor(hal::IExternalSensor* parentPipe);

    // IExternalSensor API implementations
    bool isDataAvailable() override;
    uint32_t readValue() override;
    void sendCommand(const std::string& cmd) override;

    bool isMotionDetected() const { return m_motionDetected; }
};

} // namespace arduino
