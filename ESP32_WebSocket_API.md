# ESP32脊柱曲度检测仪 WebSocket通信接口文档（修订版）

## 概述

本文档详细描述了脊柱曲度检测仪ESP32设备与后台管理系统之间的WebSocket通信接口协议。请注意，**检测仪是独立的物理设备，用户直接在设备上操作**，后台系统主要用于**接收数据**而非控制设备。通过此接口，后台系统可以接收设备发送的测量数据和状态信息，以便记录和管理。

## 连接信息

- **WebSocket服务地址**: `ws://spinedetector.local:81` 
- **协议**: WebSocket (RFC 6455)
- **数据格式**: JSON
- **设备发现方式**: mDNS (设备名称为"spinedetector.local")

## 通信模型说明

在本系统中:
1. 检测仪是独立的物理设备，所有操作（校准、测试等）均由用户在设备上直接完成
2. 后台管理系统作为WebSocket客户端连接到设备
3. 设备不识别当前操作它的用户身份，仅发送测量数据
4. 后台系统接收数据后，根据当前登录的用户信息将数据关联并存储到数据库
5. **每次测试包含5组角度数据**，设备会依次发送5条角度数据，然后发送一条汇总的测试结果

## 后台系统向设备发送的命令

后台系统向设备发送的命令非常有限，主要是建立连接:

### 连接命令

建立与设备的WebSocket连接。

```json
{
  "type": "connect"
}
```

| 字段 | 类型 | 必填 | 描述 |
|-----|-----|-----|-----|
| type | string | 是 | 命令类型，固定为 "connect" |

> **注意**: 设备不识别具体的用户，用户标识由后台系统管理。之前文档中的user_id参数不需要发送到设备。

## 设备向后台系统发送的消息

设备会向后台系统发送以下类型的消息：

### 1. 欢迎消息

当WebSocket连接建立时发送的欢迎消息。

```json
{
  "type": "welcome",
  "message": "设备已连接",
  "device_id": "SpineDetector",
  "firmware_version": "1.0.0"
}
```

| 字段 | 类型 | 描述 |
|-----|-----|-----|
| type | string | 消息类型，固定为 "welcome" |
| message | string | 欢迎信息 |
| device_id | string | 设备ID |
| firmware_version | string | 固件版本号 |

### 2. 状态消息

反映设备当前状态的消息，当设备上发生状态变化时自动发送。

```json
{
  "type": "status",
  "status": "connected",
  "device_id": "SpineDetector",
  "timestamp": 1684569823000
}
```

| 字段 | 类型 | 描述 |
|-----|-----|-----|
| type | string | 消息类型，固定为 "status" |
| status | string | 设备状态，可能的值：<br>- "connected": 已连接<br>- "calibration_started": 开始校准<br>- "calibration_success": 校准成功<br>- "calibration_failed": 校准失败<br>- "test_started": 测试开始<br>- "test_complete": 测试完成 |
| device_id | string | 设备ID |
| timestamp | number | 时间戳(毫秒) |

### 3. 角度数据消息

当用户在设备上进行测试时，设备会**依次发送5组测量数据**。每个测量位置发送一条消息，共5个位置。

```json
{
  "type": "angle_data",
  "position": 1,
  "angle": 23.5,
  "timestamp": 1684569823000
}
```

| 字段 | 类型 | 描述 |
|-----|-----|-----|
| type | string | 消息类型，固定为 "angle_data" |
| position | number | 测量位置编号（1-5） |
| angle | number | 测量的角度值 |
| timestamp | number | 时间戳(毫秒) |

### 4. 测试完成消息

用户在设备上完成5个位置的测试后，设备会自动发送测试结果。结果包含最大角度值以及脊柱侧弯程度评估。

```json
{
  "type": "test_complete",
  "max_angle": 25.7,
  "device_id": "SpineDetector",
  "timestamp": 1684569823000,
  "assessment": "moderate",
  "recommendation": "建议进一步X光检查"
}
```

