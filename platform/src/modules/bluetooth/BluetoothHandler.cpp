#include "BluetoothHandler.h"

BluetoothHandler::BluetoothHandler(MotionManager& motion) 
    : _motion(motion), _deviceConnected(false), _pServer(nullptr), _pCharacteristic(nullptr) {
}

void BluetoothHandler::begin(const char* deviceName) {
    // 1. Initialize BLE Environment
    BLEDevice::init(deviceName);

    // 2. Create BLE Server
    _pServer = BLEDevice::createServer();
    _pServer->setCallbacks(this);

    // 3. Create BLE Service
    BLEService *pService = _pServer->createService(SERVICE_UUID);

    // 4. Create BLE Characteristic
    _pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_WRITE_NR |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );

    _pCharacteristic->setCallbacks(this);
    
    // Add descriptor for notifications
    _pCharacteristic->addDescriptor(new BLE2902());

    // 5. Start the service
    pService->start();

    // 6. Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    Serial.println("BLE System Started. Waiting for connections...");
}

bool BluetoothHandler::isConnected() const {
    return _deviceConnected;
}

void BluetoothHandler::onConnect(BLEServer* pServer) {
    _deviceConnected = true;
    Serial.println("BLE Client Connected.");
}

void BluetoothHandler::onDisconnect(BLEServer* pServer) {
    _deviceConnected = false;
    Serial.println("BLE Client Disconnected.");
    // Restart advertising when disconnected
    BLEDevice::startAdvertising();
}

void BluetoothHandler::onWrite(BLECharacteristic* pCharacteristic) {
    String value = pCharacteristic->getValue().c_str();

    if (value.length() > 0) {
        Serial.print("BLE Received: ");
        Serial.println(value);

        // Protocol expected: J:<servo_index>:<angle>\n
        // Examples:
        // J:0:90  -> Base to 90
        // J:3:70  -> Gripper to 70
        
        int firstColon = value.indexOf(':');
        int secondColon = value.indexOf(':', firstColon + 1);

        if (firstColon != -1 && secondColon != -1 && value.startsWith("J")) {
            String indexStr = value.substring(firstColon + 1, secondColon);
            String angleStr = value.substring(secondColon + 1);
            
            int servoIndex = indexStr.toInt();
            float targetAngle = angleStr.toFloat();

            if (servoIndex >= 0 && servoIndex < 4) {
                // Ensure angle is within valid ranges, although MotionManager might handle it
                _motion.setTarget(servoIndex, targetAngle);
                Serial.printf("Setting Servo %d to %.1f\n", servoIndex, targetAngle);
            } else {
                Serial.println("Error: Invalid servo index received via BLE.");
            }
        } else if (value.startsWith("A:")) {
            // Protocol expected: A:<a0>:<a1>:<a2>:<a3>\n
            int c1 = value.indexOf(':', 2);
            int c2 = value.indexOf(':', c1 + 1);
            int c3 = value.indexOf(':', c2 + 1);

            if (c1 != -1 && c2 != -1 && c3 != -1) {
                float a0 = value.substring(2, c1).toFloat();
                float a1 = value.substring(c1 + 1, c2).toFloat();
                float a2 = value.substring(c2 + 1, c3).toFloat();
                float a3 = value.substring(c3 + 1).toFloat();

                _motion.setTarget(0, a0);
                _motion.setTarget(1, a1);
                _motion.setTarget(2, a2);
                _motion.setTarget(3, a3);
            }
        }
    }
}
