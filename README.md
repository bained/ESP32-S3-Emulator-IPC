# ESP32-S3 Windows Emulator with Real-Time IPC Simulation

An advanced, lightweight C++17 Windows-native emulator for the **ESP32-S3**, implementing an Arduino-compatible API layer. Designed for offline testing, verification, and hardware-in-the-loop (HIL) style simulation without deploying to actual physical boards. It leverages a clean Hardware Abstraction Layer (HAL) and high-speed local Inter-Process Communication (IPC) over Windows Named Pipes to simulate real-world sensor state transitions.

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
