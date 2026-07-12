# ESP32-S3 Emulator Developer Tutorial & Guide

Welcome to the development tutorial! This guide covers how the emulator functions, how to configure your environment, step-by-step testing of the real-time simulation, and how to write custom sensors using our Hardware Abstraction Layer (HAL).

---

## 💻 1. Getting Started & Environment Setup

The ESP32-S3 emulator compiles as a standard, modern C++17 console application on Windows. 

### Recommended Setup:
1. **IDE**: [Visual Studio Code](https://code.visualstudio.com/) or Visual Studio.
2. **C++ Compiler**: 
   * Install Visual Studio with the "Desktop development with C++" workload (MSVC compiler), OR
   * Install [MinGW-w64](https://www.mingw-w64.org/) / GCC on Windows.
3. **Build Tools**: [CMake](https://cmake.org/) (Version 3.15 or newer).
4. **VS Code Extensions**:
   * *C/C++ Extension Pack* (by Microsoft)
   * *CMake Tools* (by Microsoft)

---

## 📡 2. How it Works: The IPC Named Pipe Pattern

To simulate external hardware input (such as pushing buttons or waving hands in front of motion sensors), the emulator integrates a local Inter-Process Communication (IPC) boundary.

### The Named Pipe Mechanism
On Windows, Named Pipes are a high-speed, local duplex communication mechanism.
* **Server**: The `sensor_proxy` executable establishes and opens a named pipe server at `\\.\pipe\SensorPipe`.
* **Client**: The `esp32_s3_emulator` starts up and connects to `\\.\pipe\SensorPipe` using its native `WindowsPipeSensor` class.
* **Non-blocking Broadcasting**: Both the `PIRSensor` and `ButtonSensor` logic classes reference the active `WindowsPipeSensor` instance. They query and process the latest parsed data over the pipe buffer in a non-blocking format during the standard `loop()` ticks.

### The Communication Protocol
The emulator and proxy communicate using a simple binary protocol consisting of single `uint32_t` state packets:

| Sent Value (binary `uint32_t`) | Simulated Hardware Event | Mapped Virtual Pin State |
| :---: | :--- | :--- |
| **`0`** | Button Pressed | `Button Pin -> LOW` |
| **`1`** | Button Released | `Button Pin -> HIGH` |
| **`2`** | Motion Detected | `PIR Pin -> HIGH` |
| **`3`** | Motion Cleared | `PIR Pin -> LOW` |

---

## 📋 Sensor Inventory

For comprehensive end-to-end testing, the default `main.cpp` application maps various physical sensors to virtual pins. Below is the detailed breakdown of all registered and simulated sensors.

| Sensor Name | Mapped Pin | Protocol (Values Sent) | Function & Behavior | How to Trigger |
| :--- | :---: | :--- | :--- | :--- |
| **PIR Motion Sensor** | `Pin 4` | `2` (HIGH), `3` (LOW) | Triggers dim light to Full HIGH for 10 seconds on motion. | Type **`P`** to detect motion; **`O`** to clear. |
| **Multi-click Button** | `Pin 6` | `0` (LOW), `1` (HIGH) | Single click toggles light; double-click sets DIM/Standby mode. | Type **`B`** to simulate push-release click. |
| **Magnetic Reed Switch** | `Pin 5` | `0` (LOW), `1` (HIGH) | Available logic mapping for contact magnet alerts. | Type **`M`** for Magnet near; **`N`** for Magnet away. |
| **Thermostat Temp Sensor** | `Pin 4` | `0` - `4095` (12-bit ADC) | Available analog control map for 4-stage automated cooling stages. | Type any integer **`0`** to **`4095`** to set level. |

---

## 🧪 3. Step-by-Step Testing Guide

Follow this step-by-step guide to run the live Smart Light Controller simulation.

### Step 1: Open Two Terminals
Open two separate terminals in the project's root folder.

### Step 2: Build the Targets
In either terminal, compile the project:
```bash
cmake -B build -S .
cmake --build build --config Debug
```

### Step 3: Launch the Standalone Server (Sensor Proxy)
In **Terminal A**, run the Sensor Proxy tool. It will launch, create the pipe, and wait for the client to boot:
```bash
# Using standard PowerShell/CMD:
.\build\Debug\sensor_proxy.exe
```

### Step 4: Launch the Client (Microcontroller Emulator)
In **Terminal B**, boot the emulator. It will connect to the active pipe and start executing the Arduino `setup()` and `loop()` functions:
```bash
.\build\Debug\esp32_s3_emulator.exe
```
You will immediately see periodic status reports in the emulator console:
`[Status] Light Mode: 0 | Motion Active: NO | Time: 2000 ms`

---

### Step 5: Test Simulated Inputs

#### Testing the PIR Motion Sensor:
1. Go to **Terminal A** (Sensor Proxy) and press **`P`** (then press Enter) to simulate motion.
2. In **Terminal B** (Emulator), you will see:
   `[PIR Event] Motion detected! Turning Light to HIGH for 10s standby.`
   The raw debug logging will output `PIR raw read: 1`.
3. In **Terminal A**, press **`O`** (then press Enter) to clear the motion.
4. Wait 10 seconds. In the Emulator, you will observe the non-blocking timer expire and shut off the LED:
   `[PIR Event] Motion timeout expired. Turning Light OFF.`

#### Testing the Button Click (Single/Double Clicks):
1. In **Terminal A** (Sensor Proxy), press **`B`** (then press Enter) once.
2. The proxy automatically simulates a physical `Press (LOW) -> hold 150ms -> Release (HIGH)` click sequence.
3. In **Terminal B** (Emulator), you will see:
   `[App Event] Single Click detected -> Light set to HIGH (255)`
4. Press **`B`** twice rapidly (within 400 milliseconds) in **Terminal A**.
5. In **Terminal B**, the double-click detector will trigger:
   `[App Event] Double Click detected -> Light set to LOW/DIM (64)`

---

## 🔌 4. Extending: How to Add a New External Sensor

To add a new external sensor (for example, a **Light Lux Sensor** or an **I2C Temperature Sensor**), follow this architectural guide.

### Step 1: Define the sensor message protocol
Assign a new unique packet id/value (e.g., `4` for light level or a range representing integers).

### Step 2: Create your Sensor Class
Create `Logic/LightSensor.h` and inherit from `arduino::hal::IExternalSensor`:

```cpp
#pragma once
#include "../HAL/IExternalSensor.h"

namespace arduino {

class LightSensor : public hal::IExternalSensor {
private:
    hal::IExternalSensor* m_parentPipe;
    uint32_t m_luxLevel;

public:
    LightSensor(hal::IExternalSensor* parentPipe) 
        : m_parentPipe(parentPipe), m_luxLevel(100) {}

    bool isDataAvailable() override { return m_parentPipe ? m_parentPipe->isDataAvailable() : false; }

    uint32_t readValue() override {
        if (m_parentPipe) {
            uint32_t val = m_parentPipe->readValue();
            // Suppose values >= 100 represent raw lux values passed through
            if (val >= 100) {
                m_luxLevel = val;
            }
        }
        return m_luxLevel;
    }

    void sendCommand(const std::string& cmd) override {
        if (m_parentPipe) m_parentPipe->sendCommand(cmd);
    }
};

} // namespace arduino
```

### Step 3: Register in `main.cpp`
Instantiate the sensor, configure its input pin direction, and bind it inside your sketch `setup()`:
```cpp
// 1. Declare instance globally
static arduino::LightSensor lightSensor(&pipeSensor);

void setup() {
    // ...
    pinMode(7, INPUT); // Virtual Pin 7

    // 2. Register to pin 7
    arduino::GPIOManager::getInstance().registerExternalSensor(7, &lightSensor);
}

void loop() {
    // 3. Read it standard-style!
    int lux = analogRead(7);
    Serial.printf("Current light level: %d Lux\n", lux);
    delay(1000);
}
```

### Step 4: Add to `tools/SensorProxy.cpp`
Update the proxy's input menu to capture keyboard values (e.g. typing a number like `450` representing simulated Lux) and transmit it to the pipe! Your emulator will automatically catch and route it safely.
