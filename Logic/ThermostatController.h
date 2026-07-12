#pragma once

#include "../Core/Core.h"

namespace arduino {

namespace hal {
class IExternalSensor;
}

class ThermostatController {
public:
    ThermostatController(hal::IExternalSensor* sensor, float hysteresis = 0.5f);

    // Read from the external sensor, calculate temperature, and execute cooling logic
    int update();

    // Direct logic update for testability
    int update(float currentTemp);

    int getCurrentStage() const { return m_currentStage; }
    float getCurrentTemperature() const { return m_currentTemperature; }

    // Map raw sensor value (0-4095) to temperature range (15.0°C to 30.0°C)
    static float rawToTemperature(uint32_t rawValue);

private:
    hal::IExternalSensor* m_sensor;
    float m_hysteresis;
    int m_currentStage; // 0: OFF, 1: Low, 2: Medium, 3: High, 4: Max
    float m_currentTemperature;

    // Helper to determine stage based on current temperature and hysteresis
    int calculateStage(float temp);
};

} // namespace arduino
