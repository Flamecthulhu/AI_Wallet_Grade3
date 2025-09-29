/*
 * ESP32-S3 + ESP-01S WiFi 模組範例
 * ESP-01S 使用出廠 AT 韌體，不需要重新燒錄
 * 
 * 功能:
 * 1. 透過 AT 指令控制 ESP-01S
 * 2. 連接 WiFi
 * 3. 發送 HTTP 請求
 * 4. 建立 TCP Server/Client
 */

// ESP32-S3 UART1 連接 ESP-01S
#define ESP01_RXD 18   // ESP32-S3 RX1 -> ESP-01S TX
#define ESP01_TXD 17   // ESP32-S3 TX1 -> ESP-01S RX

// 你的 WiFi 設定
const char* ssid = "Allan’s iPhone 12 mini";      // 改成你的 WiFi 名稱
const char* password = "wwe107001";   // 改成你的 WiFi 密碼

String response = "";
bool isConnected = false;

void setup() {
  // USB 串口 (監控用)
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP32-S3 + ESP-01S WiFi 模組 ===");
  
  // UART1 連接 ESP-01S (預設 115200)
  Serial1.begin(115200, SERIAL_8N1, ESP01_RXD, ESP01_TXD);
  
  Serial.println("正在初始化 ESP-01S...\n");
  delay(2000);
  
  // 初始化 ESP-01S
  if (initESP01()) {
    Serial.println("✓ ESP-01S 初始化成功！\n");
    
    // 連接 WiFi
    if (connectWiFi(ssid, password)) {
      Serial.println("✓ WiFi 連接成功！");
      isConnected = true;
      
      // 顯示 IP 位址
      getIP();
      
      // 測試 HTTP GET 請求
      Serial.println("\n測試 HTTP 請求...");
      httpGet("www.google.com", "/");
    }
  } else {
    Serial.println("✗ ESP-01S 初始化失敗");
    Serial.println("請檢查:");
    Serial.println("  1. ESP-01S 接線是否正確");
    Serial.println("  2. ESP-01S 是否有電 (3.3V)");
    Serial.println("  3. CH_PD 是否接 3.3V");
  }
  
  Serial.println("\n==== 指令列表 ====");
  Serial.println("test    - 測試 AT 指令");
  Serial.println("wifi    - 重新連接 WiFi");
  Serial.println("ip      - 查詢 IP 位址");
  Serial.println("ping    - Ping Google");
  Serial.println("http    - HTTP GET 測試");
  Serial.println("scan    - 掃描 WiFi");
  Serial.println("status  - 查詢狀態");
  Serial.println("AT+xxx  - 直接發送 AT 指令");
  Serial.println("==================\n");
}

void loop() {
  // 從 ESP-01S 接收資料
  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);
    response += c;
  }
  
  // 從電腦接收指令
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd == "test") {
      testAT();
    } else if (cmd == "wifi") {
      connectWiFi(ssid, password);
    } else if (cmd == "ip") {
      getIP();
    } else if (cmd == "ping") {
      pingGoogle();
    } else if (cmd == "http") {
      httpGet("www.google.com", "/");
    } else if (cmd == "scan") {
      scanWiFi();
    } else if (cmd == "status") {
      checkStatus();
    } else if (cmd.startsWith("AT")) {
      // 直接發送 AT 指令
      sendCommand(cmd, 2000);
    } else {
      Serial.println("未知指令，輸入指令名稱查看列表");
    }
  }
}

// ==================== AT 指令函數 ====================

bool initESP01() {
  Serial.println("1. 測試 AT 通訊...");
  sendCommand("AT", 1000);
  
  Serial.println("2. 關閉回顯...");
  sendCommand("AT+ATE0", 1000);
  
  Serial.println("3. 設定 WiFi 模式 (Station)...");
  sendCommand("AT+CWMODE=1", 1000);
  
  Serial.println("4. 設定多連接模式...");
  sendCommand("AT+CIPMUX=1", 1000);
  
  return true;
}

