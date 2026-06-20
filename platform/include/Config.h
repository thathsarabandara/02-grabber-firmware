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
#define USE_JOYSTICK    false   // Set to true if physical joysticks are connected
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
// POWER / BATTERY SETTINGS
// ======================================================
#define INA226_ADDR             0x44  // Note: default 0x40 conflicts with PCA9685! Solder A0 pad on INA226.
#define INA226_SHUNT_RESISTOR   0.1f  // Typical shunt resistor on breakout boards (0.1 Ohm / R100)
#define INA226_MAX_CURRENT      4.0f  // Maximum expected load current in Amps
#define BATTERY_VOLTAGE_MAX     8.4f   // 2S Li-ion max
#define BATTERY_VOLTAGE_MIN     6.0f   // 2S Li-ion safe min
#define BATTERY_CAPACITY_MAH    1800.0f // 2x 18650 in Series

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

// ======================================================
// NETWORK SETTINGS
// ======================================================
// Fallback WiFi credentials if WiFiManager fails
#define FALLBACK_WIFI_SSID      "TP-Link_D664"
#define FALLBACK_WIFI_PASSWORD  "Bandara@2001"

// API Gateway config
// Note: ESP32 needs the IP address of the machine running the API Gateway, not localhost
#define API_REGISTER_URL        "http://192.168.1.103:8000/api/v1/robots/register"

// MQTT Broker config
#define MQTT_BROKER             "192.168.1.103"
#define MQTT_PORT               1883
#define MQTT_USER               "thathsara"
#define MQTT_PASS               "BandaPutha"

// ======================================================
// ROBOT IDENTITY
// ======================================================
#define ROBOT_ID                "GRABBER-V1-ESP32"
#define SERIAL_KEY              "1234-5678-9012-3456"

#endif // CONFIG_H
