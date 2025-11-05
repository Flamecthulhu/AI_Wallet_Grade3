#include <GxEPD2_BW.h>
#include <GxEPD2_BW.h> // 依型號選擇

GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=*/ 8, /*DC=*/ 18, /*RST=*/ 17, /*BUSY=*/ 3));

void setup() {
  display.init();
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(0, 10);
  display.print("Hello SSD1680!");
  display.display(); // 更新屏幕
}

void loop() {}