#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SPI.h>
#define DEBUG 0
#if DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

const uint8_t BT_TX = 15;
const uint8_t BT_RX = 16;
const uint8_t WiFi_TX = 6;
const uint8_t WiFi_RX = 7;
const uint8_t GPS_TX = 4;
const uint8_t GPS_RX = 5;
const uint8_t EP_CS = 8;
const uint8_t EP_DC = 18;
const uint8_t EP_RST = 17;
const uint8_t EP_BUSY = 3;
const uint16_t EP_HEIGHT = 296;
const uint8_t EP_WIDTH = 152;

class SSD1680 
{
  private:
    void spiWrite(uint8_t data) 
    {
      digitalWrite(EP_CS, LOW);
      SPI.transfer(data);
      digitalWrite(EP_CS, HIGH);
    }
    
    void sendCommand(uint8_t cmd) 
    {
      digitalWrite(EP_DC, LOW);
      spiWrite(cmd);
    }
    
    void sendData(uint8_t data) 
    {
      digitalWrite(EP_DC, HIGH);
      spiWrite(data);
    }
    
    void waitBusy() 
    {
      DEBUG_PRINTLN("Waiting for E-Paper display");
      while(digitalRead(EP_BUSY) == HIGH) 
      {
        delay(10);
      }
      DEBUG_PRINTLN("E-Paper readu");
    }
    
  public:
    void init() 
    {
      pinMode(EP_CS, OUTPUT);
      pinMode(EP_DC, OUTPUT);
      pinMode(EP_RST, OUTPUT);
      pinMode(EP_BUSY, INPUT);
      SPI.begin();
      SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
      digitalWrite(EP_RST, HIGH);
      delay(20);
      digitalWrite(EP_RST, LOW);
      delay(2);
      digitalWrite(EP_RST, HIGH);
      delay(20);
      waitBusy();
      sendCommand(0x12);
      delay(10);
      waitBusy();
      sendCommand(0x01);
      sendData((EP_HEIGHT - 1) & 0xFF);
      sendData(((EP_HEIGHT - 1) >> 8) & 0xFF);
      sendData(0x00);
      sendCommand(0x11);
      sendData(0x03);
      sendCommand(0x3C);
      sendData(0x05);
      sendCommand(0x18);
      sendData(0x80);
      sendCommand(0x22);
      sendData(0xB1);
      sendCommand(0x20);
      waitBusy();
      DEBUG_PRINTLN("SSD1680 initialized");
    }
    
    void setWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) 
    {
      sendCommand(0x44);
      sendData(x_start / 8);
      sendData(x_end / 8);
      sendCommand(0x45);
      sendData(y_start & 0xFF);
      sendData((y_start >> 8) & 0xFF);
      sendData(y_end & 0xFF);
      sendData((y_end >> 8) & 0xFF);
    }
    
    void setCursor(uint16_t x, uint16_t y) 
    {
      sendCommand(0x4E);
      sendData(x / 8);
      sendCommand(0x4F);
      sendData(y & 0xFF);
      sendData((y >> 8) & 0xFF);
    }
    
    void display(const uint8_t* image) 
    {
      uint16_t width = (EP_WIDTH % 8 == 0) ? (EP_WIDTH / 8) : (EP_WIDTH / 8 + 1);
      uint16_t height = EP_HEIGHT;
      setWindow(0, 0, EP_WIDTH - 1, EP_HEIGHT - 1);
      setCursor(0, 0);
      sendCommand(0x24);
      for(uint16_t j = 0; j < height; j++) 
      {
        for(uint16_t i = 0; i < width; i++) 
        {
          sendData(image[i + j * width]);
        }
      }
      sendCommand(0x22);
      sendData(0xF7);
      sendCommand(0x20);
      waitBusy();
    }
    
    void clear() 
    {
      uint16_t width = (EP_WIDTH % 8 == 0) ? (EP_WIDTH / 8) : (EP_WIDTH / 8 + 1);
      uint16_t height = EP_HEIGHT;
      setWindow(0, 0, EP_WIDTH - 1, EP_HEIGHT - 1);
      setCursor(0, 0);
      sendCommand(0x24);
      for(uint16_t j = 0; j < height; j++) {
        for(uint16_t i = 0; i < width; i++) {
          sendData(0xFF);
        }
      }
      
      sendCommand(0x22);
      sendData(0xF7);
      sendCommand(0x20);
      waitBusy();
    }

    void wake() 
    {
      digitalWrite(EP_RST, HIGH);
      delay(20);
      digitalWrite(EP_RST, LOW);
      delay(2);
      digitalWrite(EP_RST, HIGH);
      delay(20);
      waitBusy();
    }

    void sleep() 
    {
      sendCommand(0x10);
      sendData(0x01);
    }
};

SSD1680 ep;
SoftwareSerial SerialWF(WiFi_RX, WiFi_TX);

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, BT_RX, BT_TX);
    SerialWF.begin(115200);
    ep.init();
    delay(500);
    ep.clear();
    delay(2000);
    ep.sleep();
}

void loop()
{
  if (Serial2.available()) {
    String response = "";
    while (Serial2.available()) {
      char c = Serial2.read();
      response += c;
      delay(2);
    }
    Serial.print("<<< ");
    Serial.println(response);
    if (response == "TEST")  epPattern();
    if (response == "cls") 
    {
      ep.wake();
      delay(50);
      ep.clear();
    }
  }
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();    
    Serial.print(">>> ");
    Serial.println(command);
    Serial2.print(command);
    delay(300);
  }
}

void epPattern()
{
    ep.wake();
    uint16_t width = (EP_WIDTH % 8 == 0) ? (EP_WIDTH / 8) : (EP_WIDTH / 8 + 1);
    uint16_t height = EP_HEIGHT;
    uint8_t image[width * height];
    for(uint16_t y = 0; y < height; y++) {
        for(uint16_t x = 0; x < width; x++) {
        if((y / 20 + x) % 2 == 0) {
            image[x + y * width] = 0x00;  // 黑色
        } else {
            image[x + y * width] = 0xFF;  // 白色
        }
        }
    }
    ep.display(image);
    ep.sleep();
}
