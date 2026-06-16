#include "RobotNetwork.h"

RobotNetwork* RobotNetwork::_instance = nullptr;

RobotNetwork::RobotNetwork(MotionManager& motion) 
    : _motion(motion), _mqttClient(_wifiClient), _lastReconnectAttempt(0), _lastHeartbeat(0), _currentState(OFFLINE) {
    _instance = this;
}

void RobotNetwork::begin() {
    setupWiFi();
    if (WiFi.status() == WL_CONNECTED) {
        registerRobot();
        _mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
        _mqttClient.setCallback(RobotNetwork::mqttCallbackWrapper);
        connectMQTT();
    }
}

void RobotNetwork::setupWiFi() {
    Serial.println("Initializing WiFi...");
    
    // Use WiFiManager for dynamic provisioning
    WiFiManager wifiManager;
    
    // Set a timeout so it doesn't block forever if there's no input
    wifiManager.setTimeout(120);
    
    // Try to connect, if it fails after 120s timeout, it falls back to below logic
    if (!wifiManager.autoConnect("Grabber-AP", "password123")) {
        Serial.println("WiFiManager failed to connect or hit timeout.");
        Serial.println("Attempting fallback WiFi...");
        
        WiFi.mode(WIFI_STA);
        WiFi.begin(FALLBACK_WIFI_SSID, FALLBACK_WIFI_PASSWORD);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nFallback WiFi connected!");
        } else {
            Serial.println("\nFallback WiFi failed.");
        }
    } else {
        Serial.println("\nWiFi connected via WiFiManager!");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
}

void RobotNetwork::registerRobot() {
    Serial.println("Registering robot with API Gateway...");
    HTTPClient http;
    
    http.begin(API_REGISTER_URL);
    http.addHeader("Content-Type", "application/json");
    
    StaticJsonDocument<200> doc;
    doc["robot_id"] = ROBOT_ID;
    doc["serial_key"] = SERIAL_KEY;
    doc["name"] = "Grabber Arm 1";
    doc["model"] = "ESP32-PCA9685";
    doc["firmware_version"] = "1.0.0";
    
    String payload;
    serializeJson(doc, payload);
    
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0) {
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        String response = http.getString();
        Serial.println(response);
    } else {
        Serial.printf("Error code: %d\n", httpResponseCode);
    }
    
    http.end();
}

void RobotNetwork::connectMQTT() {
    Serial.print("Connecting to MQTT...");
    
    String clientId = "GrabberClient-";
    clientId += String(random(0xffff), HEX);
    
    String statusTopic = String("robot/") + ROBOT_ID + "/status";
    
    // Last Will and Testament: If disconnects, publish "OFFLINE"
    if (_mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS, statusTopic.c_str(), 1, true, "{\"status\":\"OFFLINE\"}")) {
        Serial.println("connected!");
        
        // Transition to IDLE state which will publish ONLINE/IDLE status
        transitionToState(IDLE);
        
        // Subscribe to commands
        String moveTopic = String("robot/") + ROBOT_ID + "/commands/move";
        String estopTopic = String("robot/") + ROBOT_ID + "/commands/estop";
        String clearEstopTopic = String("robot/") + ROBOT_ID + "/commands/clear_estop";
        String homeTopic = String("robot/") + ROBOT_ID + "/commands/home";
        String openGripperTopic = String("robot/") + ROBOT_ID + "/commands/open-gripper";
        String closeGripperTopic = String("robot/") + ROBOT_ID + "/commands/close-gripper";
        
        _mqttClient.subscribe(moveTopic.c_str());
        _mqttClient.subscribe(estopTopic.c_str());
        _mqttClient.subscribe(clearEstopTopic.c_str());
        _mqttClient.subscribe(homeTopic.c_str());
        _mqttClient.subscribe(openGripperTopic.c_str());
        _mqttClient.subscribe(closeGripperTopic.c_str());
        
        Serial.println("Subscribed to command topics.");
    } else {
        Serial.print("failed, rc=");
        Serial.print(_mqttClient.state());
        Serial.println(" try again in 5 seconds");
    }
}

void RobotNetwork::publishStatus(const char* status) {
    if (_mqttClient.connected()) {
        String statusTopic = String("robot/") + ROBOT_ID + "/status";
        _mqttClient.publish(statusTopic.c_str(), status, true);
    }
}

void RobotNetwork::publishHeartbeat() {
    if (_mqttClient.connected()) {
        String heartbeatTopic = String("robot/") + ROBOT_ID + "/heartbeat";
        StaticJsonDocument<256> doc;
        doc["robotId"] = ROBOT_ID;
        doc["timestamp"] = 0; // Server overrides with current epoch
        doc["state"] = stateToString(_currentState);
        doc["firmware"] = "1.0.0";
        
        String payload;
        serializeJson(doc, payload);
        _mqttClient.publish(heartbeatTopic.c_str(), payload.c_str());
    }
}