bool connectWiFi(const char* ssid, const char* pwd) {
  Serial.println("\n連接 WiFi...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += pwd;
  cmd += "\"";
  
  sendCommand(cmd, 10000);
  
  // 檢查是否連接成功
  delay(2000);
  sendCommand("AT+CWJAP?", 2000);
  
  return true;
}

void getIP() {
  Serial.println("\n查詢 IP 位址...");
  sendCommand("AT+CIFSR", 2000);
}

void scanWiFi() {
  Serial.println("\n掃描附近的 WiFi...");
  sendCommand("AT+CWLAP", 5000);
}

void checkStatus() {
  Serial.println("\n=== 系統狀態 ===");
  Serial.println("1. WiFi 連接狀態:");
  sendCommand("AT+CWJAP?", 2000);
  
  Serial.println("\n2. IP 位址:");
  sendCommand("AT+CIFSR", 2000);
  
  Serial.println("\n3. 連接狀態:");
  sendCommand("AT+CIPSTATUS", 2000);
}

void testAT() {
  Serial.println("\n測試 AT 指令...");
  sendCommand("AT", 1000);
  delay(500);
  sendCommand("AT+GMR", 2000);  // 查詢版本
}

void pingGoogle() {
  Serial.println("\n測試網路連接 (Ping)...");
  sendCommand("AT+PING=\"www.google.com\"", 5000);
}

void httpGet(const char* host, const char* path) {
  Serial.println("\n=== HTTP GET 請求 ===");
  
  // 1. 建立 TCP 連接
  Serial.println("1. 建立連接...");
  String cmd = "AT+CIPSTART=0,\"TCP\",\"";
  cmd += host;
  cmd += "\",80";
  sendCommand(cmd, 5000);
  delay(1000);
  
  // 2. 準備 HTTP 請求
  String httpRequest = "GET ";
  httpRequest += path;
  httpRequest += " HTTP/1.1\r\n";
  httpRequest += "Host: ";
  httpRequest += host;
  httpRequest += "\r\n";
  httpRequest += "Connection: close\r\n\r\n";
  
  // 3. 發送資料長度
  Serial.println("2. 發送請求...");
  cmd = "AT+CIPSEND=0,";
  cmd += httpRequest.length();
  sendCommand(cmd, 2000);
  delay(500);
  
  // 4. 發送 HTTP 請求
  Serial1.print(httpRequest);
  Serial.println("HTTP 請求已發送");
  
  // 5. 等待回應
  Serial.println("3. 等待回應...");
  delay(3000);
  
  // 6. 關閉連接
  Serial.println("4. 關閉連接...");
  sendCommand("AT+CIPCLOSE=0", 2000);
}

// ==================== 工具函數 ====================

void sendCommand(String cmd, int timeout) {
  response = "";
  
  Serial.print(">>> ");
  Serial.println(cmd);
  
  Serial1.println(cmd);
  
  unsigned long start = millis();
  while (millis() - start < timeout) {
    while (Serial1.available()) {
      char c = Serial1.read();
      Serial.write(c);
      response += c;
    }
  }
}

/*
==================== 硬體連接 ====================

ESP32-S3          ESP-01S
--------          -------
GPIO17 (TX1)   -> RX (GPIO3)
GPIO18 (RX1)   -> TX (GPIO1)
3.3V           -> VCC + CH_PD
GND            -> GND

重要提醒:
1. ESP-01S 需要穩定的 3.3V 電源 (至少 250mA)
2. CH_PD (或 CH_EN) 必須接 3.3V
3. 如果 ESP32-S3 供電不足，用外接 LDO (AMS1117-3.3)
4. TX/RX 交叉連接

==================== ESP-01S AT 韌體 ====================

ESP-01S 出廠通常已有 AT 韌體，鮑率可能是:
- 115200 (較新版本)
- 9600 (舊版本)

如果無法通訊，可以嘗試:
1. 在 Serial1.begin() 改用 9600
2. 或使用 AT+UART_CUR=115200,8,1,0,0 改鮑率

==================== 常用 AT 指令 ====================

基本指令:
AT              - 測試
AT+RST          - 重啟
AT+GMR          - 查詢版本
AT+UART?        - 查詢串口設定

WiFi 指令:
AT+CWMODE=1     - Station 模式
AT+CWJAP="SSID","PASS" - 連接 WiFi
AT+CWJAP?       - 查詢連接狀態
AT+CWLAP        - 掃描 WiFi
AT+CIFSR        - 查詢 IP

TCP/IP 指令:
AT+CIPMUX=1     - 多連接模式
AT+CIPSTART=0,"TCP","host",port - 建立連接
AT+CIPSEND=0,length - 發送資料
AT+CIPCLOSE=0   - 關閉連接
AT+CIPSTATUS    - 查詢連接狀態

==================== 除錯技巧 ====================

1. 無法連接 ESP-01S:
   - 檢查 TX/RX 是否接對
   - 嘗試 9600 或 115200 鮑率
   - 確認 CH_PD 接 3.3V
   - 測量電壓是否穩定

2. WiFi 連接失敗:
   - 檢查 SSID 和密碼
   - 確認信號強度
   - 重啟 ESP-01S (AT+RST)

3. HTTP 請求失敗:
   - 先測試 Ping
   - 檢查防火牆設定
   - 確認網路連接正常

==================================================
*/