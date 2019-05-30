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

#ifndef _ODROID_H_
#define _ODROID_H_

#include "esp_heap_caps.h"
#include "spi_lcd.h"
#include "sdcard.h"
#include "sound.h"
#include "input.h"

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_DC 21
#define PIN_NUM_LCD_CS 5
#define PIN_NUM_SD_CS 22
#define PIN_NUM_LCD_BCKL 14
#define SPI_DMA_CHANNEL 2

#ifndef ODROID_TASKS_USE_CORE
#define ODROID_TASKS_USE_CORE 0
#endif

size_t free_bytes_total();
size_t free_bytes_internal();
size_t free_bytes_spiram();

void odroid_system_init();
void odroid_system_led_set(int value);
void odroid_spi_bus_acquire();
void odroid_spi_bus_release();
void odroid_fatal_error(char *error);

#endif