#include "ThermostatController.h"
#include "../HAL/IExternalSensor.h"
#include "../GPIO/GPIO.h"

namespace arduino {

ThermostatController::ThermostatController(hal::IExternalSensor* sensor, float hysteresis)
    : m_sensor(sensor), m_hysteresis(hysteresis), m_currentStage(0), m_currentTemperature(15.0f) {}

float ThermostatController::rawToTemperature(uint32_t rawValue) {
    // Clamp raw value to 12-bit ADC range (0-4095)
    if (rawValue > 4095) rawValue = 4095;
    
    // Map 0 -> 15.0°C and 4095 -> 30.0°C
    return 15.0f + (static_cast<float>(rawValue) / 4095.0f) * 15.0f;
}

int ThermostatController::update() {
    if (m_sensor != nullptr) {
        uint32_t rawVal = m_sensor->readValue();
        m_currentTemperature = rawToTemperature(rawVal);
        return update(m_currentTemperature);
    }
    return m_currentStage;
}

int ThermostatController::update(float currentTemp) {
    m_currentTemperature = currentTemp;
    m_currentStage = calculateStage(currentTemp);

    // Write stage states to standard output pins to allow physical/visual tracking in emulator
    // Pin 10: Low, Pin 11: Medium, Pin 12: High, Pin 13: Max
    digitalWrite(10, (m_currentStage >= 1) ? HIGH : LOW);
    digitalWrite(11, (m_currentStage >= 2) ? HIGH : LOW);
    digitalWrite(12, (m_currentStage >= 3) ? HIGH : LOW);
    digitalWrite(13, (m_currentStage >= 4) ? HIGH : LOW);

    return m_currentStage;
}

int ThermostatController::calculateStage(float temp) {
    int proposedStage = 0;

    // Direct mapping thresholds
    if (temp >= 27.0f) {
        proposedStage = 4; // Max
    } else if (temp >= 24.0f) {
        proposedStage = 3; // High
    } else if (temp >= 21.0f) {
        proposedStage = 2; // Medium
    } else if (temp >= 18.0f) {
        proposedStage = 1; // Low
    } else {
        proposedStage = 0; // OFF
    }

    // Apply Hysteresis to prevent chattering when moving downwards
    if (proposedStage < m_currentStage) {
        float threshold = 18.0f;
        if (m_currentStage == 4)      threshold = 27.0f;
        else if (m_currentStage == 3) threshold = 24.0f;
        else if (m_currentStage == 2) threshold = 21.0f;
        else if (m_currentStage == 1) threshold = 18.0f;

        // If the temp has not dropped below the threshold minus the hysteresis deadband,
        // we keep the active, higher cooling stage.
        if (temp > (threshold - m_hysteresis)) {
            return m_currentStage;
        }
    }

    return proposedStage;
}

} // namespace arduino
