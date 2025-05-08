#include "SpineDetectorWebSocket.h"

// 初始化静态实例指针
SpineDetectorWebSocket* SpineDetectorWebSocket::_instance = nullptr;

// 构造函数
SpineDetectorWebSocket::SpineDetectorWebSocket(uint16_t port) 
    : _webSocket(port), _wifiConnected(false), _lastHeartbeat(0) {
    _instance = this;
}

// 析构函数
SpineDetectorWebSocket::~SpineDetectorWebSocket() {
    _instance = nullptr;
}

// 初始化WebSocket服务器
bool SpineDetectorWebSocket::begin(const char* ssid, const char* password) {
    // 连接WiFi
    WiFi.begin(ssid, password);
    
    // 等待WiFi连接
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi连接失败！");
        return false;
    }
    
    Serial.println("");
    Serial.print("WiFi已连接. IP地址: ");
    Serial.println(WiFi.localIP());
    
    // 初始化mDNS
    if (!MDNS.begin("spinedetector")) {
        Serial.println("启动mDNS失败!");
        return false;
    }
    
    // 启动WebSocket服务器
    _webSocket.begin();
    _webSocket.onEvent(webSocketEvent);
    
    Serial.println("WebSocket服务器已启动");
    _wifiConnected = true;
    
    return true;
}

// 处理WebSocket事件
void SpineDetectorWebSocket::loop() {
    _webSocket.loop();
    
    // 发送心跳包
    unsigned long currentMillis = millis();
    if (currentMillis - _lastHeartbeat > 10000) { // 每10秒发送一次心跳
        sendHeartbeat();
        _lastHeartbeat = currentMillis;
    }
}

// WebSocket事件处理函数
void SpineDetectorWebSocket::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (!_instance) return;
    
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] 客户端断开连接!\n", num);
            break;
            
        case WStype_CONNECTED:
            {
                IPAddress ip = _instance->_webSocket.remoteIP(num);
                Serial.printf("[%u] 客户端连接，IP: %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
                
                // 发送欢迎消息
                String welcomeMsg = "{\"type\":\"welcome\",\"message\":\"设备已连接\",\"device_id\":\"";
                welcomeMsg += DEVICE_ID;
                welcomeMsg += "\",\"firmware_version\":\"";
                welcomeMsg += FIRMWARE_VERSION;
                welcomeMsg += "\"}";
                
                _instance->_webSocket.sendTXT(num, welcomeMsg);
                
                // 发送状态消息
                _instance->sendStatus(STATUS_CONNECTED);
                
                // 发送设备信息
                _instance->sendDeviceInfo();
            }
            break;
            
        case WStype_TEXT:
            {
                Serial.printf("[%u] 收到文本: %s\n", num, payload);
                
                // 解析JSON
                StaticJsonDocument<200> doc;
                DeserializationError error = deserializeJson(doc, payload);
                
                if (error) {
                    Serial.print(F("deserializeJson() 失败: "));
                    Serial.println(error.f_str());
                    return;
                }
                
                // 处理connect命令
                const char* type = doc["type"];
                if (type && strcmp(type, "connect") == 0) {
                    // 客户端已发送连接命令，无需额外处理，因为已经在CONNECTED事件中处理了欢迎消息
                }
            }
            break;
    }
}

// 发送状态消息
void SpineDetectorWebSocket::sendStatus(SpineDetectorStatus status) {
    String statusMsg = "{\"type\":\"status\",\"status\":\"";
    statusMsg += statusToString(status);
    statusMsg += "\",\"device_id\":\"";
    statusMsg += DEVICE_ID;
    statusMsg += "\",\"timestamp\":";
    statusMsg += String(millis());
    statusMsg += "}";
    
    _webSocket.broadcastTXT(statusMsg);
}

// 发送角度数据
void SpineDetectorWebSocket::sendAngleData(int position, float angle) {
    String angleMsg = "{\"type\":\"angle_data\",\"position\":";
    angleMsg += String(position);
    angleMsg += ",\"angle\":";
    angleMsg += String(angle, 1); // 保留1位小数
    angleMsg += ",\"timestamp\":";
    angleMsg += String(millis());
    angleMsg += "}";
    
    _webSocket.broadcastTXT(angleMsg);
}

