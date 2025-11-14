#include "EPD266_BW.h"
#include "interface.c"

#define BUSY 3
#define RES 17
#define DC 18
#define CS 8
#define EC 46

GxEPD2_BW<GxEPD2_266_BN, GxEPD2_266_BN::HEIGHT> epd(GxEPD2_266_BN(CS, DC, RES, BUSY));

void epdInit() 
{
  epd.init(115200, false, 50, false);
  epd.setRotation(1);
  epd.setTextColor(GxEPD_BLACK);
  epd.setTextSize(4);
  epd.setFullWindow();
}

void setup() 
{
  pinMode(EC, OUTPUT);
  digitalWrite(EC, LOW);

  epdInit();
  epd.setCursor(0, 0);

  epd.fillScreen(GxEPD_WHITE);
  epd.drawBitmap(0, 0, startup, 296, 152, GxEPD_BLACK);
  epd.display();

  epd.fillScreen(GxEPD_WHITE);  
  epd.drawBitmap(0, 0, einvoice, 296, 152, GxEPD_BLACK);
  epd.display();

  digitalWrite(EC, HIGH);
  epd.fillScreen(GxEPD_WHITE); 
  epd.drawBitmap(0, 0, easycard, 296, 152, GxEPD_BLACK);
  epd.display();
  digitalWrite(EC, LOW);

  epd.fillScreen(GxEPD_WHITE);
  epd.drawBitmap(0, 0, thsr, 296, 152, GxEPD_BLACK);
  epd.display();

  epd.fillScreen(GxEPD_WHITE);
  epd.drawBitmap(0, 0, tra, 296, 152, GxEPD_BLACK);
  epd.display();

  epd.hibernate();
}

void loop() {}


//.drawBitmap(x, y, bitmapArray, width, height, GxEPD_BLACK);