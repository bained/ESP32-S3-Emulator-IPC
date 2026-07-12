#pragma once

#include <string>
#include <cstdint>

namespace arduino {
namespace hal {

class IExternalSensor {
public:
    virtual ~IExternalSensor() = default;

    // Non-blocking check to see if new sensor data is available
    virtual bool isDataAvailable() = 0;

    // Fetch the current value from the external sensor
    virtual uint32_t readValue() = 0;

    // Send a command or instruction back to the sensor/actor
    virtual void sendCommand(const std::string& cmd) = 0;
};

} // namespace hal
} // namespace arduino
