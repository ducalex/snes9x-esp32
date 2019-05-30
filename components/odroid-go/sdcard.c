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

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "odroid.h"

static bool sdcard_initialized = false;


void odroid_sdcard_init()
{
    if (sdcard_initialized) return;
    
    printf("odroid_sdcard_init: Initializing SD Card.\n");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = HSPI_HOST; // HSPI_HOST;
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;

    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = PIN_NUM_MISO;
    slot_config.gpio_mosi = PIN_NUM_MOSI;
    slot_config.gpio_sck  = PIN_NUM_CLK;
    slot_config.gpio_cs   = PIN_NUM_SD_CS;
    slot_config.dma_channel = SPI_DMA_CHANNEL;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 8
    };

    sdmmc_card_t* card;

    odroid_spi_bus_acquire();
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    odroid_spi_bus_release();

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            odroid_fatal_error("odroid_sdcard_init: Failed to mount filesystem.");
        } else {
            odroid_fatal_error("odroid_sdcard_init: Failed to initialize the card.");
        }
    } else {
        sdcard_initialized = true;
    }
}


int odroid_sdcard_read_file(char const *name, char **buffer)
{
    odroid_spi_bus_acquire();
    
    FILE *fp = fopen(name, "rb");
    
    if (fp) {
        fseek(fp, 0, SEEK_END);
        int length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        *buffer = malloc(length);
        int read = fread(*buffer, 1, length, fp);
        fclose(fp);

        if (read != length) {
            printf("odroid_sdcard_read_file: Length mismatch (Got %d, expected %d)\n", read, length);
        }
        
        odroid_spi_bus_release();
        return read;
    }
    
    printf("odroid_sdcard_read_file: failed to open %s\n", name);

    odroid_spi_bus_release();
    return -1;
}


int odroid_sdcard_write_file(char const *name, void *source, int length)
{
    odroid_spi_bus_acquire();
    FILE *fp = fopen(name, "wb");
    
    if (fp) {
        int written = fwrite(source, 1, length, fp);   // Write data
        fclose(fp);
        
        if (written != length) {
            printf("odroid_sdcard_write_file: Length mismatch (Got %d, expected %d)\n", written, length);
        }
        
        odroid_spi_bus_release();
        return written;
    }

    printf("odroid_sdcard_write_file: failed to open %s\n", name);

    odroid_spi_bus_release();
    return -1;
}


int odroid_sdcard_mkdir(char const *path)
{
    odroid_spi_bus_acquire();
    int ret = mkdir(path, 0755);
    odroid_spi_bus_release();

    return ret;
}


int odroid_sdcard_remove(char const *path)
{
    odroid_spi_bus_acquire();
    int ret = remove(path);
    odroid_spi_bus_release();

    return ret;
}


int odroid_sdcard_list_files(char const *directory, char **buffer)
{
    return 0;
}
