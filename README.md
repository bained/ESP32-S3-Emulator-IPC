# ESP32-S3 Windows Emulator with Real-Time IPC Simulation

An advanced, lightweight C++17 Windows-native emulator for the **ESP32-S3**, implementing an Arduino-compatible API layer. Designed for offline testing, verification, and hardware-in-the-loop (HIL) style simulation without deploying to actual physical boards. It leverages a clean Hardware Abstraction Layer (HAL) and high-speed local Inter-Process Communication (IPC) over Windows Named Pipes to simulate real-world sensor state transitions.

---

Demo video:
[![Watch the demo video](https://img.youtube.com/vi/dKPxLus1tC4/0.jpg)](https://www.youtube.com/watch?v=dKPxLus1tC4)

---

## 🚀 Key Features

* **Strict Module Architecture**: Follows clean, decoupled layers—*Runtime, Core, GPIO, Serial, Timing, HAL, and Utilities*.
* **Standard Arduino Core APIs**: Fully emulates standard methods like `pinMode`, `digitalWrite`, `digitalRead`, `analogRead`, `analogWrite`, `millis`, `micros`, `delay`, `delayMicroseconds`, and variadic `Serial.printf`.
* **Dynamic IPC Named Pipe Simulation**: Houses a duplex client named pipe sensor driver (`WindowsPipeSensor`) mapping state streams to virtual GPIO registers.
* **Complex Subsystem Logic Examples**:
  * **Smart Light Controller**: Advanced non-blocking multi-click button processing (single/double click detection) and PIR motion-activated timeout illumination.
  * **HVAC Thermostat Control**: Built-in 4-stage cooling transition algorithm with custom hysteresis logic preventing contact chattering.
  * **Reed Switch (Magnetic Sensor)**: Active-low magnetic field detection mapped to onboarding alerts.

---

## 📋 Sensor Inventory

Below is the comprehensive list of physical sensors simulated in this project. The default application sketch in `main.cpp` currently integrates the **PIR Motion Sensor** and the **Multi-click Button** to drive the Smart Light logic.

### 1. PIR Motion Sensor
* **Name & Function**: PIR (Passive Infrared) Motion Sensor. Detects thermal radiation changes caused by motion and holds the light active for 10 seconds.
* **Hardware Pin**: `Pin 4` (Configured as `INPUT`)
* **Communication Protocol**: Transmits commands over Named Pipe IPC:
  * Value `2` (HIGH) -> Motion Detected.
  * Value `3` (LOW) -> No Motion (Clear).
* **How to Test**: 
  * Type **`P`** in the `sensor_proxy` console to trigger motion (Turns light high).
  * Type **`O`** in the `sensor_proxy` console to clear motion (Turns light off after a 10s timeout).

### 2. Multi-click Button
* **Name & Function**: Push Button with multi-click detection. Distinguishes between a single-click (toggle ON/OFF) and double-click (DIM standby state).
* **Hardware Pin**: `Pin 6` (Configured as `INPUT_PULLUP`)
* **Communication Protocol**: Transmits binary levels over Named Pipe IPC:
  * Value `0` (LOW) -> Button Pressed.
  * Value `1` (HIGH) -> Button Released.
* **How to Test**:
  * Type **`B`** in the `sensor_proxy` console. The proxy automatically simulates a quick push-and-release pattern.

### 3. Magnetic Reed Switch (Module Available)
* **Name & Function**: Magnetic contact sensor. Closed (LOW) when a magnet is nearby, Open (HIGH) when away.
* **Hardware Pin**: `Pin 5` (Configured as `INPUT_PULLUP` when active)
* **Communication Protocol**: Transmits binary levels over Named Pipe IPC:
  * Value `0` (LOW) -> Magnet Near (Closed).
  * Value `1` (HIGH) -> Magnet Away (Open).
* **How to Test**:
  * Type **`M`** in the `sensor_proxy` console to simulate magnet near.
  * Type **`N`** in the `sensor_proxy` console to simulate magnet away.

### 4. Thermostat Temperature Sensor (Module Available)
* **Name & Function**: Analog NTC Thermistor sensor mapping temperatures to automated 4-stage cooling systems.
* **Hardware Pin**: `Pin 4` (Configured as `INPUT` when active)
* **Communication Protocol**: Receives raw 12-bit ADC integers over Named Pipe IPC:
  * Map `0 - 4095` -> Maps to `15.0°C - 30.0°C` temperature range.
* **How to Test**:
  * Type any integer from **`0`** to **`4095`** in the `sensor_proxy` console to update simulated ambient temperature.

---

## 📐 Hardware Abstraction Layer (HAL) & IPC Architecture

To keep the codebase pure and cross-platform ready, all Windows-specific operating system calls (Named Pipes, high-precision timing, console I/O) are strictly isolated behind pure virtual interfaces within the `arduino::hal` namespace:
* `ITimer` / `WindowsTimer` — High-resolution standard `<chrono>` ticks and high-frequency sleep methods.
* `IConsole` / `WindowsConsole` — Redirects serial streams safely to standard terminal streams.
* `IExternalSensor` / `WindowsPipeSensor` — Duplex IPC Named Pipe interface running on `\\.\pipe\SensorPipe`.

### IPC Routing Concept

```
  +----------------------+                     +---------------------+
  |   Sensor Proxy Tool  |                     |  ESP32-S3 Emulator  |
  |  (Standalone Server) |                     |  (Arduino Sketch)   |
  |                      |                     |                     |
  |  Listens for Keys    | <=================> |  PIRSensor          |
  |  e.g., 'P', 'B', 'M' |     Named Pipe      |  ButtonSensor       |
  |                      |  \\.\pipe\SensorPipe|  GPIOManager        |
  +----------------------+                     +---------------------+
```

---

## 🛠️ Quick Start

### Prerequisites
* Windows 10/11
* A C++17 compatible compiler (e.g., MSVC, MinGW, or GCC)
* [CMake](https://cmake.org/) (Version 3.15 or newer)

### Build the Project
Open your favorite terminal (Command Prompt, PowerShell, or Git Bash) in the root directory and run:

```bash
# 1. Configure the build system
cmake -B build -S .

# 2. Compile both the emulator and the Sensor Proxy Tool
cmake --build build --config Debug
```

This generates two main executables in the `build/Debug/` directory:
1. `esp32_s3_emulator.exe` — The simulated Arduino microcontroller execution loop.
2. `sensor_proxy.exe` — The tool to push simulated physical key clicks and state triggers to the emulator.

---

## 📚 Detailed Documentation & Step-by-Step Testing

For a detailed walkthrough on setting up your environment, running tests with the interactive proxy console, and learning how to write custom external sensors for your virtual pins, check out the **[TUTORIAL.md](./TUTORIAL.md)**.
