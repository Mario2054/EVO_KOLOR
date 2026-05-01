#pragma once
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <U8g2lib.h>

#ifndef SPI_MOSI_OLED
#define SPI_MOSI_OLED 39
#endif
#ifndef SPI_MISO_OLED
#define SPI_MISO_OLED -1
#endif
#ifndef SPI_SCK_OLED
#define SPI_SCK_OLED 38
#endif
#ifndef CS_OLED
#define CS_OLED -1
#endif
#ifndef DC_OLED
#define DC_OLED 40
#endif
#ifndef RESET_OLED
#define RESET_OLED 41
#endif

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 480
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 320
#endif

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9488 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 27000000;
      cfg.freq_read = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = SPI_SCK_OLED;
      cfg.pin_mosi = SPI_MOSI_OLED;
      cfg.pin_miso = SPI_MISO_OLED;
      cfg.pin_dc = DC_OLED;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = CS_OLED;       // -1, bo CS jest fizycznie do GND
      cfg.pin_rst = RESET_OLED;
      cfg.pin_busy = -1;
      cfg.panel_width = 320;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = false;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance);
  }

  void setContrast(uint8_t value) { setBrightness(value); }
  void display(void) {}
  void clear(void) { fillScreen(TFT_BLACK); }
};

extern LGFX display;

class EvoU8G2Compat {
public:
  int16_t cursorX = 0;
  int16_t cursorY = 0;
  uint16_t drawColor = TFT_WHITE;

  int sx(int x) const { return (int)((int32_t)x * 480L / 256L); }
  int sy(int y) const { return (int)((int32_t)y * 320L / 64L); }
  int sw(int w) const { return max(1, (int)((int32_t)w * 480L / 256L)); }
  int sh(int h) const { return max(1, (int)((int32_t)h * 320L / 64L)); }

  uint16_t fg() const { return drawColor ? TFT_WHITE : TFT_BLACK; }
  uint16_t bg() const { return drawColor ? TFT_BLACK : TFT_WHITE; }

  void begin() {}
  void clearBuffer() { display.fillScreen(TFT_BLACK); }
  void sendBuffer() {}
  void setPowerSave(uint8_t) {}
  void setContrast(uint8_t v) { display.setBrightness(v); }
  void setFont(const uint8_t*) { display.setTextSize(1); }
  void setFontMode(uint8_t) {}
  void setFontDirection(uint8_t) {}
  void setDrawColor(uint8_t c) { drawColor = c ? TFT_WHITE : TFT_BLACK; }
  void setCursor(int x, int y) { cursorX = x; cursorY = y; }

  int getDisplayWidth() { return 256; }
  int getDisplayHeight() { return 64; }
  int getMaxCharHeight() { return 12; }
  int getStrWidth(const char* s) { return s ? strlen(s) * 6 : 0; }

  void drawStr(int x, int y, const char* s) {
    if (!s) return;
    display.setTextSize(1);
    display.setTextColor(fg(), bg());
    display.setCursor(sx(x), sy(y) - 10);
    display.print(s);
  }
  void drawUTF8(int x, int y, const char* s) { drawStr(x, y, s); }

  void print(const String& s) {
    display.setTextSize(1);
    display.setTextColor(fg(), bg());
    display.setCursor(sx(cursorX), sy(cursorY) - 10);
    display.print(s);
  }
  void print(const char* s) { print(String(s)); }
  void print(int v) { print(String(v)); }
  void print(unsigned int v) { print(String(v)); }
  void print(long v) { print(String(v)); }
  void print(unsigned long v) { print(String(v)); }

  void printf(const char* fmt, ...) {
    char buf[180];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    print(buf);
  }

  void drawBox(int x, int y, int w, int h) { display.fillRect(sx(x), sy(y), sw(w), sh(h), fg()); }
  void drawFrame(int x, int y, int w, int h) { display.drawRect(sx(x), sy(y), sw(w), sh(h), fg()); }
  void drawRFrame(int x, int y, int w, int h, int r) { display.drawRoundRect(sx(x), sy(y), sw(w), sh(h), max(1, sw(r)), fg()); }
  void drawRBox(int x, int y, int w, int h, int r) { display.fillRoundRect(sx(x), sy(y), sw(w), sh(h), max(1, sw(r)), fg()); }
  void drawLine(int x0, int y0, int x1, int y1) { display.drawLine(sx(x0), sy(y0), sx(x1), sy(y1), fg()); }
  void drawHLine(int x, int y, int w) { display.drawFastHLine(sx(x), sy(y), sw(w), fg()); }
  void drawVLine(int x, int y, int h) { display.drawFastVLine(sx(x), sy(y), sh(h), fg()); }
  void drawPixel(int x, int y) { display.drawPixel(sx(x), sy(y), fg()); }
  void drawCircle(int x, int y, int r) { display.drawCircle(sx(x), sy(y), max(1, sw(r)), fg()); }
  void drawDisc(int x, int y, int r) { display.fillCircle(sx(x), sy(y), max(1, sw(r)), fg()); }
  void drawEllipse(int x, int y, int rx, int ry) { display.drawEllipse(sx(x), sy(y), max(1, sw(rx)), max(1, sh(ry)), fg()); }
  void drawFilledEllipse(int x, int y, int rx, int ry) { display.fillEllipse(sx(x), sy(y), max(1, sw(rx)), max(1, sh(ry)), fg()); }
  void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) { display.drawTriangle(sx(x0), sy(y0), sx(x1), sy(y1), sx(x2), sy(y2), fg()); }
  void drawGlyph(int x, int y, uint16_t code) { char c[2] = {(char)code, 0}; drawStr(x, y, c); }
  void drawXBMP(int x, int y, int w, int h, const uint8_t*) { drawFrame(x, y, w, h); }
};

extern EvoU8G2Compat u8g2;
