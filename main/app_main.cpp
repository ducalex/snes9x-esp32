#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define HAVE_STRINGS_H
#define HAVE_STDINT_H

#include "snes9x.h"
#include "memmap.h"

extern "C" {
	#define ODROID_TASKS_USE_CORE 0
	#include "odroid.h"
	#include "esp_system.h"
	#include "esp_heap_caps.h"
	#include "freertos/FreeRTOS.h"
	#include "freertos/task.h"
}

extern char *rom_selector();
extern void snes_task(void*);


extern "C" void app_main()
{
	// I tried reducing DMA, Reserved memory, and whatnot but it just won't allocate a continuous 128K segment :(
	// Internal ram is a bit faster than spiram + it makes room for loading bigger ROMS
	Memory.RAM1 = (uint8 *) heap_caps_malloc(0x10000, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    Memory.RAM2 = (uint8 *) heap_caps_malloc(0x10000, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
	
	//heap_caps_malloc_extmem_enable(32768);

	odroid_system_init();

	spi_lcd_useFrameBuffer(true);
	
	char *rom_file = rom_selector();
	
	spi_lcd_setWindow(32, 0, 256, 240);
	spi_lcd_print(0, 0, "Starting SNES...");
	spi_lcd_fb_update();

	xTaskCreatePinnedToCore(&snes_task, "snes_task", 1024 * 32, rom_file, 10, NULL, 1);
	vTaskDelete(NULL);
}



void _splitpath (const char *path, char *drive, char *dir, char *fname, char *ext)
{
	*drive = 0;

	const char	*slash = strrchr(path, SLASH_CHAR),
				*dot   = strrchr(path, '.');

	if (dot && slash && dot < slash)
		dot = NULL;

	if (!slash)
	{
		*dir = 0;

		strcpy(fname, path);

		if (dot)
		{
			fname[dot - path] = 0;
			strcpy(ext, dot + 1);
		}
		else
			*ext = 0;
	}
	else
	{
		strcpy(dir, path);
		dir[slash - path] = 0;

		strcpy(fname, slash + 1);

		if (dot)
		{
			fname[dot - slash - 1] = 0;
			strcpy(ext, dot + 1);
		}
		else
			*ext = 0;
	}
}

void _makepath (char *path, const char *, const char *dir, const char *fname, const char *ext)
{
	if (dir && *dir)
	{
		strcpy(path, dir);
		strcat(path, SLASH_STR);
	}
	else
		*path = 0;

	strcat(path, fname);

	if (ext && *ext)
	{
		strcat(path, ".");
		strcat(path, ext);
	}
}
