// Display Library for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Reduced version - Author: Jean-Marc Zingg
// Library: https://github.com/ZinggJM/GxEPD2

#ifndef _GxEPD2_BW_H_
#define _GxEPD2_BW_H_

#ifndef ENABLE_GxEPD2_GFX
#define ENABLE_GxEPD2_GFX 0
#endif

#if ENABLE_GxEPD2_GFX
#include "GxEPD2_GFX.h"
#define GxEPD2_GFX_BASE_CLASS GxEPD2_GFX
#elif defined(_GFX_H_)
#define GxEPD2_GFX_BASE_CLASS GFX
#else
#include <Adafruit_GFX.h>
#define GxEPD2_GFX_BASE_CLASS Adafruit_GFX
#endif

#include "GxEPD2_EPD.h"

// 使用 __has_include 來條件性包含特定型號的頭文件
#if defined __has_include
#  if !__has_include("GxEPD2.h")
#    undef __has_include
#    define __has_include(x) true
#  endif
#else
#  define __has_include(x) true
#endif

// 只包含 266 型號
#if __has_include("epd/GxEPD2_266_BN.h")
#include "epd/GxEPD2_266_BN.h"
#endif
#if __has_include("gdey/GxEPD2_266_GDEY0266T90.h")
#include "gdey/GxEPD2_266_GDEY0266T90.h"
#endif

template<typename GxEPD2_Type, const uint16_t page_height>
class GxEPD2_BW : public GxEPD2_GFX_BASE_CLASS
{
  public:
    GxEPD2_Type epd2;
    
#if ENABLE_GxEPD2_GFX
    GxEPD2_BW(GxEPD2_Type epd2_instance) : GxEPD2_GFX_BASE_CLASS(epd2, GxEPD2_Type::WIDTH_VISIBLE, GxEPD2_Type::HEIGHT), epd2(epd2_instance)
#else
    GxEPD2_BW(GxEPD2_Type epd2_instance) : GxEPD2_GFX_BASE_CLASS(GxEPD2_Type::WIDTH_VISIBLE, GxEPD2_Type::HEIGHT), epd2(epd2_instance)
#endif
    {
      _page_height = page_height;
      _pages = (HEIGHT / _page_height) + ((HEIGHT % _page_height) > 0);
      _reverse = (epd2_instance.panel == GxEPD2::GDE0213B1);
      _mirror = false;
      _using_partial_mode = false;
      _current_page = 0;
      setFullWindow();
    }

    // 基本屬性
    uint16_t pages() { return _pages; }
    uint16_t pageHeight() { return _page_height; }
    bool mirror(bool m) { _swap_(_mirror, m); return m; }

