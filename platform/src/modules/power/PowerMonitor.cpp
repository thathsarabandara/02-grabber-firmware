#include "PowerMonitor.h"

PowerMonitor powerMonitor;

PowerMonitor::PowerMonitor() : _ina(INA226_ADDR), _voltage(0), _current_mA(0), _power_mW(0),
                               _peakCurrent(0), _idleCurrent(0), _movingCurrent(0),
                               _energy_mWh(0), _lastUpdateMs(0), _inaFound(false) {
}

void PowerMonitor::begin() {
    Serial.println("Scanning I2C bus...");
    byte error, address;
    int nDevices = 0;
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.printf("I2C device found at address 0x%02X\n", address);
            nDevices++;
        } else if (error == 4) {
            Serial.printf("Unknown error at address 0x%02X\n", address);
        }
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found! Check connections.");
    } else {
        Serial.printf("I2C scan complete. Found %d device(s).\n", nDevices);
    }

    Serial.println("Initializing INA226 Power Monitor...");
    if (!_ina.init()) {
        Serial.println("Failed to find INA226! Check wiring and I2C address.");
        _inaFound = false;
        return;
    }
    
    _inaFound = true;
    _ina.waitUntilConversionCompleted(); // Wait for first conversion
    _lastUpdateMs = millis();
    Serial.println("INA226 Initialized.");
}

void PowerMonitor::update(bool isMoving) {
    if (!_inaFound) return;
    
    unsigned long now = millis();
    unsigned long dt = now - _lastUpdateMs;
    
    // Read sensor
    _ina.readAndClearFlags();
    _voltage = _ina.getBusVoltage_V();
    _current_mA = _ina.getCurrent_mA();
    _power_mW = _ina.getBusPower();
    
    // Update Peak Current
    if (_current_mA > _peakCurrent) {
        _peakCurrent = _current_mA;
    }
    
    // Filter idle/moving currents (simple exponential moving average)
    if (isMoving) {
        if (_movingCurrent == 0) _movingCurrent = _current_mA;
        _movingCurrent = (_movingCurrent * 0.9f) + (_current_mA * 0.1f);
    } else {
        if (_idleCurrent == 0) _idleCurrent = _current_mA;
        _idleCurrent = (_idleCurrent * 0.9f) + (_current_mA * 0.1f);
    }
    
    // Integrate power over time (Energy in mWh)
    // power_mW * hours
    float hours = dt / 3600000.0f;
    _energy_mWh += (_power_mW * hours);
    
    _lastUpdateMs = now;
}

float PowerMonitor::getVoltage() const { return _voltage; }
float PowerMonitor::getCurrent() const { return _current_mA; }
float PowerMonitor::getPower() const { return _power_mW; }
float PowerMonitor::getPeakCurrent() const { return _peakCurrent; }
float PowerMonitor::getIdleCurrent() const { return _idleCurrent; }
float PowerMonitor::getMovingCurrent() const { return _movingCurrent; }
float PowerMonitor::getEnergyWh() const { return _energy_mWh / 1000.0f; }

float PowerMonitor::getRemainingCapacityPercent() const {
    // Simple linear approximation based on voltage for a 2S LiPo
    if (_voltage >= BATTERY_VOLTAGE_MAX) return 100.0f;
    if (_voltage <= BATTERY_VOLTAGE_MIN) return 0.0f;
    
    float percentage = ((_voltage - BATTERY_VOLTAGE_MIN) / (BATTERY_VOLTAGE_MAX - BATTERY_VOLTAGE_MIN)) * 100.0f;
    return percentage;
}

float PowerMonitor::getRuntimePredictionMins() const {
    if (_current_mA <= 0) return 999.0f; // Infinite
    
    float remainingCapacity_mAh = (getRemainingCapacityPercent() / 100.0f) * BATTERY_CAPACITY_MAH;
    
    // Use an average between current draw and recent moving current for a realistic estimate
    float avgCurrent = (_current_mA + _movingCurrent) / 2.0f;
    if (avgCurrent <= 0) return 999.0f;
    
    float hours = remainingCapacity_mAh / avgCurrent;
    return hours * 60.0f;
}

bool PowerMonitor::isDetected() const {
    return _inaFound;
}
