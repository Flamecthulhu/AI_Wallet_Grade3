#include <Arduino.h>
#include <SPI.h>

#define EPD_CS    4 //8
#define EPD_DC    16 //18
#define EPD_RST   17
#define EPD_BUSY  0 //3
#define EPD_WIDTH  152
#define EPD_HEIGHT 296


class SSD1680 {
  private:
    void spiWrite(uint8_t data) {
      digitalWrite(EPD_CS, LOW);
      SPI.transfer(data);
      digitalWrite(EPD_CS, HIGH);
    }
    
    void sendCommand(uint8_t cmd) {
      digitalWrite(EPD_DC, LOW);
      spiWrite(cmd);
    }
    
    void sendData(uint8_t data) {
      digitalWrite(EPD_DC, HIGH);
      spiWrite(data);
    }
    
    void waitBusy() {
      //Serial.println("等待顯示器就緒...");
      while(digitalRead(EPD_BUSY) == HIGH) {
        delay(10);
      }
      //Serial.println("顯示器就緒");
    }
    
  public:
    void init() {
      // 初始化 SPI
      pinMode(EPD_CS, OUTPUT);
      pinMode(EPD_DC, OUTPUT);
      pinMode(EPD_RST, OUTPUT);
      pinMode(EPD_BUSY, INPUT);
      
      SPI.begin();
      SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
      
      // 硬體重置
      digitalWrite(EPD_RST, HIGH);
      delay(20);
      digitalWrite(EPD_RST, LOW);
      delay(2);
      digitalWrite(EPD_RST, HIGH);
      delay(20);
      
      waitBusy();
      
      // 軟體重置
      sendCommand(0x12);
      delay(10);
      waitBusy();
      
      // 設定驅動器輸出控制
      sendCommand(0x01);
      sendData((EPD_HEIGHT - 1) & 0xFF);
      sendData(((EPD_HEIGHT - 1) >> 8) & 0xFF);
      sendData(0x00);
      
      // 設定資料進入模式
      sendCommand(0x11);
      sendData(0x03);  // X增量, Y增量
      
      // 設定邊界波形
      sendCommand(0x3C);
      sendData(0x05);
      
      // 設定溫度感測器
      sendCommand(0x18);
      sendData(0x80);
      
      // 設定顯示更新控制
      sendCommand(0x22);
      sendData(0xB1);
      
      sendCommand(0x20);
      waitBusy();
      
      Serial.println("SSD1680 初始化完成");
    }
    
    void setWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
      // 設定 X 範圍
      sendCommand(0x44);
      sendData(x_start / 8);
      sendData(x_end / 8);
      
      // 設定 Y 範圍
      sendCommand(0x45);
      sendData(y_start & 0xFF);
      sendData((y_start >> 8) & 0xFF);
      sendData(y_end & 0xFF);
      sendData((y_end >> 8) & 0xFF);
    }
    
    void setCursor(uint16_t x, uint16_t y) {
      // 設定 X 位置
      sendCommand(0x4E);
      sendData(x / 8);
      
      // 設定 Y 位置
      sendCommand(0x4F);
      sendData(y & 0xFF);
      sendData((y >> 8) & 0xFF);
    }
    
    void display(const uint8_t* image) {
      uint16_t width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
      uint16_t height = EPD_HEIGHT;
      
      setWindow(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
      setCursor(0, 0);
      
      // 寫入黑白資料
      sendCommand(0x24);
      for(uint16_t j = 0; j < height; j++) {
        for(uint16_t i = 0; i < width; i++) {
          sendData(image[i + j * width]);
        }
      }
      
      // 更新顯示
      sendCommand(0x22);
      sendData(0xF7);
      sendCommand(0x20);
      waitBusy();
    }
    
    void clear() {
      uint16_t width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
      uint16_t height = EPD_HEIGHT;
      
      setWindow(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
      setCursor(0, 0);
      
      sendCommand(0x24);
      for(uint16_t j = 0; j < height; j++) {
        for(uint16_t i = 0; i < width; i++) {
          sendData(0xFF);  // 0xFF = 白色
        }
      }
      
      sendCommand(0x22);
      sendData(0xF7);
      sendCommand(0x20);
      waitBusy();
    }
    
    void wake() 
    {
      digitalWrite(EPD_RST, HIGH);
      delay(20);
      digitalWrite(EPD_RST, LOW);
      delay(2);
      digitalWrite(EPD_RST, HIGH);
      delay(20);
      waitBusy();
    }

    void sleep() 
    {
      sendCommand(0x10);
      sendData(0x01);
    }
};

SSD1680 epd;

void setup() {
  Serial.begin(115200);
  //Serial.println("SSD1680 電子紙測試程式");
  
  epd.init();
  delay(500);
  
  // 清除螢幕
  epd.clear();
  delay(2000);
  
  // 顯示測試圖案
  displayTestPattern();
  


  // 進入睡眠模式以省電
  epd.sleep();
}

void loop() {
  // 空迴圈
}

void displayTestPattern() {
  Serial.println("顯示測試圖案");
  uint16_t width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
  uint16_t height = EPD_HEIGHT;
  uint8_t image[width * height];
  
  // 建立測試圖案: 棋盤格
  for(uint16_t y = 0; y < height; y++) {
    for(uint16_t x = 0; x < width; x++) {
      if((y / 20 + x) % 2 == 0) {
        image[x + y * width] = 0x00;  // 黑色
      } else {
        image[x + y * width] = 0xFF;  // 白色
      }
    }
  }
  Serial.printf("Dec: %d\n", int(image));
  Serial.printf("Hex: %X\n", int(image));
  
  int binaryNum[10000];
  int i = 0;
  int n = int(image);

  while (n > 0) {
    binaryNum[i] = n % 2;
    n = n / 2;
    i++;
  }

  Serial.printf("Bin: ");
  for (int j = i - 1; j >= 0; j--)
    Serial.printf("%d", binaryNum[j]);
  Serial.printf("\n");

  epd.display(image);
}