| 字段 | 类型 | 描述 |
|-----|-----|-----|
| type | string | 消息类型，固定为 "test_complete" |
| max_angle | number | 测试过程中测得的最大角度 |
| device_id | string | 设备ID |
| timestamp | number | 时间戳(毫秒) |
| assessment | string | 脊柱侧弯程度评估:<br>- "normal": 正常 (≤5度)<br>- "mild": 轻度 (>5度且≤7度)<br>- "moderate": 中度 (>7度) |
| recommendation | string | 基于角度的建议:<br>- 角度≤5度: "正常范围内"<br>- 角度>5度且≤7度: "建议进一步X光检查"<br>- 角度>7度: "建议治疗介入" |

> **注意**: 设备发送的数据不包含用户信息，后台系统需要根据当前登录用户关联数据。

### 5. 设备信息消息

包含设备详细信息的消息。

```json
{
  "type": "device_info",
  "device_id": "SpineDetector",
  "firmware_version": "1.0.0",
  "ip_address": "192.168.1.100",
  "mac_address": "AA:BB:CC:DD:EE:FF",
  "uptime": 3600,
  "free_heap": 123456,
  "wifi_rssi": -65
}
```

| 字段 | 类型 | 描述 |
|-----|-----|-----|
| type | string | 消息类型，固定为 "device_info" |
| device_id | string | 设备ID |
| firmware_version | string | 固件版本 |
| ip_address | string | IP地址 |
| mac_address | string | MAC地址 |
| uptime | number | 设备运行时间(秒) |
| free_heap | number | 可用内存(字节) |
| wifi_rssi | number | WiFi信号强度(dBm) |

### 6. 心跳消息

设备定期发送的心跳包，用于保持连接活跃。

```json
{
  "type": "heartbeat",
  "device_id": "SpineDetector",
  "timestamp": 1684569823000,
  "free_heap": 123456
}
```

| 字段 | 类型 | 描述 |
|-----|-----|-----|
| type | string | 消息类型，固定为 "heartbeat" |
| device_id | string | 设备ID |
| timestamp | number | 时间戳(毫秒) |
| free_heap | number | 可用内存(字节) |

### 7. 错误消息

设备遇到错误时发送的消息。

```json
{
  "type": "error",
  "error_code": "E1001",
  "error_message": "传感器读数异常",
  "device_id": "SpineDetector",
  "timestamp": 1684569823000
}
```

| 字段 | 类型 | 描述 |
|-----|-----|-----|
| type | string | 消息类型，固定为 "error" |
| error_code | string | 错误代码 |
| error_message | string | 错误描述 |
| device_id | string | 设备ID |
| timestamp | number | 时间戳(毫秒) |

### 8. 日志消息

设备发送的日志条目，如校准完成等操作日志。

```json
{
  "type": "log",
  "device_id": "SpineDetector",
  "log_type": "info",
  "message": "设备已校准完成",
  "timestamp": 1684569823000
}
```

| 字段 | 类型 | 描述 |
|-----|-----|-----|
| type | string | 消息类型，固定为 "log" |
| device_id | string | 设备ID |
| log_type | string | 日志类型: "info", "warning", "error" |
| message | string | 日志内容 |
| timestamp | number | 时间戳(毫秒) |

## 通信流程示例

以下是实际应用场景中的通信流程示例：

### 示例1: 连接建立流程

```
// 后台系统发送
{
  "type": "connect"
}

// 设备回应
{
  "type": "welcome",
  "message": "设备已连接",
  "device_id": "SpineDetector",
  "firmware_version": "1.0.0"
}
{
  "type": "status",
  "status": "connected",
  "device_id": "SpineDetector",
  "timestamp": 1684569823000
}
{
  "type": "device_info",
  "device_id": "SpineDetector",
  "firmware_version": "1.0.0",
  "ip_address": "192.168.1.100",
  "mac_address": "AA:BB:CC:DD:EE:FF",
  "uptime": 3600,
  "free_heap": 123456,
  "wifi_rssi": -65
}
```

