#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SPI.h>
#define DEBUG 1
#if DEBUG
  #define DEBUG_PRINT(x, y) Serial.print(x, y)
  //#define DEBUG_PRINTF(x) Serial.printf(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x, y)
  //#define DEBUG_PRINTF(x)
  #define DEBUG_PRINTLN(x)
#endif

/*
NMEA GPS
  $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
  $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48
  $GPGGA,123520,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
  $GPGSA,A,3,04,05,07,09,12,25,29,31,32,17,24,10,1.8,1.0,1.5*33
  $GPGLL,4916.45,N,12311.12,W,225444,A,*1D

$GPGGA,181908.00,3404.7041778,N,07044.3966270,W,4,13,1.00,495.144,M,29.200,M,0.10,0000*40

181908.00 is the time stamp: UTC time in hours, minutes and seconds.
3404.7041778 is the latitude in the DDMM.MMMMM format. Decimal places are variable.
N denotes north latitude.
07044.3966270 is the longitude in the DDDMM.MMMMM format. Decimal places are variable.
W denotes west longitude.
4 denotes the Quality Indicator:
0 = void
1 = Uncorrected coordinate
2 = Differentially correct coordinate (e.g., WAAS, DGPS)
4 = RTK Fix coordinate (centimeter precision)
5 = RTK Float (decimeter precision)
13 denotes number of satellites used in the coordinate
1.0 denotes the HDOP (horizontal dilution of precision)
495.144 denotes altitude of the antenna
M denotes units of altitude (eg. meters or feet)
29.200 denotes the geoidal separation (subtract this from the altitude of the antenna to arrive at the Height Above Ellipsoid (HAE).
M denotes the units used by the geoidal separation
1.0 denotes the age of the correction (if any)
0000 denotes the correction station ID (if any)
*40 denotes the checksum

Serial
  BLE<+cmd>
    基本設定流程:
      AT+VERR?                // 查詢版本 -> OK+Get:HMSoftV123
      AT+NAMEMyESP32          // 改名稱 -> OK+Set:MyESP32
      AT+PIN123456            // 改 PIN -> OK+Set:123456
      AT+RESET                // 重置生效 -> OK+RESET
    
    恢復出廠設定:
      AT+RENEW                   // 恢復預設 -> OK+RENEW
      AT+RESET                   // 重置 -> OK+RESET

    查看所有設定:
      AT+VERR?                   // 版本
      AT+NAME?                   // 名稱
      AT+ADDR?                   // MAC 位址
      AT+BAUD?                   // 鮑率
      AT+ROLE?                   // 角色
      AT+TYPE?                   // 連接類型
      AT+POWE?                   // 發射功率

  WIFI<+cmd>
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
*/

const uint8_t BT_TX = 15;
const uint8_t BT_RX = 16;
const uint8_t WiFi_TX = 6;
const uint8_t WiFi_RX = 7;
const uint8_t GPS_TX = 4;
const uint8_t GPS_RX = 5;

const uint8_t GPS_RLED = 38;
const uint8_t GPS_GLED = 40;
const uint8_t BT_BLED = 45;
const uint8_t BT_RLED = 36;
const uint8_t WF_LED = 20;

const uint16_t GSPBAUD = 9600;
const uint WIFIBAUD = 115200;
const uint16_t BTBAUD = 9600;

const uint8_t RELAY = 46;

const uint8_t EP_CS = 8;
const uint8_t EP_DC = 18;
const uint8_t EP_RST = 17;
const uint8_t EP_BUSY = 3;
const uint16_t EP_HEIGHT = 296;
const uint8_t EP_WIDTH = 152;

uint8_t hour, minute, second, status = 0;
int search_loop = 0;
float lon, lat;

//SoftwareSerial BTSer(BT_RX, BT_TX);
//SoftwareSerial WFSer(WiFi_RX, WiFi_TX);
SoftwareSerial GPSSer(GPS_RX, GPS_TX);

static const uint8_t cfg_nav5_automotive[] = 
{
  0xB5,0x62,0x06,0x24,0x24,0x00,
  0xFF,0xFF,0x06,0x03,0x00,0x00,
  0x00,0x10,0x27,0x00,0x00,0x05,
  0x00,0xFA,0x00,0x00,0x00,0x64,
  0x00,0x2C,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00
};

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
      DEBUG_PRINTLN("E-Paper ready");
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

    void partial()
    {
      sendCommand(0x22);
      sendData(0xFF); // 或 0xC4、0xF7，依模式而定
      sendCommand(0x20);
      waitBusy();
    }
};
SSD1680 ep;

