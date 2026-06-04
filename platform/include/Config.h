#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ======================================================
// PCA9685 SETTINGS
// ======================================================
#define PCA9685_ADDR    0x40
#define SERVO_FREQ      50
#define SERVOMIN        125
#define SERVOMAX        575

// ======================================================
// JOYSTICK PINS (ESP32 ADC)
// ======================================================
#define JOY1_X          32
#define JOY1_Y          33
#define JOY2_X          34
#define JOY2_Y          35

// ======================================================
// SERVO CHANNELS
// ======================================================
#define BASE_SERVO      0
#define SHOULDER_SERVO  1
#define ELBOW_SERVO     2
#define GRIPPER_SERVO   3

// ======================================================
// SAFE LIMITS (Degrees)
// ======================================================
#define BASE_MIN        1
#define BASE_MAX        180
#define BASE_HOME       90

#define SHOULDER_MIN    40
#define SHOULDER_MAX    120
#define SHOULDER_HOME   90

#define ELBOW_MIN       20
#define ELBOW_MAX       80
#define ELBOW_HOME      50

#define GRIPPER_MIN     70
#define GRIPPER_MAX     100
#define GRIPPER_HOME    90

// ======================================================
// MOVEMENT SETTINGS
// ======================================================
const float DEFAULT_SMOOTH_FACTOR = 0.08f;
const float DEFAULT_MAX_SPEED     = 0.9f;
const int   UPDATE_DELAY_MS       = 15;
const int   JOYSTICK_DEADZONE     = 180;
const int   JOYSTICK_CENTER       = 2048;

// ======================================================
// DATA STRUCTURES
// ======================================================
struct ServoData {
    uint8_t channel;
    float currentAngle;
    float targetAngle;
    int minAngle;
    int maxAngle;
};

#endif // CONFIG_H
