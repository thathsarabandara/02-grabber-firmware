#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Adafruit_PWMServoDriver.h>
#include "../../../include/Config.h"

class ServoController {
public:
    ServoController(uint8_t addr = PCA9685_ADDR);
    
    void begin();
    void setAngle(uint8_t channel, float angle);
    void detach(uint8_t channel);
    void attachAll(const ServoData* servos, int count);
    void detachAll(const ServoData* servos, int count);

private:
    Adafruit_PWMServoDriver _pwm;
    int angleToPulse(float angle);
};

#endif // SERVO_CONTROLLER_H
