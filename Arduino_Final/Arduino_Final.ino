#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SPI.h>

#include "EPD266_BW.h"
#include "interface.c"

#define DEBUG_MODE 0

const uint8_t GPS_TX = 4;
const uint8_t GPS_RX = 5;
const uint8_t BT_TX = 15;
const uint8_t BT_RX = 16;
const uint8_t WiFi_TX = 6;
const uint8_t WiFi_RX = 7;

const uint8_t GPS_RLED = 38;
const uint8_t GPS_GLED = 40;
const uint8_t BT_RLED = 36;
const uint8_t BT_BLED = 45;
const uint8_t WF_LED = 20;

const uint16_t GSPBAUD = 9600;
const uint16_t BTBAUD = 9600;
const int WIFIBAUD = 115200;

const uint8_t RELAY = 46;

const uint8_t EP_CS = 8;
const uint8_t EP_DC = 18;
const uint8_t EP_RST = 17;
const uint8_t EP_BUSY = 3;
const uint16_t EP_HEIGHT = 296;
const uint8_t EP_WIDTH = 152;

uint8_t hour, minute, second, status = 0;
uint16_t min_of_day;
float lon, lat;

class easyfunction
{
  void pinInit()
  {
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
  } 
  void epdInit() 
  {
    epd.init(115200, false, 50, false);
    epd.setRotation(1);
    epd.setTextColor(GxEPD_BLACK);
    epd.setTextSize(4);
    epd.setFullWindow();
  }
  void baudInit()
  {
    Serial.begin(115200);
    GPSSerial.begin(GSPBAUD);
    Serial1.begin(WIFIBAUD, SERIAL_8N1, WiFi_RX, WiFi_TX);
    Serial2.begin(BTBAUD, SERIAL_8N1, BT_RX, BT_TX);
  }
 
};

SoftwareSerial GPSSerial(GPS_RX, GPS_TX);
GxEPD2_BW<GxEPD2_266_BN, GxEPD2_266_BN::HEIGHT> epd(GxEPD2_266_BN(EP_CS, EP_DC, EP_RST, EP_BUSY));
easyfunction ef;

void setup()
{
  ef.baudInit();
  ef.pinInit();
  ef.epdInit();
}

void loop()
{
  if (GPSSer.available()) //GPS
  {
    String line = GPSSer.readStringUntil('\n');
    line.trim();
    if (line.startsWith("$GPGGA")) 
    {
      line = line.substring(7);
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

      lat = latDeg + latMin / 60.0;
      lon = lonDeg + lonMin / 60.0;
      float alt = salt.toFloat();

      String hh = stime.substring(0, 2);
      String mm = stime.substring(2, 4);
      String ss = stime.substring(4, 6);

      hour = hh.toInt();
      hour += 8; //Convert UTC to +8 hour
      if (hour > 24)  hour -= 24;

      minute = mm.toInt();
      min_of_day = (hour * 60) + minute;
      
      if (DEBUG_MODE)
      {
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
  }
}
