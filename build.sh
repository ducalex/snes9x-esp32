#!/bin/bash
#make -j 4
release=`date +%Y%m%d`;
../odroid-go-multi-firmware/tools/mkfw/mkfw "Snes9x ($release)" tile.raw 0 16 2097152 app build/snes9x-esp32.bin
mv firmware.fw snes9x.fw
