cmake_minimum_required(VERSION 3.15)
project(esp32_s3_emulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Emulator source files grouped by their respective architectural modules
set(SOURCES
    main.cpp
    Core/Core.cpp
    GPIO/GPIO.cpp
    HAL/WindowsConsole.cpp
    HAL/WindowsTimer.cpp
    HAL/WindowsPipeSensor.cpp
    Logic/ThermostatController.cpp
    Logic/ReedSwitch.cpp
    Logic/PIRSensor.cpp
    Runtime/Runtime.cpp
    Serial/Serial.cpp
    Timing/Timing.cpp
    Utilities/Utilities.cpp
)

# Executable Target
add_executable(esp32_s3_emulator ${SOURCES})

# Include headers from /include and root-level directories
target_include_directories(esp32_s3_emulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# standalone Sensor Proxy Tool for IPC testing
add_executable(sensor_proxy tools/SensorProxy.cpp)
