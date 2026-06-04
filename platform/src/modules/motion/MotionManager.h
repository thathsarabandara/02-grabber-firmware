#ifndef MOTION_MANAGER_H
#define MOTION_MANAGER_H

#include "../../../include/Config.h"
#include "../servo_control/ServoController.h"

class MotionManager {
public:
    MotionManager(ServoController& controller);

    void begin();
    void update();
    void setTarget(int servoIndex, float newTarget);
    void moveJoint(int servoIndex, float intensity);
    float getTarget(int servoIndex) const;
    float getCurrent(int servoIndex) const;
    
    void startupSequence();

private:
    ServoController& _controller;
    ServoData _servos[4];
    float _smoothFactor;
};

#endif // MOTION_MANAGER_H