### 示例2: 用户在设备上进行校准

当用户在物理设备上执行校准操作时，设备会自动发送状态消息：

```
// 用户在设备上开始校准 (无需后台系统发送命令)

// 设备自动发送状态
{
  "type": "status",
  "status": "calibration_started",
  "device_id": "SpineDetector",
  "timestamp": 1684569823000
}

// 校准完成后自动发送
{
  "type": "status",
  "status": "calibration_success",
  "device_id": "SpineDetector",
  "timestamp": 1684569828000
}

// 发送日志
{
  "type": "log",
  "device_id": "SpineDetector",
  "log_type": "info",
  "message": "设备已校准完成",
  "timestamp": 1684569828500
}
```

### 示例3: 用户在设备上进行测试

当用户在物理设备上进行测试时，设备会自动发送5组测量数据，然后发送最终结果：

```
// 用户在设备上开始测试 (无需后台系统发送命令)

// 设备自动发送状态
{
  "type": "status",
  "status": "test_started",
  "device_id": "SpineDetector",
  "timestamp": 1684569823000
}

// 设备自动发送5组测量数据
{
  "type": "angle_data",
  "position": 1,
  "angle": 4.2,
  "timestamp": 1684569825000
}
{
  "type": "angle_data",
  "position": 2,
  "angle": 5.7,
  "timestamp": 1684569827000
}
{
  "type": "angle_data",
  "position": 3,
  "angle": 6.5,
  "timestamp": 1684569829000
}
{
  "type": "angle_data",
  "position": 4,
  "angle": 7.8,
  "timestamp": 1684569831000
}
{
  "type": "angle_data",
  "position": 5,
  "angle": 6.2,
  "timestamp": 1684569833000
}

// 所有5个位置测量完成后，发送最终结果
{
  "type": "test_complete",
  "max_angle": 7.8,
  "device_id": "SpineDetector",
  "timestamp": 1684569835000,
  "assessment": "moderate",
  "recommendation": "建议治疗介入"
}
```

## 后台系统实现指南

### 数据处理流程

1. **连接管理**:
   - 后台系统作为WebSocket客户端连接到设备
   - 无需发送用户信息到设备

2. **数据接收**:
   - 接收设备发送的所有消息
   - 对接收到的数据进行验证与解析
   - **累计接收5组角度数据，再接收最终结果**

3. **数据关联**:
   - 将接收到的测量数据与当前登录的用户关联
   - 在数据库中保存关联后的记录
   - **保存脊柱侧弯程度评估和建议**

4. **状态监控**:
   - 监控设备状态变化
   - 在界面上更新设备状态

### JavaScript客户端实现示例

