/*
 * ESP32 HM-10 BLE 模組 AT 命令工具
 * 適用於 HM-10, HM-11 等 BLE 4.0 模組
 * 
 * HM-10 特點:
 * - 預設鮑率: 9600
 * - 不需要按鍵進入 AT 模式 (隨時可用)
 * - 命令不需要換行符號 (發送後立即執行)
 * - 大部分命令回應 OK 或 OK+xxx
 */

#define BT_RXD2 16  // ESP32 RX2 (連接到 HM-10 TX)
#define BT_TXD2 17  // ESP32 TX2 (連接到 HM-10 RX)

void setup() {
  // USB 串口 (電腦監控)
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP32 HM-10 BLE AT 命令工具 ===");
  Serial.println("低功耗藍牙 (BLE 4.0) 模組\n");
  
  // HM-10 預設鮑率是 9600
  Serial2.begin(9600, SERIAL_8N1, BT_RXD2, BT_TXD2);
  
  Serial.println("UART2 已啟動 (9600 baud)");
  Serial.println("HM-10 隨時可接收 AT 命令\n");
  
  printHelp();
  
  // 自動測試連接
  delay(1000);
  Serial.println("自動測試連接...");
  Serial2.print("AT");
  delay(500);
}

void loop() {
  // 從 HM-10 接收資料並顯示
  if (Serial2.available()) {
    String response = "";
    while (Serial2.available()) {
      char c = Serial2.read();
      response += c;
      delay(2);  // 等待資料接收完整
    }
    Serial.print("<<< ");
    Serial.println(response);
  }
  
  // 從電腦接收指令並發送到 HM-10
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    // 處理特殊指令
    if (command.startsWith("BAUD:")) {
      changeBaudRate(command.substring(5).toInt());
      return;
    } else if (command == "HELP") {
      printHelp();
      return;
    } else if (command == "TEST") {
      testConnection();
      return;
    }
    
    // 發送 AT 命令 (HM-10 不需要換行符)
    Serial.print(">>> ");
    Serial.println(command);
    Serial2.print(command);  // 注意: 用 print 不是 println
    
    // 等待回應
    delay(300);
  }
}

void changeBaudRate(int baud) {
  if (baud > 0) {
    Serial2.end();
    Serial2.begin(baud, SERIAL_8N1, BT_RXD2, BT_TXD2);
    Serial.print("鮑率已切換為: ");
    Serial.println(baud);
  } else {
    Serial.println("無效的鮑率");
  }
}

void testConnection() {
  Serial.println("\n=== 測試 HM-10 連接 ===");
  
  Serial.println("1. 測試基本連接...");
  Serial2.print("AT");
  delay(500);
  
  Serial.println("2. 查詢版本...");
  Serial2.print("AT+VERR?");
  delay(500);
  
  Serial.println("3. 查詢名稱...");
  Serial2.print("AT+NAME?");
  delay(500);
  
  Serial.println("4. 查詢位址...");
  Serial2.print("AT+ADDR?");
  delay(500);
  
  Serial.println("\n測試完成！\n");
}

void printHelp() {
  Serial.println("==================== 使用說明 ====================");
  Serial.println("\n工具指令:");
  Serial.println("  HELP           - 顯示此說明");
  Serial.println("  TEST           - 自動測試連接");
  Serial.println("  BAUD:9600      - 切換鮑率");
  
  Serial.println("\n==================== HM-10 AT 命令 ====================");
  Serial.println("注意: 命令不需要換行符，直接輸入即可");
  
  Serial.println("\n【查詢命令】");
  Serial.println("  AT                    - 測試連接 (回應: OK)");
  Serial.println("  AT+VERR?              - 查詢韌體版本");
  Serial.println("  AT+VERS?              - 查詢軟體版本");
  Serial.println("  AT+NAME?              - 查詢裝置名稱");
  Serial.println("  AT+ADDR?              - 查詢 MAC 位址");
  Serial.println("  AT+BAUD?              - 查詢鮑率");
  Serial.println("  AT+TYPE?              - 查詢連接類型");
  Serial.println("  AT+POWE?              - 查詢發射功率");
  Serial.println("  AT+ROLE?              - 查詢角色 (0=從機)");
  Serial.println("  AT+PIN?               - 查詢 PIN 碼");
  
  Serial.println("\n【設定命令】");
  Serial.println("  AT+NAMEMyESP32        - 設定名稱 (最多 12 字元)");
  Serial.println("  AT+BAUD0              - 設定鮑率 (見下方對照表)");
  Serial.println("  AT+ROLE0              - 設定角色 (0=從機, 1=主機)");
  Serial.println("  AT+TYPE0              - 連接方式 (0=不需配對, 1~3=不同配對方式)");
  Serial.println("  AT+POWE3              - 發射功率 (0~3, 3 最強)");
  Serial.println("  AT+PIN123456          - 設定 PIN 碼 (6 位數)");
  Serial.println("  AT+RESET              - 軟體重置");
  Serial.println("  AT+RENEW              - 恢復出廠設定");
  
  Serial.println("\n【鮑率對照表】");
  Serial.println("  0 = 9600    (預設)");
  Serial.println("  1 = 19200");
  Serial.println("  2 = 38400");
  Serial.println("  3 = 57600");
  Serial.println("  4 = 115200");
  Serial.println("  5 = 4800");
  Serial.println("  6 = 2400");
  Serial.println("  7 = 1200");
  Serial.println("  8 = 230400");
  
  Serial.println("\n【發射功率對照表】");
  Serial.println("  0 = -23dBm");
  Serial.println("  1 = -6dBm");
  Serial.println("  2 = 0dBm");
  Serial.println("  3 = 6dBm (預設，最強)");
  
  Serial.println("\n【進階命令】");
  Serial.println("  AT+SLEEP              - 進入睡眠模式");
  Serial.println("  AT+IMME0              - 設定啟動後自動工作");
  Serial.println("  AT+IMME1              - 設定啟動後等待 AT 命令");
  Serial.println("  AT+START              - 開始工作 (IMME1 時使用)");
  Serial.println("  AT+NOTI0              - 關閉連接狀態通知");
  Serial.println("  AT+NOTI1              - 開啟連接狀態通知");

  Serial.println("\n==================================================");
  Serial.println("請在序列埠監控視窗輸入 AT 命令...\n");
}

/*
==================== HM-10 設定範例 ====================

### 基本設定流程:
1. AT                      // 測試連接 -> OK
2. AT+VERR?                // 查詢版本 -> OK+Get:HMSoftV123
3. AT+NAMEMyESP32          // 改名稱 -> OK+Set:MyESP32
4. AT+PIN123456            // 改 PIN -> OK+Set:123456
5. AT+RESET                // 重置生效 -> OK+RESET

### 改為 115200 鮑率:
1. AT+BAUD4                // 設定為 115200 -> OK+Set:4
2. 在序列埠輸入: BAUD:115200
3. AT                      // 測試 -> OK

### 恢復出廠設定:
AT+RENEW                   // 恢復預設 -> OK+RENEW
AT+RESET                   // 重置 -> OK+RESET

### 查看所有設定:
AT+VERR?                   // 版本
AT+NAME?                   // 名稱
AT+ADDR?                   // MAC 位址
AT+BAUD?                   // 鮑率
AT+ROLE?                   // 角色
AT+TYPE?                   // 連接類型
AT+POWE?                   // 發射功率

==================================================
*/