#include <Arduino.h>
#include <Wire.h>
#include "include/Config.h"
#include "src/modules/servo_control/ServoController.h"
#include "src/modules/motion/MotionManager.h"
#include "src/modules/joystick/JoystickHandler.h"
#include "src/modules/bluetooth/BluetoothHandler.h"

// ======================================================
// GLOBAL INSTANCES
// ======================================================
ServoController servoCtrl(PCA9685_ADDR);
MotionManager   motion(servoCtrl);
JoystickHandler joysticks(motion);
BluetoothHandler ble(motion);

// ======================================================
// DEBUG UTILITIES
// ======================================================
void printState() {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        Serial.printf("Base: %.2f | Shoulder: %.2f | Elbow: %.2f | Grip: %.2f\n",
            motion.getCurrent(0),
            motion.getCurrent(1),
            motion.getCurrent(2),
            motion.getCurrent(3)
        );
    }
}

// ======================================================
// SETUP
// ======================================================
void setup() {
    Serial.begin(115200);
    
    // Initialize I2C for PCA9685
    Wire.begin(21, 22);
    Wire.setClock(100000);
    
    // Initialize Motion System (calls servoCtrl.begin and startup sequence)
    motion.begin();
    
    // Initialize BLE Server
    ble.begin();
    
    Serial.println("System initialized and ready.");
}

// ======================================================
// MAIN LOOP
// ======================================================
void loop() {
    // 1. Process Input (Only read physical joysticks if no BLE client is overriding)
    if (!ble.isConnected()) {
        joysticks.readAndProcess();
    }
    
    // 2. Update Motion (Smoothing and Servo writing)
    motion.update();
    
    // 3. Debug Output
    printState();
    
    // 4. Rate Limiting
    delay(UPDATE_DELAY_MS);
}