void RobotNetwork::transitionToState(RobotState newState) {
    if (_currentState == newState) return;
    
    _currentState = newState;
    
    // Setup emergency stop lock in MotionManager
    if (_currentState == EMERGENCY_STOP) {
        _motion.setEmergencyStop(true);
    } else if (_currentState == IDLE || _currentState == MOVING) {
        _motion.setEmergencyStop(false);
    }
    
    StaticJsonDocument<128> doc;
    doc["state"] = stateToString(_currentState);
    
    String payload;
    serializeJson(doc, payload);
    publishStatus(payload.c_str());
    
    Serial.printf("State Transition -> %s\n", stateToString(_currentState));
}

void RobotNetwork::triggerError(const char* errorCode) {
    _currentState = ERROR_STATE;
    _motion.setEmergencyStop(true);
    
    StaticJsonDocument<128> doc;
    doc["state"] = "ERROR";
    doc["errorCode"] = errorCode;
    
    String payload;
    serializeJson(doc, payload);
    publishStatus(payload.c_str());
    
    String errorTopic = String("robot/") + ROBOT_ID + "/errors";
    _mqttClient.publish(errorTopic.c_str(), payload.c_str(), true);
    
    Serial.printf("Error Triggered: %s\n", errorCode);
}

const char* RobotNetwork::stateToString(RobotState state) const {
    switch (state) {
        case OFFLINE: return "OFFLINE";
        case IDLE: return "IDLE";
        case MOVING: return "MOVING";
        case EXECUTING: return "EXECUTING";
        case ERROR_STATE: return "ERROR";
        case EMERGENCY_STOP: return "EMERGENCY_STOP";
        default: return "UNKNOWN";
    }
}

void RobotNetwork::update() {
    if (WiFi.status() != WL_CONNECTED) {
        _currentState = OFFLINE;
        return; // Don't try MQTT if no WiFi
    }
    
    if (!_mqttClient.connected()) {
        _currentState = OFFLINE;
        unsigned long now = millis();
        if (now - _lastReconnectAttempt > 5000) {
            _lastReconnectAttempt = now;
            connectMQTT();
        }
    } else {
        _mqttClient.loop();
        
        // Heartbeat periodic loop (every 5 seconds)
        unsigned long now = millis();
        if (now - _lastHeartbeat > 5000) {
            _lastHeartbeat = now;
            publishHeartbeat();
        }
        
        // Auto transition IDLE/MOVING based on physical activity
        if (_currentState == IDLE && _motion.isMoving()) {
            transitionToState(MOVING);
        } else if (_currentState == MOVING && !_motion.isMoving()) {
            transitionToState(IDLE);
        }
    }
}

bool RobotNetwork::isConnected() {
    return _mqttClient.connected();
}

void RobotNetwork::handleMessage(char* topic, byte* payload, unsigned int length) {
    String topicStr(topic);
    
    if (topicStr.endsWith("/commands/estop")) {
        transitionToState(EMERGENCY_STOP);
        return;
    }
    
    if (topicStr.endsWith("/commands/clear_estop")) {
        transitionToState(IDLE);
        return;
    }
    
    if (topicStr.endsWith("/commands/home")) {
        if (_currentState == EMERGENCY_STOP || _currentState == ERROR_STATE) {
            Serial.println("MQTT Home command ignored: safety/error lock active.");
            return;
        }
        _motion.setTarget(0, BASE_HOME);
        _motion.setTarget(1, SHOULDER_HOME);
        _motion.setTarget(2, ELBOW_HOME);
        _motion.setTarget(3, GRIPPER_HOME);
        Serial.println("MQTT Command: Home position executed.");
        return;
    }
    
    if (topicStr.endsWith("/commands/open-gripper")) {
        if (_currentState == EMERGENCY_STOP || _currentState == ERROR_STATE) {
            Serial.println("MQTT Open Gripper command ignored: safety/error lock active.");
            return;
        }
        _motion.setTarget(3, GRIPPER_MAX);
        Serial.println("MQTT Command: Open Gripper executed.");
        return;
    }
    
    if (topicStr.endsWith("/commands/close-gripper")) {
        if (_currentState == EMERGENCY_STOP || _currentState == ERROR_STATE) {
            Serial.println("MQTT Close Gripper command ignored: safety/error lock active.");
            return;
        }
        _motion.setTarget(3, GRIPPER_MIN);
        Serial.println("MQTT Command: Close Gripper executed.");
        return;
    }
    
    if (topicStr.endsWith("/commands/move")) {
        if (_currentState == EMERGENCY_STOP || _currentState == ERROR_STATE) {
            Serial.println("MQTT Move command ignored: safety/error lock active.");
            return;
        }
        
        // Parse JSON: {"servo": 0, "angle": 90}
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, payload, length);
        
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        
        int servoIndex = doc["servo"]; // 0 to 3
        float angle = doc["angle"];
        
        // Apply bounds and update target
        if (servoIndex >= 0 && servoIndex <= 3) {
            _motion.setTarget(servoIndex, angle);
            Serial.printf("MQTT Command: Servo %d -> %.2f\n", servoIndex, angle);
        }
    }
}

void RobotNetwork::mqttCallbackWrapper(char* topic, byte* payload, unsigned int length) {
    if (_instance) {
        _instance->handleMessage(topic, payload, length);
    }
}
