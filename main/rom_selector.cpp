extern "C" {
    #include <stdlib.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <dirent.h>

    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_err.h"
    #include "esp_attr.h"

    #include "odroid.h"

    #include "fonts/DejaVuSans18.h"
    #include "fonts/DejaVu12.h"
}

static int selected = 0;
static bool a_pressed = false;
static int files_count = 0;

static void input_callback(odroid_input_state state)
{
	if (state.values[ODROID_INPUT_UP])
		if (--selected < 0) selected = 0;
	if (state.values[ODROID_INPUT_DOWN])
		if (++selected >= files_count) selected = files_count - 1;
	if (state.values[ODROID_INPUT_A])
		a_pressed = true;
}

char * rom_selector()
{
    spi_lcd_setFontColor(0xFFFF);
	spi_lcd_setFont(tft_Dejavu18);

	DIR *dir;
	struct dirent *ent;
	char files[32][64];
	
	odroid_spi_bus_acquire();
	if ((dir = opendir("/sdcard/roms/snes")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
            memcpy(files[files_count++], ent->d_name, 64);
		}
		closedir (dir);
	}
	odroid_spi_bus_release();

    odroid_input_set_callback(&input_callback);

    while(1) {
        if (a_pressed) {
            char path[128];
            sprintf(path, "/sdcard/roms/snes/%s", files[selected]);
            spi_lcd_clear();
            spi_lcd_fb_flush();
            return strdup(path);
        }

        int row = 0;
        int offset = 0;
        char buffer[30];

        if (selected > 10) {
            offset = selected - 10;
        }

        spi_lcd_clear();
        spi_lcd_print(0, row++ * 20, "Please select a ROM:");

        for (int i = offset; i < files_count; i++) {
            spi_lcd_print(5, row * 20, (char *)(selected == i ? ">" : " "));
            strncpy(buffer, files[i], 30);
            buffer[29] = 0;
            spi_lcd_print(20, row++ * 20, buffer);
        }

        spi_lcd_fb_flush();
        // vTaskDelay
    }
}