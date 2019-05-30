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

#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include "odroid.h"

static odroid_input_state gamepad_state = {0};
static bool input_gamepad_initialized = false;
static void (*odroid_input_callback_fn)(odroid_input_state) = NULL;


void odroid_input_read_raw(uint8_t *values)
{
	memset(values, 0, ODROID_INPUT_MAX);

    int joyX = adc1_get_raw(ODROID_GAMEPAD_IO_X);
    int joyY = adc1_get_raw(ODROID_GAMEPAD_IO_Y);

    if (joyX > 2048 + 1024)
        values[ODROID_INPUT_LEFT] = 1;
    else if (joyX > 1024)
        values[ODROID_INPUT_RIGHT] = 1;

    if (joyY > 2048 + 1024)
        values[ODROID_INPUT_UP] = 1;
    else if (joyY > 1024)
        values[ODROID_INPUT_DOWN] = 1;

    values[ODROID_INPUT_SELECT] = !(gpio_get_level(ODROID_GAMEPAD_IO_SELECT));
    values[ODROID_INPUT_START] = !(gpio_get_level(ODROID_GAMEPAD_IO_START));

    values[ODROID_INPUT_A] = !(gpio_get_level(ODROID_GAMEPAD_IO_A));
    values[ODROID_INPUT_B] = !(gpio_get_level(ODROID_GAMEPAD_IO_B));

    values[ODROID_INPUT_MENU] = !(gpio_get_level(ODROID_GAMEPAD_IO_MENU));
    values[ODROID_INPUT_VOLUME] = !(gpio_get_level(ODROID_GAMEPAD_IO_VOLUME));
}


static void odroid_input_task(void *arg)
{
	printf("odroid_input_task: Input task started.\n");

    int i, changes;

    while(1)
    {
        // Read hardware
        odroid_input_read_raw(&gamepad_state.realtime);
        changes = 0;

        // Debounce
        for(i = 0; i < ODROID_INPUT_MAX; ++i)
		{
			gamepad_state.debounce[i] <<= 1;

            gamepad_state.debounce[i] |= gamepad_state.realtime[i] ? 1 : 0;
            uint8_t val = gamepad_state.debounce[i] & 0x03; //0x0f;
            switch (val) {
                case 0x00:
                    gamepad_state.values[i] = 0;
                    break;

                case 0x03: //0x0f:
                    gamepad_state.values[i] = 1;
                    break;

                default:
                    // ignore
                    break;
            }

            if (gamepad_state.values[i] != gamepad_state.previous[i]) {
                changes++;
            }
		}
		
        if (changes > 0 && odroid_input_callback_fn != NULL) {
            (*odroid_input_callback_fn)(gamepad_state);
            // update previous only if there's a callback, otherwise get_state will do it;
            memcpy(gamepad_state.previous, gamepad_state.values, ODROID_INPUT_MAX);
        }

        // delay
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}


void odroid_input_get_state(odroid_input_state *dst)
{
    memcpy(dst, &gamepad_state, sizeof(gamepad_state));
    memcpy(gamepad_state.previous, gamepad_state.values, ODROID_INPUT_MAX);
}


void odroid_input_set_callback(void (*callback)(odroid_input_state))
{
    odroid_input_callback_fn = callback;
}


void odroid_input_init(void)
{
	if (input_gamepad_initialized) {
		return;
	}

    printf("odroid_input_init: Initializing input.\n");

    gpio_set_direction(ODROID_GAMEPAD_IO_SELECT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ODROID_GAMEPAD_IO_SELECT, GPIO_PULLUP_ONLY);

    gpio_set_direction(ODROID_GAMEPAD_IO_START, GPIO_MODE_INPUT);

    gpio_set_direction(ODROID_GAMEPAD_IO_A, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ODROID_GAMEPAD_IO_A, GPIO_PULLUP_ONLY);

    gpio_set_direction(ODROID_GAMEPAD_IO_B, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ODROID_GAMEPAD_IO_B, GPIO_PULLUP_ONLY);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ODROID_GAMEPAD_IO_X, ADC_ATTEN_11db);
    adc1_config_channel_atten(ODROID_GAMEPAD_IO_Y, ADC_ATTEN_11db);

    rtc_gpio_deinit(ODROID_GAMEPAD_IO_MENU);
    gpio_set_direction(ODROID_GAMEPAD_IO_MENU, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ODROID_GAMEPAD_IO_MENU, GPIO_PULLUP_ONLY);

    gpio_set_direction(ODROID_GAMEPAD_IO_VOLUME, GPIO_MODE_INPUT);

    input_gamepad_initialized = true;

    // Start background polling
    xTaskCreatePinnedToCore(&odroid_input_task, "odroid_input_task", 1024 * 2, NULL, 6, NULL, ODROID_TASKS_USE_CORE);
}
