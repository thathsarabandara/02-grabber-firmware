#ifndef ROBOT_NETWORK_H
#define ROBOT_NETWORK_H

#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "../../../include/Config.h"
#include "../motion/MotionManager.h"

class RobotNetwork {
public:
    RobotNetwork(MotionManager& motion);
    
    void begin();
    void update();
    bool isConnected();

private:
    MotionManager& _motion;
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;
    
    unsigned long _lastReconnectAttempt;
    
    void setupWiFi();
    void registerRobot();
    void connectMQTT();
    void publishStatus(const char* status);
    
    void handleMessage(char* topic, byte* payload, unsigned int length);
    static void mqttCallbackWrapper(char* topic, byte* payload, unsigned int length);
    static RobotNetwork* _instance;
};

#endif // ROBOT_NETWORK_H
