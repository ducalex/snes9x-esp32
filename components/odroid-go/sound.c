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

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "odroid.h"

#define SAMPLESIZE 2 // 16bit
#define SAMPLERATE 22050

static bool sound_initialized = false;


void odroid_sound_init(void)
{
    if (sound_initialized) return;
    
    printf("odroid_sound_init: Initializing sound.\n");

    static const i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | /*I2S_MODE_PDM,*/ I2S_MODE_DAC_BUILT_IN,
        .sample_rate = SAMPLERATE,
        .bits_per_sample = SAMPLESIZE * 8, /* the DAC module will only take the 8bits from MSB */
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 2,
        .dma_buf_len = 512, // (2) 16bit channels
        .use_apll = false
    };

    esp_err_t ret = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);   //install and start i2s driver

    if (ret != ESP_OK) {
        odroid_fatal_error("odroid_sound_init: Failed to initialize I2S.");
    }

    i2s_set_pin(I2S_NUM_0, NULL); //for internal DAC, this will enable both of the internal channels

    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
}


void odroid_sound_deinit()
{
    i2s_driver_uninstall(I2S_NUM_0);
    sound_initialized = false;
}


size_t * odroid_sound_write(void *buffer, size_t length)
{
    size_t bytesWritten = 0;
    i2s_write(I2S_NUM_0, buffer, length, &bytesWritten, portMAX_DELAY);
    return bytesWritten;
}


void odroid_sound_set_sample_rate(uint32_t rate)
{
    i2s_set_sample_rates(I2S_NUM_0, rate); //set sample rates
}


void odroid_sound_set_volume(uint32_t rate)
{
    
}
