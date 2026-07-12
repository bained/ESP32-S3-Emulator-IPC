#define NOMINMAX
#include "WindowsPipeSensor.h"
#include <windows.h>

namespace arduino {
namespace hal {

WindowsPipeSensor::WindowsPipeSensor(const std::string& pipeName)
    : m_pipeName(pipeName), m_pipeHandle(INVALID_HANDLE_VALUE), m_connected(false), m_lastValue(0) {
    // Initial attempt to connect; if it fails, it will retry on subsequent reads
    connectPipe();
}

WindowsPipeSensor::~WindowsPipeSensor() {
    if (m_pipeHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(static_cast<HANDLE>(m_pipeHandle));
    }
}

bool WindowsPipeSensor::connectPipe() {
    if (m_connected && m_pipeHandle != INVALID_HANDLE_VALUE) {
        return true;
    }

    // Attempt to open the named pipe in non-blocking / standard client mode
    HANDLE handle = CreateFileA(
        m_pipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,              // No sharing
        NULL,           // Default security attributes
        OPEN_EXISTING,  // Opens existing pipe
        0,              // Default attributes
        NULL            // No template file
    );

    if (handle != INVALID_HANDLE_VALUE) {
        m_pipeHandle = static_cast<void*>(handle);
        m_connected = true;
        return true;
    }

    m_connected = false;
    return false;
}

bool WindowsPipeSensor::isDataAvailable() {
    if (!m_connected || m_pipeHandle == INVALID_HANDLE_VALUE) {
        if (!connectPipe()) {
            return false;
        }
    }

    DWORD bytesAvailable = 0;
    BOOL success = PeekNamedPipe(
        static_cast<HANDLE>(m_pipeHandle),
        NULL,           // Buffer
        0,              // Buffer size
        NULL,           // Bytes read
        &bytesAvailable,
        NULL            // Bytes left in message
    );

    if (!success) {
        // If Peek fails, the pipe has likely disconnected or broken
        CloseHandle(static_cast<HANDLE>(m_pipeHandle));
        m_pipeHandle = INVALID_HANDLE_VALUE;
        m_connected = false;
        return false;
    }

    return bytesAvailable >= sizeof(uint32_t);
}

uint32_t WindowsPipeSensor::readValue() {
    if (!isDataAvailable()) {
        return m_lastValue; // Return cached last known value if no new data is present
    }

    uint32_t value = 0;
    DWORD bytesRead = 0;
    BOOL success = ReadFile(
        static_cast<HANDLE>(m_pipeHandle),
        &value,
        sizeof(value),
        &bytesRead,
        NULL
    );

    if (success && bytesRead == sizeof(value)) {
        m_lastValue = value;
    } else if (!success) {
        // Pipe broke during read
        CloseHandle(static_cast<HANDLE>(m_pipeHandle));
        m_pipeHandle = INVALID_HANDLE_VALUE;
        m_connected = false;
    }

    return m_lastValue;
}

void WindowsPipeSensor::sendCommand(const std::string& cmd) {
    if (!m_connected || m_pipeHandle == INVALID_HANDLE_VALUE) {
        if (!connectPipe()) {
            return;
        }
    }

    DWORD bytesWritten = 0;
    BOOL success = WriteFile(
        static_cast<HANDLE>(m_pipeHandle),
        cmd.c_str(),
        static_cast<DWORD>(cmd.length()),
        &bytesWritten,
        NULL
    );

    if (!success) {
        // Pipe broke during write
        CloseHandle(static_cast<HANDLE>(m_pipeHandle));
        m_pipeHandle = INVALID_HANDLE_VALUE;
        m_connected = false;
    }
}

} // namespace hal
} // namespace arduino
