#ifndef ROBOT_NETWORK_H
#define ROBOT_NETWORK_H

#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "../../../include/Config.h"
#include "../motion/MotionManager.h"

enum RobotState {
    OFFLINE,
    IDLE,
    MOVING,
    EXECUTING,
    ERROR_STATE,
    EMERGENCY_STOP
};

class RobotNetwork {
public:
    RobotNetwork(MotionManager& motion);
    
    void begin();
    void update();
    bool isConnected();
    
    void transitionToState(RobotState newState);
    void triggerError(const char* errorCode);

private:
    MotionManager& _motion;
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;
    
    unsigned long _lastReconnectAttempt;
    unsigned long _lastHeartbeat;
    unsigned long _lastTelemetry;
    RobotState _currentState;
    
    void setupWiFi();
    void registerRobot();
    void connectMQTT();
    void publishStatus(const char* status);
    void publishHeartbeat();
    void publishTelemetry();
    const char* stateToString(RobotState state) const;
    
    void handleMessage(char* topic, byte* payload, unsigned int length);
    static void mqttCallbackWrapper(char* topic, byte* payload, unsigned int length);
    static RobotNetwork* _instance;
};

#endif // ROBOT_NETWORK_H
