#include <Arduino.h>
#include <Wire.h>
#include "include/Config.h"
#include "src/modules/servo_control/ServoController.h"
#include "src/modules/motion/MotionManager.h"
#include "src/modules/joystick/JoystickHandler.h"
#include "src/modules/bluetooth/BluetoothHandler.h"
#include "src/modules/network/RobotNetwork.h"
#include "src/modules/power/PowerMonitor.h"

// ======================================================
// GLOBAL INSTANCES
// ======================================================
ServoController servoCtrl(PCA9685_ADDR);
MotionManager   motion(servoCtrl);
JoystickHandler joysticks(motion);
BluetoothHandler ble(motion);
RobotNetwork   network(motion);

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
        if (powerMonitor.isDetected()) {
            Serial.printf("Power: %.2fV | %.2fmA (Peak: %.2fmA) | %.2fmW | %.3fWh | %.1f%% (%.1fmins left)\n",
                powerMonitor.getVoltage(),
                powerMonitor.getCurrent(),
                powerMonitor.getPeakCurrent(),
                powerMonitor.getPower(),
                powerMonitor.getEnergyWh(),
                powerMonitor.getRemainingCapacityPercent(),
                powerMonitor.getRuntimePredictionMins()
            );
        } else {
            Serial.println("Power: INA226 NOT DETECTED! Check I2C bus wiring/address.");
        }
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
    
    // Initialize WiFi, MQTT and HTTP Registration
    network.begin();
    
    // Initialize Power Monitor
    powerMonitor.begin();
    
    Serial.println("System initialized and ready.");
}

// ======================================================
// MAIN LOOP
// ======================================================
void loop() {
    // 1. Keep Network (MQTT) alive
    network.update();

    // 2. Process Input (Priority: MQTT > BLE > Joystick)
    // If MQTT is connected, it can override local controls. 
    // Let's allow Joystick if NO BLE client and NO recent MQTT commands.
    // For simplicity,don't read joystick if BLE is connected or MQTT is actively controlling.
    // We'll read joystick only if BLE is not connected. MQTT commands just set target directly via callback.
    if (!ble.isConnected()) {
        joysticks.readAndProcess();
    }
    
    // 2. Update Motion (Smoothing and Servo writing)
    motion.update();
    
    // 3. Update Power Monitor
    powerMonitor.update(motion.isMoving());
    
    // 4. Debug Output
    printState();
    
    // 5. Rate Limiting
    delay(UPDATE_DELAY_MS);
}