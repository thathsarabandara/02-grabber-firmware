#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

#include <Arduino.h>
#include <Wire.h>
#include <INA226_WE.h>
#include "../../../include/Config.h"

class PowerMonitor {
public:
    PowerMonitor();
    
    void begin();
    void update(bool isMoving);
    
    float getVoltage() const;
    float getCurrent() const;
    float getPower() const;
    float getPeakCurrent() const;
    float getIdleCurrent() const;
    float getMovingCurrent() const;
    float getEnergyWh() const;
    float getRemainingCapacityPercent() const;
    float getRuntimePredictionMins() const;

private:
    INA226_WE _ina;
    
    float _voltage;
    float _current_mA;
    float _power_mW;
    
    float _peakCurrent;
    float _idleCurrent;
    float _movingCurrent;
    
    float _energy_mWh;
    unsigned long _lastUpdateMs;
    
    bool _inaFound;
};

extern PowerMonitor powerMonitor;

#endif // POWER_MONITOR_H
