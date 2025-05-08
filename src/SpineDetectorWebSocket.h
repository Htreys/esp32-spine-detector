#ifndef SPINE_DETECTOR_WEBSOCKET_H
#define SPINE_DETECTOR_WEBSOCKET_H

#include <Arduino.h>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "config.h"

// 状态定义
enum SpineDetectorStatus {
    STATUS_CONNECTED,
    STATUS_CALIBRATION_STARTED,
    STATUS_CALIBRATION_SUCCESS,
    STATUS_CALIBRATION_FAILED,
    STATUS_TEST_STARTED,
    STATUS_TEST_COMPLETE
};

// 评估结果定义
enum AssessmentResult {
    ASSESSMENT_NORMAL,     // 角度 <= 5
    ASSESSMENT_MILD,       // 角度 > 5 且 <= 7
    ASSESSMENT_MODERATE    // 角度 > 7
};

class SpineDetectorWebSocket {
public:
    SpineDetectorWebSocket(uint16_t port = WEBSOCKET_PORT);
    ~SpineDetectorWebSocket();

    // 初始化WebSocket服务器
    bool begin(const char* ssid, const char* password);
    
    // 处理WebSocket事件
    void loop();
    
    // 发送状态消息
    void sendStatus(SpineDetectorStatus status);
    
    // 发送角度数据
    void sendAngleData(int position, float angle);
    
    // 发送测试完成数据
    void sendTestComplete(float maxAngle);
    
    // 发送设备信息
    void sendDeviceInfo();
    
    // 发送心跳包
    void sendHeartbeat();
    
    // 发送错误消息
    void sendError(const char* errorCode, const char* errorMessage);
    
    // 发送日志消息
    void sendLog(const char* logType, const char* message);

    // 获取WiFi状态
    bool isWiFiConnected() { return _wifiConnected; }

private:
    WebSocketsServer _webSocket;
    bool _wifiConnected;
    unsigned long _lastHeartbeat;
    
    // WebSocket事件处理函数
    static void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    
    // 将状态枚举转换为字符串
    const char* statusToString(SpineDetectorStatus status);
    
    // 将评估结果枚举转换为字符串
    const char* assessmentToString(AssessmentResult assessment);
    
    // 获取建议
    const char* getRecommendation(float angle);
    
    // 获取评估结果
    AssessmentResult getAssessment(float angle);
    
    // 静态实例指针，用于在回调函数中访问类实例
    static SpineDetectorWebSocket* _instance;
};

#endif // SPINE_DETECTOR_WEBSOCKET_H 