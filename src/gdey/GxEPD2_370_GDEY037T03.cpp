// Display Library for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines! 
//
// based on Demo Example from Good Display: https://www.good-display.com/product/437.html
// Panel: GDEY037T03 : https://www.good-display.com/product/437.html
// Controller: UC8253 : https://v4.cecdn.yun300.cn/100001_1909185148/UC8253.pdf
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

#include "GxEPD2_370_GDEY037T03.h"

GxEPD2_370_GDEY037T03::GxEPD2_370_GDEY037T03(int16_t cs, int16_t dc, int16_t rst, int16_t busy) :
  GxEPD2_EPD(cs, dc, rst, busy, LOW, 10000000, WIDTH, HEIGHT, panel, hasColor, hasPartialUpdate, hasFastPartialUpdate)
{
}

void GxEPD2_370_GDEY037T03::clearScreen(uint8_t value)
{
  // full refresh needed for all cases (previous != screen)
  _writeScreenBuffer(0x10, value); // set previous buffer
  _writeScreenBuffer(0x13, value); // set current buffer
  refresh(false);                  // full refresh
  _initial_write = false;
}

void GxEPD2_370_GDEY037T03::writeScreenBuffer(uint8_t value)
{
  if (_initial_write) {
    // on first write, perform clearScreen to initialize both buffers and do a full update
    return clearScreen(value);
  }
  _writeScreenBuffer(0x13, value); // set current buffer
}

void GxEPD2_370_GDEY037T03::writeScreenBufferAgain(uint8_t value)
{
  _writeScreenBuffer(0x10, value); // set previous buffer
  // _writeScreenBuffer(0x13, value); // not needed to set current again
}

void GxEPD2_370_GDEY037T03::_writeScreenBuffer(uint8_t command, uint8_t value)
{
  if (!_init_display_done) _InitDisplay();
  _writeCommand(command);
  _startTransfer();
  for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++) {
    _transfer(value);
  }
  _endTransfer();
}

void GxEPD2_370_GDEY037T03::writeImage(const uint8_t bitmap[], int16_t x, int16_t y,
                                      int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  _writeImage(0x13, bitmap, x, y, w, h, invert, mirror_y, pgm);  // write to current buffer
}

void GxEPD2_370_GDEY037T03::writeImageForFullRefresh(const uint8_t bitmap[], int16_t x, int16_t y,
                                                    int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  // write the same image data to both previous and current buffers for a clean full refresh
  _writeImage(0x10, bitmap, x, y, w, h, invert, mirror_y, pgm);  // set previous buffer
  _writeImage(0x13, bitmap, x, y, w, h, invert, mirror_y, pgm);  // set current buffer
}

