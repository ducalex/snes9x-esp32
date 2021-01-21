# This port is now abandoned.
A new port is being worked on as part of [retro-go](https://github.com/ducalex/retro-go/) and is already in better shape than this one.

# Snes9x for Odroid GO
This is a port of the snes9x emulator to the Odroid Go.

You will notice that I butchered the snes9x codebase and that was for multiple reasons such as: fixing compile errors, smaller binary, performance, simplicity.

Once (If) I get the emulator to run at an acceptable speed I will bring the code back to as close to upstream as possible.

## Things to note:
- It is very slow  (currently ~4x too slow)
- No sound
- No saves
- ROMs are limited to 2MB
- No enhancement chip support (DSP will be supported eventually, SA/FX probably not)

## Device support
Although only the Odroid GO is officially supported, it is confirmed to also run on a standard ESP32-**WROVER** with an ILI9341 LCD. 

Careful: Most ESP32 dev boards use the ESP32-**WROOM** modules and it won't work for this project (there is not enough memory).


# Photo

![Super mario world](https://i.imgur.com/xmtncsX.jpg)
