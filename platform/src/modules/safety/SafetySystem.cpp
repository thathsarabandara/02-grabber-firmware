#include "SafetySystem.h"

float SafetySystem::calculateSafeSpeed(
    float currentAngle,
    float minLimit,
    float maxLimit,
    float direction,
    float maxSpeed
) {
    float distanceToMin = currentAngle - minLimit;
    float distanceToMax = maxLimit - currentAngle;

    float safeSpeed = maxSpeed;

    // Slow down near minimum limit
    if (direction < 0 && distanceToMin < 15) {
        safeSpeed *= (distanceToMin / 15.0f);
    }

    // Slow down near maximum limit
    if (direction > 0 && distanceToMax < 15) {
        safeSpeed *= (distanceToMax / 15.0f);
    }

    // Ensure we don't completely stall and keep within bounds
    return constrain(safeSpeed, 0.05f, maxSpeed);
}