```javascript
class SpineDetectorClient {
  constructor(url = 'ws://spinedetector.local:81') {
    this.url = url;
    this.ws = null;
    this.connected = false;
    this.reconnectAttempts = 0;
    this.eventListeners = {};
    
    // 当前登录用户信息 (从系统获取)
    this.currentUser = {
      id: null,
      name: null
    };
    
    // 当前测试的角度数据集合
    this.currentTestData = {
      angles: [],
      startTime: null
    };
  }

  // 设置当前用户信息
  setCurrentUser(userId, userName) {
    this.currentUser.id = userId;
    this.currentUser.name = userName;
    console.log(`当前用户已设置为: ${userName} (ID: ${userId})`);
  }

  connect() {
    return new Promise((resolve, reject) => {
      this.ws = new WebSocket(this.url);
      
      this.ws.onopen = () => {
        console.log('WebSocket连接已建立');
        // 发送连接命令 (不需要用户ID)
        this.sendCommand({
          type: 'connect'
        });
        
        this.connected = true;
        this.reconnectAttempts = 0;
        resolve();
      };
      
      this.ws.onmessage = (event) => {
        const data = JSON.parse(event.data);
        console.log('收到消息:', data);
        
        // 处理测试状态和数据
        if (data.type === 'status' && data.status === 'test_started') {
          // 新测试开始，重置数据集合
          this.currentTestData = {
            angles: [],
            startTime: data.timestamp
          };
        } else if (data.type === 'angle_data') {
          // 收集角度数据
          this.currentTestData.angles.push({
            position: data.position,
            angle: data.angle,
            timestamp: data.timestamp
          });
        } else if (data.type === 'test_complete') {
          // 添加用户信息和收集的角度数据
          data.user_id = this.currentUser.id;
          data.user_name = this.currentUser.name;
          data.angles = this.currentTestData.angles;
          data.startTime = this.currentTestData.startTime;
          
          // 根据最大角度给出评估和建议
          if (!data.assessment) {
            if (data.max_angle <= 5) {
              data.assessment = "normal";
              data.recommendation = "正常范围内";
            } else if (data.max_angle <= 7) {
              data.assessment = "mild";
              data.recommendation = "建议进一步X光检查";
            } else {
              data.assessment = "moderate";
              data.recommendation = "建议治疗介入";
            }
          }
          
          // 保存完整测试结果
          this.saveTestResult(data);
          
          // 重置测试数据
          this.currentTestData = {
            angles: [],
            startTime: null
          };
        }
        
        // 触发对应类型的事件
        if (this.eventListeners[data.type]) {
          this.eventListeners[data.type].forEach(callback => callback(data));
        }
        
        // 触发所有消息的事件
        if (this.eventListeners['message']) {
          this.eventListeners['message'].forEach(callback => callback(data));
        }
      };
      
      this.ws.onerror = (error) => {
        console.error('WebSocket错误:', error);
        reject(error);
      };
      
      this.ws.onclose = () => {
        console.log('WebSocket连接已关闭');
        this.connected = false;
        
        // 尝试重新连接
        if (this.reconnectAttempts < 5) {
          this.reconnectAttempts++;
          setTimeout(() => this.connect(), 5000);
        }
      };
    });
  }
  
  on(eventType, callback) {
    if (!this.eventListeners[eventType]) {
      this.eventListeners[eventType] = [];
    }
    this.eventListeners[eventType].push(callback);
  }
  
  off(eventType, callback) {
    if (this.eventListeners[eventType]) {
      this.eventListeners[eventType] = this.eventListeners[eventType].filter(cb => cb !== callback);
    }
  }
  
  sendCommand(command) {
    if (!this.connected) {
      throw new Error('WebSocket未连接');
    }
    this.ws.send(JSON.stringify(command));
  }
  
  // 保存测试结果到后台系统
  saveTestResult(data) {
    // 添加时间戳和用户信息
    const result = {
      ...data,
      saved_at: new Date().toISOString(),
      user_id: this.currentUser.id,
      user_name: this.currentUser.name
    };
    
    // 调用API保存数据
    fetch('/api/test-results', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(result),
    })
    .then(response => response.json())
    .then(data => {
      console.log('测试结果已保存:', data);
    })
    .catch((error) => {
      console.error('保存测试结果失败:', error);
    });
  }
  
  disconnect() {
    if (this.ws) {
      this.ws.close();
    }
  }
}

// 使用示例
const client = new SpineDetectorClient();

// 设置当前登录用户 (从系统获取)
client.setCurrentUser('USER123', '张三');

// 监听欢迎消息
client.on('welcome', (data) => {
  console.log('设备欢迎消息:', data);
  document.getElementById('device-status').textContent = '已连接';
  document.getElementById('device-info').textContent = `设备ID: ${data.device_id}, 固件版本: ${data.firmware_version}`;
});

// 监听状态变化
client.on('status', (data) => {
  console.log('设备状态:', data.status);
  document.getElementById('device-status').textContent = data.status;
  
  // 显示状态变化提示
  showNotification(`设备状态: ${getStatusDisplayName(data.status)}`);
  
  // 如果开始测试，显示测试进度条
  if (data.status === 'test_started') {
    showTestProgressBar(0, 5);
  }
});

// 显示测量数据
client.on('angle_data', (data) => {
  console.log(`位置${data.position}测量角度: ${data.angle}`);
  
  // 更新UI显示测量数据
  updateAngleChart(data.position, data.angle);
  
  // 更新测试进度
  updateTestProgress(data.position, 5);
});

// 处理测试完成
client.on('test_complete', (data) => {
  console.log('测试完成, 最大角度:', data.max_angle);
  
  // 获取评估结果和建议
  const assessment = data.assessment || getAssessment(data.max_angle);
  const recommendation = data.recommendation || getRecommendation(data.max_angle);
  
  // 显示测试结果和建议
  showTestResults({
    maxAngle: data.max_angle,
    assessment: assessment,
    recommendation: recommendation,
    date: new Date(data.timestamp).toLocaleDateString(),
    time: new Date(data.timestamp).toLocaleTimeString()
  });
  
  // 添加到历史记录
  addTestToHistory({
    date: new Date(data.timestamp).toLocaleDateString(),
    time: new Date(data.timestamp).toLocaleTimeString(),
    user: client.currentUser.name,
    maxAngle: data.max_angle,
    assessment: assessment,
    recommendation: recommendation,
    deviceId: data.device_id
  });
  
  // 完成进度条
  completeTestProgress();
});

// 连接到设备
client.connect().then(() => {
  console.log('已连接到设备');
}).catch(err => {
  console.error('连接失败:', err);
  document.getElementById('device-status').textContent = '连接失败';
});

// 状态显示名称
function getStatusDisplayName(status) {
  const statusMap = {
    'connected': '已连接',
    'calibration_started': '正在校准',
    'calibration_success': '校准成功',
    'calibration_failed': '校准失败',
    'test_started': '测试开始',
    'test_complete': '测试完成'
  };
  return statusMap[status] || status;
}

// 根据角度获取评估
function getAssessment(angle) {
  if (angle <= 5) return 'normal';
  if (angle <= 7) return 'mild';
  return 'moderate';
}

// 根据角度获取建议
function getRecommendation(angle) {
  if (angle <= 5) return '正常范围内';
  if (angle <= 7) return '建议进一步X光检查';
  return '建议治疗介入';
}

// 显示测试进度条
function showTestProgressBar(current, total) {
  // 实现进度条显示逻辑
}

// 更新测试进度
function updateTestProgress(position, total) {
  // 更新进度条
  const progress = (position / total) * 100;
  document.getElementById('test-progress').style.width = `${progress}%`;
  document.getElementById('test-progress-text').textContent = `测量进度: ${position}/${total}`;
}

// 完成进度条
function completeTestProgress() {
  document.getElementById('test-progress').style.width = '100%';
  document.getElementById('test-progress-text').textContent = '测量完成';
  
  // 2秒后隐藏进度条
  setTimeout(() => {
    document.getElementById('test-progress-container').style.display = 'none';
  }, 2000);
}
```

## 注意事项

1. **用户关联**:
   - 设备不记录或识别用户信息
   - 后台系统负责将接收的数据与当前登录用户关联

2. **测试数据完整性**:
   - 每次测试需要收集完整的5组角度数据
   - 只有当5组数据全部收集完毕后，才会发送test_complete消息

3. **脊柱侧弯评估**:
   - 角度≤5度: 正常范围
   - 角度>5度且≤7度: 轻微侧弯，建议进一步X光检查
   - 角度>7度: 中度侧弯，建议治疗介入

4. **操作方式**:
   - 所有操作（校准、测试）均由用户在物理设备上直接完成
   - 后台系统只接收设备发送的数据和状态更新

5. **连接可靠性**:
   - 确保处理WebSocket连接断开的情况，并实现重连机制

6. **并发处理**:
   - 多个客户端可能同时连接到设备，确保后台系统能正确处理并发连接 