void setup()
{
    Serial.begin(115200);
    //BTSer.begin(BTBAUD);
    GPSSer.begin(GSPBAUD);
    //WFSer.begin(WIFIBAUD);
    Serial1.begin(WIFIBAUD, SERIAL_8N1, WiFi_RX, WiFi_TX);
    Serial2.begin(BTBAUD, SERIAL_8N1, BT_RX, BT_TX);
    
    pinMode(GPS_RLED, OUTPUT);
    pinMode(GPS_GLED, OUTPUT);
    pinMode(BT_RLED, OUTPUT);
    pinMode(BT_BLED, OUTPUT);
    pinMode(WF_LED, OUTPUT);
    pinMode(RELAY, OUTPUT);
    
    digitalWrite(RELAY, LOW);
    digitalWrite(GPS_RLED, LOW);
    digitalWrite(GPS_GLED, HIGH);
    digitalWrite(BT_BLED, HIGH);
    digitalWrite(BT_RLED, LOW);
    digitalWrite(WF_LED, HIGH);

    GPSSer.write(cfg_nav5_automotive, sizeof(cfg_nav5_automotive));

    ep.init();
    delay(500);
    ep.clear();
    delay(2000);
    ep.sleep();
}

void loop()
{
  if (Serial.available()) 
  {
    String command = "";
    command.trim();
    
    while (Serial.available()) 
    {
      char c = Serial.read();
      command += c;
      delay(2);
    }
    
    delay(5);

    if (command.startsWith("BT"))
    {
      command = command.substring(2);
      command = "AT" + command;
      DEBUG_PRINTLN("BT: " + command);
      Serial2.print(command);
    }

    else if (command.startsWith("WF"))
    {
      command = command.substring(2);
      command = "AT" + command;
      DEBUG_PRINTLN("WF: " + command);
      Serial1.print(command);
      Serial1.print("\r\n");
    }
    
  }

  if (Serial2.available()) //Bluetooth
  {
    String response = "";
    while (Serial2.available()) 
    {
      char c = Serial2.read();
      response += c;
      delay(2);
    }
    Serial.print("HM-10: ");
    Serial.println(response);
    delay(5);
    if (response == "OK+CONN") 
    {
      digitalWrite(BT_BLED, LOW);  
      digitalWrite(BT_RLED, HIGH);
    }

    else if (response == "OK+LOST") 
    {
      digitalWrite(BT_BLED, HIGH);  
      digitalWrite(BT_RLED, LOW);
    }  

    if (response == "test")  epPattern();

    else if (response == "cls")
    {
      ep.wake();
      delay(50);
      ep.clear();
    }

    else if (response == "relay")
    {
      if (status == 0)
      {
        digitalWrite(RELAY, HIGH);
        status = 1;
      }
      else
      {
        digitalWrite(RELAY, LOW);
        status = 0;
      }
    }
  }

  if (Serial1.available())  //WiFi
  {
    String response = "";
    while (Serial1.available()) 
    {
      char c = Serial1.read();
      response += c;
      delay(2);
    }    
    if (response.startsWith("+CWLAP:"))
    {
      digitalWrite(WF_LED, HIGH);
      response = response.substring(7);
      Serial.print("ESP-01s: ");
      Serial.println(response);
    }
  }

  if (GPSSer.available()) //GPS
  {
    String line = GPSSer.readStringUntil('\n');
    line.trim();
    if (line.startsWith("$GPGGA")) 
    {
      line = line.substring(7);
      //Serial.print("GPGGA: ");  Serial.println(line);
      String parts[15];
      int index = 0;

      while (line.length() > 0 && index < 15) 
      {
        int commaIndex = line.indexOf(',');
        if (commaIndex == -1) 
        {
          parts[index++] = line;
          break;
        }
        parts[index++] = line.substring(0, commaIndex);
        line = line.substring(commaIndex + 1);
      }
      String stime = parts[0];
      String slat  = parts[1];
      String slon  = parts[3];
      String ssatellite = parts[6];
      String salt = parts[8];

      int satellite = ssatellite.toInt();
      float rawLat = slat.toFloat();
      float rawLon = slon.toFloat();  

      float latDeg = floor(rawLat / 100);
      float lonDeg = floor(rawLon / 100);
      float latMin = rawLat - latDeg * 100;
      float lonMin = rawLon - lonDeg * 100;

      float lat = latDeg + latMin / 60.0;
      float lon = lonDeg + lonMin / 60.0;
      float alt = salt.toFloat();
      String hh = stime.substring(0, 2);
      String mm = stime.substring(2, 4);
      String ss = stime.substring(4, 6);
      Serial.printf("UTC Time: %s:%s:%s\n", hh.c_str(), mm.c_str(), ss.c_str());
      if (parts[5] == "0")
      {
        Serial.println("Positioning...");
        digitalWrite(GPS_RLED, LOW);
        digitalWrite(GPS_GLED, HIGH);
      }
      else
      {
        digitalWrite(GPS_RLED, HIGH);
        digitalWrite(GPS_GLED, LOW);
        Serial.print("Latitude: ");
        Serial.print(lat, 6);
        Serial.print(" ");
        Serial.print(parts[2]);
        Serial.print(", Longitude: ");
        Serial.print(lon, 6);
        Serial.print(" ");
        Serial.println(parts[4]);
        Serial.print("Altitude: ");
        Serial.print(alt, 1);
        Serial.print(" M, Satellite count: ");
        Serial.println(satellite);
      }
      
    }
  }
  search_loop = search_loop + 1;
  if (search_loop == 500000)
  {
    Serial1.print("AT+CWLAP");
    Serial1.print("\r\n");
    search_loop = 0;   
    digitalWrite(WF_LED, LOW);
    delay(10);
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
