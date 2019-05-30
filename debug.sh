#!/bin/sh
. ${IDF_PATH}/add_path.sh

COMPORT=COM4
BINNAME=snes9x-esp32

#make
make -j 8 || exit
#make flash
esptool.py --chip esp32 --port $COMPORT --before default_reset --after hard_reset erase_region 0x100000 2097152 || exit
esptool.py --chip esp32 --port $COMPORT --baud 921600 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size detect 0x100000 "build/$BINNAME.bin" || exit
#make monitor
idf_monitor.py --port COM4 build/$BINNAME.elf
