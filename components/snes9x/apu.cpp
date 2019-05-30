/*****************************************************************************\
     Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
                This file is licensed under the Snes9x License.
   For further information, consult the LICENSE file in the root directory.
\*****************************************************************************/

#include <cmath>
#include "snes9x.h"
#include "apu.h"
static const int APU_DEFAULT_INPUT_RATE = 31950; // ~59.94Hz
static const int APU_SAMPLE_BLOCK       = 48;
static const int APU_NUMERATOR_NTSC     = 15664;
static const int APU_DENOMINATOR_NTSC   = 328125;
static const int APU_NUMERATOR_PAL      = 34176;
static const int APU_DENOMINATOR_PAL    = 709379;

namespace SNES {
    struct Processor
    {
        unsigned frequency;
        int32 clock;
    };

    #include "apu_smp.hpp"

    class CPU
    {
    public:
        uint8 registers[4];

        inline void reset ()
        {
            registers[0] = registers[1] = registers[2] = registers[3] = 0;
        }

        inline void port_write (uint8 port, uint8 data)
        {
            registers[port & 3] = data;
        }

        inline uint8 port_read (uint8 port)
        {
            return registers[port & 3];
        }
    };
    CPU cpu;
} // namespace SNES


bool8 S9xInitSound(int buffer_ms)
{
    return false;
}

bool8 S9xInitAPU(void)
{
    return (TRUE);
}

void S9xDeinitAPU(void)
{
}

uint8 S9xAPUReadPort(int port)
{
    S9xAPUExecute();
    return ((uint8)SNES::smp.port_read(port & 3));
}

void S9xAPUWritePort(int port, uint8 byte)
{
    S9xAPUExecute();
    SNES::cpu.port_write(port & 3, byte);
}

void S9xAPUSetReferenceTime(int32 cpucycles)
{
}

void S9xAPUExecute(void)
{
    static uint32 remainder = 0;
    int32 cpucycles = CPU.Cycles;
    SNES::smp.clock -= (APU_NUMERATOR_NTSC * (cpucycles - 0) + remainder) / APU_DENOMINATOR_NTSC;
    SNES::smp.enter();

    S9xAPUSetReferenceTime(CPU.Cycles);
}

void S9xAPUEndScanline(void)
{
    S9xAPUExecute();
}

void S9xAPUTimingSetSpeedup(int ticks)
{
}

void S9xResetAPU(void)
{
    SNES::cpu.reset();
    SNES::smp.power();

    S9xClearSamples();
}

void S9xSoftResetAPU(void)
{
    SNES::cpu.reset();
    SNES::smp.reset();

    S9xClearSamples();
}

void S9xAPUSaveState(uint8 *block)
{
}

void S9xAPULoadState(uint8 *block)
{
}

void S9xAPULoadBlarggState(uint8 *oldblock)
{
}

bool8 S9xSPCDump(const char *filename)
{
    return (TRUE);
}

bool8 S9xMixSamples(uint8 *dest, int sample_count)
{
    return true;
}

int S9xGetSampleCount(void)
{
    return 0;
}

void S9xLandSamples(void)
{
}

void S9xClearSamples(void)
{
}

bool8 S9xSyncSound(void)
{
    return true;
}

void S9xSetSamplesAvailableCallback(apu_callback callback, void *data)
{
}

void S9xUpdateDynamicRate(int avail, int buffer_size)
{
}

void S9xSetSoundControl(uint8 voice_switch)
{
}

void S9xSetSoundMute(bool8 mute)
{
}

void S9xDumpSPCSnapshot(void)
{
}
