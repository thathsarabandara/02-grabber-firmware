#ifndef BLUETOOTH_HANDLER_H
#define BLUETOOTH_HANDLER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "../motion/MotionManager.h"

// Define Service and Characteristic UUIDs
// Using standard ones or custom ones. Let's use custom ones for this project.
#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class BluetoothHandler : public BLEServerCallbacks, public BLECharacteristicCallbacks {
public:
    BluetoothHandler(MotionManager& motion);
    
    void begin(const char* deviceName = "Grabber_BLE");
    bool isConnected() const;

    // BLEServerCallbacks
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;

    // BLECharacteristicCallbacks
    void onWrite(BLECharacteristic* pCharacteristic) override;

private:
    MotionManager& _motion;
    bool _deviceConnected;
    BLEServer* _pServer;
    BLECharacteristic* _pCharacteristic;
};

#endif // BLUETOOTH_HANDLER_H
