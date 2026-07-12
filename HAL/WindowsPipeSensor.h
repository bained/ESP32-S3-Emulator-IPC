#pragma once

#include "IExternalSensor.h"
#include <string>

namespace arduino {
namespace hal {

class WindowsPipeSensor : public IExternalSensor {
private:
    std::string m_pipeName;
    void* m_pipeHandle;    // Opaque pointer mapping to Windows HANDLE to avoid header macro pollution
    bool m_connected;
    uint32_t m_lastValue;

    // Attempts to connect/reconnect to the named pipe
    bool connectPipe();

public:
    WindowsPipeSensor(const std::string& pipeName);
    ~WindowsPipeSensor() override;

    // IExternalSensor API implementations
    bool isDataAvailable() override;
    uint32_t readValue() override;
    void sendCommand(const std::string& cmd) override;

    // Status helper
    bool isConnected() const { return m_connected; }
};

} // namespace hal
} // namespace arduino