    // 繪圖核心
    void drawPixel(int16_t x, int16_t y, uint16_t color)
    {
      if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;
      if (_mirror) x = width() - x - 1;
      
      switch (getRotation())
      {
        case 1:
          _swap_(x, y);
          x = WIDTH - x - 1;
          break;
        case 2:
          x = WIDTH - x - 1;
          y = HEIGHT - y - 1;
          break;
        case 3:
          _swap_(x, y);
          y = HEIGHT - y - 1;
          break;
      }
      
      x -= _pw_x;
      if (!_reverse) y -= _pw_y;
      else y = HEIGHT - _pw_y - y - 1;
      
      if ((x < 0) || (x >= int16_t(_pw_w)) || (y < 0) || (y >= int16_t(_pw_h))) return;
      
      y -= _current_page * _page_height;
      if ((y < 0) || (y >= int16_t(_page_height))) return;
      
      uint16_t i = x / 8 + y * (_pw_w / 8);
      if (color)
        _buffer[i] = (_buffer[i] | (1 << (7 - x % 8)));
      else
        _buffer[i] = (_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
    }

    // 初始化方法
    void init(uint32_t serial_diag_bitrate = 0)
    {
      epd2.init(serial_diag_bitrate);
      _using_partial_mode = false;
      _current_page = 0;
      setFullWindow();
    }

    void init(uint32_t serial_diag_bitrate, bool initial, uint16_t reset_duration = 10, bool pulldown_rst_mode = false)
    {
      epd2.init(serial_diag_bitrate, initial, reset_duration, pulldown_rst_mode);
      _using_partial_mode = false;
      _current_page = 0;
      setFullWindow();
    }

    void end() { epd2.end(); }

    // 緩衝區操作
    void fillScreen(uint16_t color)
    {
      uint8_t data = (color == GxEPD_BLACK) ? 0x00 : 0xFF;
      for (uint16_t x = 0; x < sizeof(_buffer); x++)
      {
        _buffer[x] = data;
      }
    }

    void display(bool partial_update_mode = false)
    {
      if (partial_update_mode) epd2.writeImage(_buffer, 0, 0, GxEPD2_Type::WIDTH, _page_height);
      else epd2.writeImageForFullRefresh(_buffer, 0, 0, GxEPD2_Type::WIDTH, _page_height);
      epd2.refresh(partial_update_mode);
      if (epd2.hasFastPartialUpdate)
      {
        epd2.writeImageAgain(_buffer, 0, 0, GxEPD2_Type::WIDTH, _page_height);
      }
      if (!partial_update_mode) epd2.powerOff();
    }

    // 視窗管理
    void setFullWindow()
    {
      _using_partial_mode = false;
      _pw_x = 0;
      _pw_y = 0;
      _pw_w = GxEPD2_Type::WIDTH;
      _pw_h = HEIGHT;
    }

    void setPartialWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    {
      _pw_x = gx_uint16_min(x, width());
      _pw_y = gx_uint16_min(y, height());
      _pw_w = gx_uint16_min(w, width() - _pw_x);
      _pw_h = gx_uint16_min(h, height() - _pw_y);
      _rotate(_pw_x, _pw_y, _pw_w, _pw_h);
      _using_partial_mode = true;
      
      _pw_w += _pw_x % 8;
      if (_pw_w % 8 > 0) _pw_w += 8 - _pw_w % 8;
      _pw_x -= _pw_x % 8;
      if (_reverse) _pw_y = HEIGHT - _pw_h - _pw_y;
    }

    // 分頁繪圖
    void firstPage()
    {
      fillScreen(GxEPD_WHITE);
      _current_page = 0;
      _second_phase = false;
    }

    bool nextPage()
    {
      if (1 == _pages)
      {
        if (_using_partial_mode)
        {
          epd2.writeImage(_buffer, _pw_x, _pw_y, _pw_w, _pw_h);
          epd2.refresh(_pw_x, _pw_y, _pw_w, _pw_h);
          if (epd2.hasFastPartialUpdate)
          {
            epd2.writeImageAgain(_buffer, _pw_x, _pw_y, _pw_w, _pw_h);
          }
        }
        else
        {
          epd2.writeImageForFullRefresh(_buffer, 0, 0, GxEPD2_Type::WIDTH, HEIGHT);
          epd2.refresh(false);
          if (epd2.hasFastPartialUpdate)
          {
            epd2.writeImageAgain(_buffer, 0, 0, GxEPD2_Type::WIDTH, HEIGHT);
          }
          epd2.powerOff();
        }
        return false;
      }
      
      uint16_t page_ys = _current_page * _page_height;
      if (_using_partial_mode)
      {
        uint16_t page_ye = _current_page < int16_t(_pages - 1) ? page_ys + _page_height : HEIGHT;
        uint16_t dest_ys = _pw_y + page_ys;
        uint16_t dest_ye = gx_uint16_min(_pw_y + _pw_h, _pw_y + page_ye);
        
        if (dest_ye > dest_ys)
        {
          if (!_second_phase) epd2.writeImage(_buffer, _pw_x, dest_ys, _pw_w, dest_ye - dest_ys);
          else epd2.writeImageAgain(_buffer, _pw_x, dest_ys, _pw_w, dest_ye - dest_ys);
        }
        
        _current_page++;
        if (_current_page == int16_t(_pages))
        {
          _current_page = 0;
          if (!_second_phase)
          {
            epd2.refresh(_pw_x, _pw_y, _pw_w, _pw_h);
            if (epd2.hasFastPartialUpdate)
            {
              _second_phase = true;
              fillScreen(GxEPD_WHITE);
              return true;
            }
          }
          return false;
        }
        fillScreen(GxEPD_WHITE);
        return true;
      }
      else
      {
        if (!_second_phase) epd2.writeImageForFullRefresh(_buffer, 0, page_ys, GxEPD2_Type::WIDTH, gx_uint16_min(_page_height, HEIGHT - page_ys));
        else epd2.writeImageAgain(_buffer, 0, page_ys, GxEPD2_Type::WIDTH, gx_uint16_min(_page_height, HEIGHT - page_ys));
        
        _current_page++;
        if (_current_page == int16_t(_pages))
        {
          _current_page = 0;
          if (epd2.hasFastPartialUpdate)
          {
            if (!_second_phase)
            {
              epd2.refresh(false);
              _second_phase = true;
              fillScreen(GxEPD_WHITE);
              return true;
            }
          } 
          else epd2.refresh(false);
          epd2.powerOff();
          return false;
        }
        fillScreen(GxEPD_WHITE);
        return true;
      }
    }

    // 圖像操作
    void clearScreen(uint8_t value = 0xFF) { epd2.clearScreen(value); }
    void writeScreenBuffer(uint8_t value = 0xFF) { epd2.writeScreenBuffer(value); }
    
    void writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false)
    {
      epd2.writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
    }
    
    void drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false)
    {
      epd2.drawImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
    }

    // 刷新與電源
    void refresh(bool partial_update_mode = false)
    {
      epd2.refresh(partial_update_mode);
      if (!partial_update_mode) epd2.powerOff();
    }
    
    void refresh(int16_t x, int16_t y, int16_t w, int16_t h)
    {
      epd2.refresh(x, y, w, h);
    }
    
    void powerOff() { epd2.powerOff(); }
    void hibernate() { epd2.hibernate(); }

  private:
    template <typename T> static inline void _swap_(T & a, T & b)
    {
      T t = a; a = b; b = t;
    }
    
    static inline uint16_t gx_uint16_min(uint16_t a, uint16_t b)
    {
      return (a < b ? a : b);
    }
    
    void _rotate(uint16_t& x, uint16_t& y, uint16_t& w, uint16_t& h)
    {
      switch (getRotation())
      {
        case 1:
          _swap_(x, y);
          _swap_(w, h);
          x = WIDTH - x - w;
          break;
        case 2:
          x = WIDTH - x - w;
          y = HEIGHT - y - h;
          break;
        case 3:
          _swap_(x, y);
          _swap_(w, h);
          y = HEIGHT - y - h;
          break;
      }
    }

    uint8_t _buffer[(GxEPD2_Type::WIDTH / 8) * page_height];
    bool _using_partial_mode, _second_phase, _mirror, _reverse;
    int16_t _current_page;
    uint16_t _pages, _page_height;
    uint16_t _pw_x, _pw_y, _pw_w, _pw_h;
};

#endif