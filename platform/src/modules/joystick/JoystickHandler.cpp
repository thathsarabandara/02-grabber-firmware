#include "JoystickHandler.h"

JoystickHandler::JoystickHandler(MotionManager& motion) : _motion(motion) {}

void JoystickHandler::readAndProcess() {
#if defined(USE_JOYSTICK) && (USE_JOYSTICK == true)
    int baseVal = analogRead(JOY1_X);
    int shoulderVal = analogRead(JOY1_Y);
    int elbowVal = analogRead(JOY2_X);
    int gripperVal = analogRead(JOY2_Y);

    // Safety check: if all axes read exactly 0, the joystick module is likely disconnected or unpowered.
    // Skip processing to prevent the robot from moving to minimum limits.
    if (baseVal == 0 && shoulderVal == 0 && elbowVal == 0 && gripperVal == 0) {
        return;
    }

    processAxis(baseVal, 0);      // BASE
    processAxis(shoulderVal, 1);  // SHOULDER
    processAxis(elbowVal, 2);     // ELBOW
    processAxis(gripperVal, 3);    // GRIPPER
#endif
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
