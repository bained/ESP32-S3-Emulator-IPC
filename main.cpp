#include "include/Arduino.h"
#include "Runtime/Runtime.h"
#include "GPIO/GPIOManager.h"
#include "HAL/WindowsPipeSensor.h"
#include "Logic/PIRSensor.h"
#include "Logic/ButtonSensor.h"

// Standard configuration
const int lightPin = LED_BUILTIN; // Output LED
const int pirPin = 4;            // PIR sensor input pin
const int buttonPin = 6;         // Multi-click button input pin

// Instantiate the HAL named pipe client
static arduino::hal::WindowsPipeSensor pipeSensor("\\\\.\\pipe\\SensorPipe");

// Instantiate our virtual routed sensors
static arduino::PIRSensor pirSensor(&pipeSensor);
static arduino::ButtonSensor buttonSensor(&pipeSensor);

// Light states: 0 = OFF, 1 = DIM (Low), 2 = HIGH
int lightMode = 0;

// Button non-blocking detection state
int lastButtonState = HIGH;
unsigned long lastClickTime = 0;
int clickCount = 0;

// PIR non-blocking state
bool motionActive = false;
unsigned long motionTimer = 0;
const unsigned long motionTimeout = 10000; // 10-second motion timeout

// ==========================================
// User Application Code (Standard Sketch)
// ==========================================

void setup() {
    Serial.begin(115200);
    Serial.println("==========================================");
    Serial.println("    ESP32-S3 Windows Smart Light sketch   ");
    Serial.println("==========================================");

    // Initialize Pin Directions
    pinMode(lightPin, OUTPUT);
    pinMode(pirPin, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    // Register our filtered virtual sensor proxies into the GPIOManager mapping
    arduino::GPIOManager::getInstance().registerExternalSensor(pirPin, &pirSensor);
    arduino::GPIOManager::getInstance().registerExternalSensor(buttonPin, &buttonSensor);

    Serial.printf("[Setup] Registered PIR on pin %d, Button on pin %d\n", pirPin, buttonPin);
    Serial.println("[Setup] System Ready! Enter command in Proxy console:");
}

void loop() {
    unsigned long now = millis();

    // --------------------------------------------------
    // 1. Process Button Clicks (Multi-click logic)
    // --------------------------------------------------
    int buttonRead = digitalRead(buttonPin);
    if (buttonRead != lastButtonState) {
        lastButtonState = buttonRead;
        if (buttonRead == LOW) { // Button Pressed
            if (now - lastClickTime < 400) { // Double click time window: 400ms
                clickCount++;
            } else {
                clickCount = 1;
            }
            lastClickTime = now;
        }
    }

    // Process the registered clicks after the multi-click timeout window has passed
    if (clickCount > 0 && (now - lastClickTime > 400)) {
        if (clickCount == 1) {
            // Single Click -> Toggle Light between OFF and HIGH
            if (lightMode == 0) {
                lightMode = 2; // HIGH
                Serial.println("[App Event] Single Click detected -> Light set to HIGH (255)");
            } else {
                lightMode = 0; // OFF
                Serial.println("[App Event] Single Click detected -> Light set to OFF (0)");
            }
            motionActive = false; // Override/cancel active motion timers
        } else if (clickCount >= 2) {
            // Double Click -> Toggle Light Standby (DIM/LOW)
            if (lightMode != 1) {
                lightMode = 1; // LOW
                Serial.println("[App Event] Double Click detected -> Light set to LOW/DIM (64)");
            } else {
                lightMode = 0; // OFF
                Serial.println("[App Event] Double Click detected -> Light set to OFF (0)");
            }
            motionActive = false;
        }
        clickCount = 0;
    }

    // --------------------------------------------------
    // 2. Process PIR Motion Detections
    // --------------------------------------------------
    int pirRead = digitalRead(pirPin);
    Serial.printf("[DEBUG] PIR raw read: %d\n", pirRead);
    if (pirRead == HIGH) {
        if (!motionActive) {
            Serial.println("[PIR Event] Motion detected! Turning Light to HIGH for 10s standby.");
            motionActive = true;
        }
        motionTimer = now; // Refresh motion timer
        if (lightMode == 0) {
            lightMode = 2; // Turn on if off
        }
    }

    // Check for motion timeout
    if (motionActive && (now - motionTimer > motionTimeout)) {
        Serial.println("[PIR Event] Motion timeout expired. Turning Light OFF.");
        motionActive = false;
        lightMode = 0;
    }

    // --------------------------------------------------
    // 3. Write Light State (Analog PWM simulation)
    // --------------------------------------------------
    if (lightMode == 2) {
        analogWrite(lightPin, 255); // FULL
    } else if (lightMode == 1) {
        analogWrite(lightPin, 64);  // DIM
    } else {
        analogWrite(lightPin, 0);   // OFF
    }

    // Status report tick
    static unsigned long lastReportTime = 0;
    if (now - lastReportTime > 2000) {
        lastReportTime = now;
        Serial.printf("[Status] Light Mode: %d | Motion Active: %s | Time: %lu ms\n",
                      lightMode,
                      (motionActive ? "YES" : "NO"),
                      now);
    }

    delay(50); // Fast cycle tick for crisp button response
}

// ==========================================
// Emulator Entry Point
// ==========================================

int main() {
    arduino::Runtime emulatorRuntime;
    emulatorRuntime.run();
    return 0;
}