void GxEPD2_370_GDEY037T03::writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part,
                                          int16_t w_bitmap, int16_t h_bitmap,
                                          int16_t x, int16_t y, int16_t w, int16_t h,
                                          bool invert, bool mirror_y, bool pgm)
{
  _writeImagePart(0x13, bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_370_GDEY037T03::writeImage(const uint8_t* black, const uint8_t* color, 
                                      int16_t x, int16_t y, int16_t w, int16_t h,
                                      bool invert, bool mirror_y, bool pgm)
{
  // Only handle black channel for monochrome display
  if (black) {
    writeImage(black, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_370_GDEY037T03::writeImagePart(const uint8_t* black, const uint8_t* color,
                                          int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                          int16_t x, int16_t y, int16_t w, int16_t h,
                                          bool invert, bool mirror_y, bool pgm)
{
  if (black) {
    writeImagePart(black, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_370_GDEY037T03::writeNative(const uint8_t* data1, const uint8_t* data2,
                                       int16_t x, int16_t y, int16_t w, int16_t h,
                                       bool invert, bool mirror_y, bool pgm)
{
  // For monochrome, treat data1 as the black channel
  if (data1) {
    writeImage(data1, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_370_GDEY037T03::writeImageAgain(const uint8_t bitmap[], int16_t x, int16_t y,
                                           int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  // Copy image to previous buffer for differential updates (fast partial)
  _writeImage(0x10, bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_370_GDEY037T03::writeImagePartAgain(const uint8_t bitmap[], int16_t x_part, int16_t y_part,
                                               int16_t w_bitmap, int16_t h_bitmap,
                                               int16_t x, int16_t y, int16_t w, int16_t h,
                                               bool invert, bool mirror_y, bool pgm)
{
  _writeImagePart(0x10, bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_370_GDEY037T03::drawImage(const uint8_t bitmap[], int16_t x, int16_t y,
                                     int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImageAgain(bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_370_GDEY037T03::drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part,
                                         int16_t w_bitmap, int16_t h_bitmap,
                                         int16_t x, int16_t y, int16_t w, int16_t h,
                                         bool invert, bool mirror_y, bool pgm)
{
  writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImagePartAgain(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_370_GDEY037T03::drawImage(const uint8_t* black, const uint8_t* color,
                                     int16_t x, int16_t y, int16_t w, int16_t h,
                                     bool invert, bool mirror_y, bool pgm)
{
  if (black) {
    drawImage(black, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_370_GDEY037T03::drawImagePart(const uint8_t* black, const uint8_t* color,
                                         int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                         int16_t x, int16_t y, int16_t w, int16_t h,
                                         bool invert, bool mirror_y, bool pgm)
{
  if (black) {
    drawImagePart(black, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_370_GDEY037T03::drawNative(const uint8_t* data1, const uint8_t* data2,
                                      int16_t x, int16_t y, int16_t w, int16_t h,
                                      bool invert, bool mirror_y, bool pgm)
{
  if (data1) {
    drawImage(data1, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_370_GDEY037T03::refresh(bool partial_update_mode)
{
  if (partial_update_mode) {
    // if partial update requested, refresh only the changed area by doing a full update on that area
    refresh(0, 0, WIDTH, HEIGHT);
  } else {
    _Update_Full();
    _initial_refresh = false; // initial full update done
  }
}

void GxEPD2_370_GDEY037T03::refresh(int16_t x, int16_t y, int16_t w, int16_t h)
{
  if (_initial_refresh) {
    // The first refresh must be a full update
    return refresh(false);
  }
  // compute intersection of requested area with screen
  int16_t w1 = (x < 0) ? w + x : w;
  int16_t h1 = (y < 0) ? h + y : h;
  int16_t x1 = (x < 0) ? 0 : x;
  int16_t y1 = (y < 0) ? 0 : y;
  w1 = (x1 + w1 < (int16_t)WIDTH) ? w1 : (int16_t)WIDTH - x1;
  h1 = (y1 + h1 < (int16_t)HEIGHT) ? h1 : (int16_t)HEIGHT - y1;
  if ((w1 <= 0) || (h1 <= 0)) return;
  // align x1 and w1 to byte boundaries (multiples of 8)
  w1 += x1 % 8;
  if (w1 % 8 > 0) w1 += 8 - (w1 % 8);
  x1 -= x1 % 8;
  if (usePartialUpdateWindow) _writeCommand(0x91); // partial in
  _setPartialRamArea(x1, y1, w1, h1);
  _Update_Part();
  if (usePartialUpdateWindow) _writeCommand(0x92); // partial out
}

void GxEPD2_370_GDEY037T03::powerOff()
{
  _PowerOff();
}

void GxEPD2_370_GDEY037T03::hibernate()
{
  _PowerOff();
  if (_rst >= 0) {
    _writeCommand(0x07);   // deep sleep
    _writeData(0xA5);      // check code (required value for deep sleep)
    _hibernating = true;
    _init_display_done = false;
  }
}

void GxEPD2_370_GDEY037T03::_setPartialRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  uint16_t xe = (x + w - 1) | 0x0007;  // last byte boundary (inclusive)
  uint16_t ye = y + h - 1;
  x &= 0xFFF8;                        // first byte boundary (start)
  _writeCommand(0x90);                // PARTIAL WINDOW setting
  _writeData(x);
  _writeData(xe);
  _writeData(y / 256);
  _writeData(y % 256);
  _writeData(ye / 256);
  _writeData(ye % 256);
  _writeData(0x01);                   // enable partial window
}

void GxEPD2_370_GDEY037T03::_writeImage(uint8_t command, const uint8_t bitmap[],
                                       int16_t x, int16_t y, int16_t w, int16_t h,
                                       bool invert, bool mirror_y, bool pgm)
{
  // Limit the area to write within the display bounds
  int16_t x_end = x + w;
  int16_t y_end = y + h;
  // Clip width and height if the image goes beyond the display area
  w = (x_end > (int16_t)WIDTH) ? ((int16_t)WIDTH - x) : w;
  h = (y_end > (int16_t)HEIGHT) ? ((int16_t)HEIGHT - y) : h;
  // Align x and w to byte boundaries (multiple of 8 pixels)
  x -= x % 8;
  w = 8 * ((w + 7) / 8);
  // If nothing to write after clipping, return
  if (w <= 0 || h <= 0) return;
  if (!_init_display_done) _InitDisplay();
  if (_initial_write) {
    // Ensure the screen buffer is cleared on first image write
    writeScreenBuffer();
  }
  // Use partial mode to set drawing area
  if (usePartialUpdateWindow) _writeCommand(0x91);  // partial in
  _setPartialRamArea(x, y, w, h);
  _writeCommand(command);
  _startTransfer();
  // Write each line of the image to the controller
  uint16_t bytes_per_line = w / 8;
  for (int16_t line = 0; line < h; line++) {
    int16_t src_line = mirror_y ? (h - 1 - line) : line;
    // Calculate starting index in bitmap array for this line
    uint32_t idx = pgm 
                   ? src_line * (uint32_t)(w / 8) 
                   : src_line * (uint32_t)(w / 8);
    // Actually, above calculation can be simplified as idx = src_line * bytes_per_line (since x is aligned to byte boundary and we consider only given bitmap slice)
    // But since this _writeImage is intended for writing the entire provided bitmap (assumed sized exactly w x h), we can use this direct indexing.
    idx = src_line * bytes_per_line;
    for (int16_t byte = 0; byte < bytes_per_line; byte++) {
      uint8_t data;
      if (pgm) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
        data = pgm_read_byte(&bitmap[idx + byte]);
#else
        data = bitmap[idx + byte];
#endif
      } else {
        data = bitmap[idx + byte];
      }
      if (invert) data = ~data;
      _transfer(data);
    }
  }
  _endTransfer();
  if (usePartialUpdateWindow) _writeCommand(0x92);  // partial out
  delay(1); // small delay to ensure internal processing, avoid watchdog reset on some MCUs
}

void GxEPD2_370_GDEY037T03::_writeImagePart(uint8_t command, const uint8_t bitmap[],
                                           int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                           int16_t x, int16_t y, int16_t w, int16_t h,
                                           bool invert, bool mirror_y, bool pgm)
{
  // Limit the source region [x_part, y_part, w, h] to the source bitmap dimensions
  w = (w_bitmap - x_part < w) ? (w_bitmap - x_part) : w;
  h = (h_bitmap - y_part < h) ? (h_bitmap - y_part) : h;
  // Align target x and width to byte boundaries
  x -= x % 8;
  w = 8 * ((w + 7) / 8);
  // Clip the target area to display bounds
  int16_t x1 = (x < 0) ? 0 : x;
  int16_t y1 = (y < 0) ? 0 : y;
  int16_t w1 = (x + w < (int16_t)WIDTH) ? w : (int16_t)WIDTH - x;
  int16_t h1 = (y + h < (int16_t)HEIGHT) ? h : (int16_t)HEIGHT - y;
  // Adjust source offsets if target was clipped
  int16_t dx = x1 - x;
  int16_t dy = y1 - y;
  w1 -= dx;
  h1 -= dy;
  if (w1 <= 0 || h1 <= 0) return;
  if (!_init_display_done) _InitDisplay();
  if (_initial_write) {
    writeScreenBuffer(); // clear on first use
  }
  _writeCommand(0x91); // enter partial mode (using partial window for write operations)
  _setPartialRamArea(x1, y1, w1, h1);
  _writeCommand(command);
  _startTransfer();
  uint16_t bytes_per_line_src = w_bitmap / 8;
  for (int16_t i = 0; i < h1; i++) {
    for (int16_t j = 0; j < w1 / 8; j++) {
      uint8_t data;
      // Calculate index in source bitmap:
      // If mirror_y, take lines from bottom of the source region; otherwise top.
      uint16_t src_line = mirror_y ? (h_bitmap - 1 - (y_part + i + dy)) : (y_part + i + dy);
      uint32_t idx = (uint32_t)(x_part / 8 + j + dx / 8) + (uint32_t)src_line * bytes_per_line_src;
      if (pgm) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
        data = pgm_read_byte(&bitmap[idx]);
#else
        data = bitmap[idx];
#endif
      } else {
        data = bitmap[idx];
      }
      if (invert) data = ~data;
      _transfer(data);
    }
  }
  _endTransfer();
  _writeCommand(0x92); // exit partial mode
  delay(1);
}

void GxEPD2_370_GDEY037T03::_PowerOn()
{
  if (!_power_is_on) {
    _writeCommand(0x04); // POWER ON
    _waitWhileBusy("_PowerOn", power_on_time);
  }
  _power_is_on = true;
}

void GxEPD2_370_GDEY037T03::_PowerOff()
{
  if (_power_is_on) {
    _writeCommand(0x02); // POWER OFF
    _waitWhileBusy("_PowerOff", power_off_time);
  }
  _power_is_on = false;
}

void GxEPD2_370_GDEY037T03::_InitDisplay()
{
  if (_hibernating) {
    _reset();  // reset if waking from deep sleep
  } else {
    // Panel soft reset (if not coming from deep sleep)
    _writeCommand(0x00); // PANEL SETTING
    _writeData(0x1e);    // soft reset
    _writeData(0x0d);
    delay(1);
  }
  _power_is_on = false;
  // Set panel settings (KW/R, etc)
  _writeCommand(0x00);   // PANEL SETTING
  _writeData(0x1f);      // KW: 3f, KWR: 2F, BWROTP: 0f, BWOTP: 1f (for UC8253)
  _writeData(0x0d);
  _init_display_done = true;
}

void GxEPD2_370_GDEY037T03::_Update_Full()
{
  if (useFastFullUpdate) {
    _writeCommand(0xE0); // Cascade Setting (CLK Timing)
    _writeData(0x02);    // set TSFIX (timing)
    _writeCommand(0xE5); // Force Temperature (TSSET)
    _writeData(0x5A);    // 0x5A for fast update (~1.5s at 25°C)
    // _writeData(0x6E); // uncomment if useFastFullUpdate == false for slower update (~2.0s at 25°C)
  }
  _writeCommand(0x50);
  _writeData(0x97);
  _PowerOn();
  _writeCommand(0x12);   // DISPLAY REFRESH
  _waitWhileBusy("_Update_Full", full_refresh_time);
  _init_display_done = false; // reset init flag (controller needs re-init after full refresh)
}

void GxEPD2_370_GDEY037T03::_Update_Part()
{
  if (hasFastPartialUpdate) {
    _writeCommand(0xE0); // Cascade Setting (CLK Timing)
    _writeData(0x02);    // set TSFIX (timing)
    _writeCommand(0xE5); // Force Temperature (TSSET)
    _writeData(0x79);    // 0x79 for partial update waveform (fast partial)
  }
  _writeCommand(0x50);
  _writeData(0xD7);
  _PowerOn();
  _writeCommand(0x12);   // DISPLAY REFRESH
  _waitWhileBusy("_Update_Part", partial_refresh_time);
  _init_display_done = false;
}
