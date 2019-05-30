#!/bin/bash
make -j 4
release=`date +%Y%m%d`;
../odroid-go-firmware/tools/mkfw/mkfw "Snes9x ($release)" tile.raw 0 16 2097152 app build/esp32-snes9x.bin 
mv firmware.fw snes9x.fw
