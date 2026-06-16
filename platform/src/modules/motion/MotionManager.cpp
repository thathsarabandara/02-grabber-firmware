#include "MotionManager.h"
#include "../safety/SafetySystem.h"

MotionManager::MotionManager(ServoController& controller) 
    : _controller(controller), _smoothFactor(DEFAULT_SMOOTH_FACTOR), _emergencyStop(false) 
{
    // Initialize servo data with home positions and limits
    _servos[0] = {BASE_SERVO, BASE_HOME, BASE_HOME, BASE_MIN, BASE_MAX};
    _servos[1] = {SHOULDER_SERVO, SHOULDER_HOME, SHOULDER_HOME, SHOULDER_MIN, SHOULDER_MAX};
    _servos[2] = {ELBOW_SERVO, ELBOW_HOME, ELBOW_HOME, ELBOW_MIN, ELBOW_MAX};
    _servos[3] = {GRIPPER_SERVO, GRIPPER_HOME, GRIPPER_HOME, GRIPPER_MIN, GRIPPER_MAX};
}

void MotionManager::begin() {
    _controller.begin();
    startupSequence();
}

void MotionManager::update() {
    if (_emergencyStop) return;
    for (int i = 0; i < 4; i++) {
        float error = _servos[i].targetAngle - _servos[i].currentAngle;

        if (abs(error) > 0.05f) {
            _servos[i].currentAngle += error * _smoothFactor;
            _controller.setAngle(_servos[i].channel, _servos[i].currentAngle);
        }
    }
}

void MotionManager::setTarget(int servoIndex, float newTarget) {
    if (_emergencyStop) return;
    if (servoIndex < 0 || servoIndex >= 4) return;
    
    _servos[servoIndex].targetAngle = constrain(
        newTarget, 
        _servos[servoIndex].minAngle, 
        _servos[servoIndex].maxAngle
    );
}

void MotionManager::moveJoint(int servoIndex, float intensity) {
    if (_emergencyStop) return;
    if (servoIndex < 0 || servoIndex >= 4) return;
    
    float direction = (intensity > 0) ? 1.0f : -1.0f;
    float absIntensity = abs(intensity);
    
    float safeSpeed = SafetySystem::calculateSafeSpeed(
        _servos[servoIndex].targetAngle,
        _servos[servoIndex].minAngle,
        _servos[servoIndex].maxAngle,
        direction,
        DEFAULT_MAX_SPEED
    );
    
    float movement = absIntensity * safeSpeed * direction;
    setTarget(servoIndex, _servos[servoIndex].targetAngle + movement);
}

float MotionManager::getTarget(int servoIndex) const {
    return (servoIndex >= 0 && servoIndex < 4) ? _servos[servoIndex].targetAngle : 0;
}

float MotionManager::getCurrent(int servoIndex) const {
    return (servoIndex >= 0 && servoIndex < 4) ? _servos[servoIndex].currentAngle : 0;
}

bool MotionManager::isMoving() const {
    for (int i = 0; i < 4; i++) {
        float error = _servos[i].targetAngle - _servos[i].currentAngle;
        if (abs(error) > 0.05f) {
            return true;
        }
    }
    return false;
}

void MotionManager::setEmergencyStop(bool stop) {
    _emergencyStop = stop;
}

bool MotionManager::isEmergencyStop() const {
    return _emergencyStop;
}

void MotionManager::startupSequence() {
    // Initial sync
    for (int i = 0; i < 4; i++) {
        _controller.setAngle(_servos[i].channel, _servos[i].currentAngle);
    }
    delay(1000);
    Serial.println("Motion System Initialized");
}