// 发送测试完成数据
void SpineDetectorWebSocket::sendTestComplete(float maxAngle) {
    AssessmentResult assessment = getAssessment(maxAngle);
    
    String testMsg = "{\"type\":\"test_complete\",\"max_angle\":";
    testMsg += String(maxAngle, 1); // 保留1位小数
    testMsg += ",\"device_id\":\"";
    testMsg += DEVICE_ID;
    testMsg += "\",\"timestamp\":";
    testMsg += String(millis());
    testMsg += ",\"assessment\":\"";
    testMsg += assessmentToString(assessment);
    testMsg += "\",\"recommendation\":\"";
    testMsg += getRecommendation(maxAngle);
    testMsg += "\"}";
    
    _webSocket.broadcastTXT(testMsg);
}

// 发送设备信息
void SpineDetectorWebSocket::sendDeviceInfo() {
    String infoMsg = "{\"type\":\"device_info\",\"device_id\":\"";
    infoMsg += DEVICE_ID;
    infoMsg += "\",\"firmware_version\":\"";
    infoMsg += FIRMWARE_VERSION;
    infoMsg += "\",\"ip_address\":\"";
    infoMsg += WiFi.localIP().toString();
    infoMsg += "\",\"mac_address\":\"";
    infoMsg += WiFi.macAddress();
    infoMsg += "\",\"uptime\":";
    infoMsg += String(millis() / 1000);
    infoMsg += ",\"free_heap\":";
    infoMsg += String(ESP.getFreeHeap());
    infoMsg += ",\"wifi_rssi\":";
    infoMsg += String(WiFi.RSSI());
    infoMsg += "}";
    
    _webSocket.broadcastTXT(infoMsg);
}

// 发送心跳包
void SpineDetectorWebSocket::sendHeartbeat() {
    String heartbeatMsg = "{\"type\":\"heartbeat\",\"device_id\":\"";
    heartbeatMsg += DEVICE_ID;
    heartbeatMsg += "\",\"timestamp\":";
    heartbeatMsg += String(millis());
    heartbeatMsg += ",\"free_heap\":";
    heartbeatMsg += String(ESP.getFreeHeap());
    heartbeatMsg += "}";
    
    _webSocket.broadcastTXT(heartbeatMsg);
}

// 发送错误消息
void SpineDetectorWebSocket::sendError(const char* errorCode, const char* errorMessage) {
    String errorMsg = "{\"type\":\"error\",\"error_code\":\"";
    errorMsg += errorCode;
    errorMsg += "\",\"error_message\":\"";
    errorMsg += errorMessage;
    errorMsg += "\",\"device_id\":\"";
    errorMsg += DEVICE_ID;
    errorMsg += "\",\"timestamp\":";
    errorMsg += String(millis());
    errorMsg += "}";
    
    _webSocket.broadcastTXT(errorMsg);
}

// 发送日志消息
void SpineDetectorWebSocket::sendLog(const char* logType, const char* message) {
    String logMsg = "{\"type\":\"log\",\"device_id\":\"";
    logMsg += DEVICE_ID;
    logMsg += "\",\"log_type\":\"";
    logMsg += logType;
    logMsg += "\",\"message\":\"";
    logMsg += message;
    logMsg += "\",\"timestamp\":";
    logMsg += String(millis());
    logMsg += "}";
    
    _webSocket.broadcastTXT(logMsg);
}

// 将状态枚举转换为字符串
const char* SpineDetectorWebSocket::statusToString(SpineDetectorStatus status) {
    switch (status) {
        case STATUS_CONNECTED:
            return "connected";
        case STATUS_CALIBRATION_STARTED:
            return "calibration_started";
        case STATUS_CALIBRATION_SUCCESS:
            return "calibration_success";
        case STATUS_CALIBRATION_FAILED:
            return "calibration_failed";
        case STATUS_TEST_STARTED:
            return "test_started";
        case STATUS_TEST_COMPLETE:
            return "test_complete";
        default:
            return "unknown";
    }
}

// 将评估结果枚举转换为字符串
const char* SpineDetectorWebSocket::assessmentToString(AssessmentResult assessment) {
    switch (assessment) {
        case ASSESSMENT_NORMAL:
            return "normal";
        case ASSESSMENT_MILD:
            return "mild";
        case ASSESSMENT_MODERATE:
            return "moderate";
        default:
            return "unknown";
    }
}

// 获取建议
const char* SpineDetectorWebSocket::getRecommendation(float angle) {
    if (angle <= 5.0f) {
        return "正常范围内";
    } else if (angle <= 7.0f) {
        return "建议进一步X光检查";
    } else {
        return "建议治疗介入";
    }
}

// 获取评估结果
AssessmentResult SpineDetectorWebSocket::getAssessment(float angle) {
    if (angle <= 5.0f) {
        return ASSESSMENT_NORMAL;
    } else if (angle <= 7.0f) {
        return ASSESSMENT_MILD;
    } else {
        return ASSESSMENT_MODERATE;
    }
} 