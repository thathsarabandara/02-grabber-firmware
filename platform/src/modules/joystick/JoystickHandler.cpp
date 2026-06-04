#include "JoystickHandler.h"

JoystickHandler::JoystickHandler(MotionManager& motion) : _motion(motion) {}

void JoystickHandler::readAndProcess() {
    processAxis(analogRead(JOY1_X), 0); // BASE
    processAxis(analogRead(JOY1_Y), 1); // SHOULDER
    processAxis(analogRead(JOY2_X), 2); // ELBOW
    processAxis(analogRead(JOY2_Y), 3); // GRIPPER
}

void JoystickHandler::processAxis(int rawValue, int servoIndex) {
    int offset = rawValue - JOYSTICK_CENTER;

    if (abs(offset) < JOYSTICK_DEADZONE) {
        return;
    }

    float normalized = (float)abs(offset) / (float)JOYSTICK_CENTER;
    normalized = constrain(normalized, 0.0f, 1.0f);

    // Apply quadratic curve for smoother control at low stick deflection
    float intensity = normalized * normalized;
    
    // Maintain direction
    if (offset < 0) {
        intensity = -intensity;
    }

    _motion.moveJoint(servoIndex, intensity);
}
