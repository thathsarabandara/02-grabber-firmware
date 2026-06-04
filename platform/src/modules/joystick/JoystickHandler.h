#ifndef JOYSTICK_HANDLER_H
#define JOYSTICK_HANDLER_H

#include "../../../include/Config.h"
#include "../motion/MotionManager.h"
#include "../safety/SafetySystem.h"

class JoystickHandler {
public:
    JoystickHandler(MotionManager& motion);
    
    void readAndProcess();

private:
    MotionManager& _motion;
    
    void processAxis(int rawValue, int servoIndex);
};

#endif // JOYSTICK_HANDLER_H
