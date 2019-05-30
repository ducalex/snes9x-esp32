/* 
 * This file is part of odroid-go-std-lib.
 * Copyright (c) 2019 ducalex.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SPI_LCD_H_
#define _SPI_LCD_H_

typedef struct {
      uint8_t charCode;
      int adjYOffset;
      int width;
      int height;
      int xOffset;
      int xDelta;
      uint16_t dataPtr;
} propFont;

#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_MH 0x04
#define TFT_RGB_BGR 0x08
#define TFT_LED_DUTY_MAX 0x1fff
#define LCD_CMD 0
#define LCD_DATA 1

#define LCD_RGB(r, g, b) \
      (((((r / 255 * 31) & 0x1F) << 11) | (((g / 255 * 62) & 0x3F) << 5) | (((b / 255 * 31) & 0x1F))) << 8 \
      | ((((r / 255 * 31) & 0x1F) << 11) | (((g / 255 * 62) & 0x3F) << 5) | (((b / 255 * 31) & 0x1F))) >> 8) & 0xFFFF

#define LCD_DEFAULT_PALETTE NULL

short backlight_percentage_get(void);
void backlight_percentage_set(short level);

void spi_lcd_init();
void spi_lcd_cmd(uint8_t cmd);
void spi_lcd_write8(uint8_t data);
void spi_lcd_write16(uint16_t data);
void spi_lcd_write(uint8_t *data, int len, int dc);
void spi_lcd_setWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void spi_lcd_wait_finish();
void spi_lcd_usePalette(bool use);
void spi_lcd_setPalette(const int16_t *palette);
void spi_lcd_clear();
void spi_lcd_drawPixel(int x, int y, uint16_t color);
void spi_lcd_fill(int x, int y, int w, int h, uint16_t color);
void spi_lcd_setFont(const uint8_t *font);
void spi_lcd_setFontColor(uint16_t color);
void spi_lcd_print(int x, int y, char *string);
void spi_lcd_printf(int x, int y, char *string, ...);
void spi_lcd_useFrameBuffer(bool use);
void spi_lcd_fb_setPtr(void *buffer);
void*spi_lcd_fb_getPtr();
void spi_lcd_fb_free();
void spi_lcd_fb_alloc();
void spi_lcd_fb_clear();
void spi_lcd_fb_flush(); // fb_flush sends the buffer to the display now, it's synchronous
void spi_lcd_fb_update(); // fb_update tells the display task it's time to redraw, it's async
void spi_lcd_fb_write(void *buffer);

#endif