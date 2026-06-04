#ifndef SAFETY_SYSTEM_H
#define SAFETY_SYSTEM_H

#include "../../../include/Config.h"

class SafetySystem {
public:
    static float calculateSafeSpeed(
        float currentAngle,
        float minLimit,
        float maxLimit,
        float direction,
        float maxSpeed
    );
};

#endif // SAFETY_SYSTEM_H
