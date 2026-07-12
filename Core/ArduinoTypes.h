#pragma once

#include <cstdint>
#include <string>

// Standard Arduino Type Definitions
using byte = uint8_t;
using word = uint16_t;
using boolean = bool;

// Using std::string as a basis for Arduino String in the emulator.
// In the future, this can be extended with compatibility helper functions or a custom class if needed.
using String = std::string;
