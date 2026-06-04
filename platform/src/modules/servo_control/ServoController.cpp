#include "ServoController.h"

ServoController::ServoController(uint8_t addr) : _pwm(addr) {}

void ServoController::begin() {
    _pwm.begin();
    _pwm.setPWMFreq(SERVO_FREQ);
}

void ServoController::setAngle(uint8_t channel, float angle) {
    angle = constrain(angle, 0, 180);
    int pulse = angleToPulse(angle);
    _pwm.setPWM(channel, 0, pulse);
}

void ServoController::detach(uint8_t channel) {
    _pwm.setPWM(channel, 0, 0);
}

void ServoController::attachAll(const ServoData* servos, int count) {
    for (int i = 0; i < count; i++) {
        setAngle(servos[i].channel, servos[i].currentAngle);
    }
}

void ServoController::detachAll(const ServoData* servos, int count) {
    for (int i = 0; i < count; i++) {
        detach(servos[i].channel);
    }
}

int ServoController::angleToPulse(float angle) {
    return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}
