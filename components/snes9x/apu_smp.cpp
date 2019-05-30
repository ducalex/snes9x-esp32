#include "apu_snes.hpp"

#define SMP_CPP
namespace SNES {

#ifdef DEBUGGER
#include "debugger/disassembler.cpp"
#endif

SMP smp;



uint8 SMP::op_adc(uint8 x, uint8 y) {
  int r = x + y + regs.p.c;
  regs.p.n = r & 0x80;
  regs.p.v = ~(x ^ y) & (x ^ r) & 0x80;
  regs.p.h = (x ^ y ^ r) & 0x10;
  regs.p.z = (uint8)r == 0;
  regs.p.c = r > 0xff;
  return r;
}

uint16 SMP::op_addw(uint16 x, uint16 y) {
  uint16 r;
  regs.p.c = 0;
  r  = op_adc(x, y);
  r |= op_adc(x >> 8, y >> 8) << 8;
  regs.p.z = r == 0;
  return r;
}

uint8 SMP::op_and(uint8 x, uint8 y) {
  x &= y;
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}

uint8 SMP::op_cmp(uint8 x, uint8 y) {
  int r = x - y;
  regs.p.n = r & 0x80;
  regs.p.z = (uint8)r == 0;
  regs.p.c = r >= 0;
  return x;
}

uint16 SMP::op_cmpw(uint16 x, uint16 y) {
  int r = x - y;
  regs.p.n = r & 0x8000;
  regs.p.z = (uint16)r == 0;
  regs.p.c = r >= 0;
  return x;
}

uint8 SMP::op_eor(uint8 x, uint8 y) {
  x ^= y;
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}

uint8 SMP::op_or(uint8 x, uint8 y) {
  x |= y;
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}

uint8 SMP::op_sbc(uint8 x, uint8 y) {
  int r = x - y - !regs.p.c;
  regs.p.n = r & 0x80;
  regs.p.v = (x ^ y) & (x ^ r) & 0x80;
  regs.p.h = !((x ^ y ^ r) & 0x10);
  regs.p.z = (uint8)r == 0;
  regs.p.c = r >= 0;
  return r;
}

uint16 SMP::op_subw(uint16 x, uint16 y) {
  uint16 r;
  regs.p.c = 1;
  r  = op_sbc(x, y);
  r |= op_sbc(x >> 8, y >> 8) << 8;
  regs.p.z = r == 0;
  return r;
}

uint8 SMP::op_inc(uint8 x) {
  x++;
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}

uint8 SMP::op_dec(uint8 x) {
  x--;
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}

uint8 SMP::op_asl(uint8 x) {
  regs.p.c = x & 0x80;
  x <<= 1;
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}

uint8 SMP::op_lsr(uint8 x) {
  regs.p.c = x & 0x01;
  x >>= 1;
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}

uint8 SMP::op_rol(uint8 x) {
  unsigned carry = (unsigned)regs.p.c;
  regs.p.c = x & 0x80;
  x = (x << 1) | carry;
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}

uint8 SMP::op_ror(uint8 x) {
  unsigned carry = (unsigned)regs.p.c << 7;
  regs.p.c = x & 0x01;
  x = carry | (x >> 1);
  regs.p.n = x & 0x80;
  regs.p.z = x == 0;
  return x;
}





void SMP::tick() {
  timer0.tick();
  timer1.tick();
  timer2.tick();

  clock++;
}

void SMP::tick(unsigned clocks) {
  timer0.tick(clocks);
  timer1.tick(clocks);
  timer2.tick(clocks);

  clock += clocks;
}

void SMP::op_io() {
  tick();
}

void SMP::op_io(unsigned clocks) {
  tick(clocks);
}

uint8 SMP::op_read(uint16 addr) {
  tick();
  if((addr & 0xfff0) == 0x00f0) return mmio_read(addr);
  if(addr >= 0xffc0 && status.iplrom_enable) return iplrom[addr & 0x3f];
  return apuram[addr];
}

void SMP::op_write(uint16 addr, uint8 data) {
  tick();
  if((addr & 0xfff0) == 0x00f0) mmio_write(addr, data);
  apuram[addr] = data;  //all writes go to RAM, even MMIO writes
}

uint8 SMP::op_readstack()
{
  tick();
  return apuram[0x0100 | ++regs.sp];
}

void SMP::op_writestack(uint8 data)
{
  tick();
  apuram[0x0100 | regs.sp--] = data;
}

void SMP::op_step() {
  #define op_readpc() op_read(regs.pc++)
  #define op_readdp(addr) op_read((regs.p.p << 8) + ((addr) & 0xff))
  #define op_writedp(addr, data) op_write((regs.p.p << 8) + ((addr) & 0xff), data)
  #define op_readaddr(addr) op_read(addr)
  #define op_writeaddr(addr, data) op_write(addr, data)

  if(opcode_cycle == 0)
  {
    opcode_number = op_readpc();
  }

  switch(opcode_number) {
    #include "core/oppseudo_misc.cpp"
    #include "core/oppseudo_mov.cpp"
    #include "core/oppseudo_pc.cpp"
    #include "core/oppseudo_read.cpp"
    #include "core/oppseudo_rmw.cpp"
  }
}







const uint8 SMP::iplrom[64] = {
/*ffc0*/  0xcd, 0xef,        //mov   x,#$ef
/*ffc2*/  0xbd,              //mov   sp,x
/*ffc3*/  0xe8, 0x00,        //mov   a,#$00
/*ffc5*/  0xc6,              //mov   (x),a
/*ffc6*/  0x1d,              //dec   x
/*ffc7*/  0xd0, 0xfc,        //bne   $ffc5
/*ffc9*/  0x8f, 0xaa, 0xf4,  //mov   $f4,#$aa
/*ffcc*/  0x8f, 0xbb, 0xf5,  //mov   $f5,#$bb
/*ffcf*/  0x78, 0xcc, 0xf4,  //cmp   $f4,#$cc
/*ffd2*/  0xd0, 0xfb,        //bne   $ffcf
/*ffd4*/  0x2f, 0x19,        //bra   $ffef
/*ffd6*/  0xeb, 0xf4,        //mov   y,$f4
/*ffd8*/  0xd0, 0xfc,        //bne   $ffd6
/*ffda*/  0x7e, 0xf4,        //cmp   y,$f4
/*ffdc*/  0xd0, 0x0b,        //bne   $ffe9
/*ffde*/  0xe4, 0xf5,        //mov   a,$f5
/*ffe0*/  0xcb, 0xf4,        //mov   $f4,y
/*ffe2*/  0xd7, 0x00,        //mov   ($00)+y,a
/*ffe4*/  0xfc,              //inc   y
/*ffe5*/  0xd0, 0xf3,        //bne   $ffda
/*ffe7*/  0xab, 0x01,        //inc   $01
/*ffe9*/  0x10, 0xef,        //bpl   $ffda
/*ffeb*/  0x7e, 0xf4,        //cmp   y,$f4
/*ffed*/  0x10, 0xeb,        //bpl   $ffda
/*ffef*/  0xba, 0xf6,        //movw  ya,$f6
/*fff1*/  0xda, 0x00,        //movw  $00,ya
/*fff3*/  0xba, 0xf4,        //movw  ya,$f4
/*fff5*/  0xc4, 0xf4,        //mov   $f4,a
/*fff7*/  0xdd,              //mov   a,y
/*fff8*/  0x5d,              //mov   x,a
/*fff9*/  0xd0, 0xdb,        //bne   $ffd6
/*fffb*/  0x1f, 0x00, 0x00,  //jmp   ($0000+x)
/*fffe*/  0xc0, 0xff         //reset vector location ($ffc0)
};



unsigned SMP::port_read(unsigned addr) {
  return apuram[0xf4 + (addr & 3)];
}

void SMP::port_write(unsigned addr, unsigned data) {
  apuram[0xf4 + (addr & 3)] = data;
}

unsigned SMP::mmio_read(unsigned addr) {
  switch(addr) {

  case 0xf2:
    return status.dsp_addr;

  case 0xf3:
    return 0;

  case 0xf4:
  case 0xf5:
  case 0xf6:
  case 0xf7:
    return cpu.port_read(addr);

  case 0xf8:
    return status.ram00f8;

  case 0xf9:
    return status.ram00f9;

  case 0xfd: {
    unsigned result = timer0.stage3_ticks & 15;
    timer0.stage3_ticks = 0;
    return result;
  }

  case 0xfe: {
    unsigned result = timer1.stage3_ticks & 15;
    timer1.stage3_ticks = 0;
    return result;
  }

  case 0xff: {
    unsigned result = timer2.stage3_ticks & 15;
    timer2.stage3_ticks = 0;
    return result;
  }

  }

  return 0x00;
}

void SMP::mmio_write(unsigned addr, unsigned data) {
  switch(addr) {

  case 0xf1:
    status.iplrom_enable = data & 0x80;

    if(data & 0x30) {
      if(data & 0x20) {
        cpu.port_write(3, 0x00);
        cpu.port_write(2, 0x00);
      }
      if(data & 0x10) {
        cpu.port_write(1, 0x00);
        cpu.port_write(0, 0x00);
      }
    }

    if(timer2.enable == false && (data & 0x04)) {
      timer2.stage2_ticks = 0;
      timer2.stage3_ticks = 0;
    }
    timer2.enable = data & 0x04;

    if(timer1.enable == false && (data & 0x02)) {
      timer1.stage2_ticks = 0;
      timer1.stage3_ticks = 0;
    }
    timer1.enable = data & 0x02;

    if(timer0.enable == false && (data & 0x01)) {
      timer0.stage2_ticks = 0;
      timer0.stage3_ticks = 0;
    }
    timer0.enable = data & 0x01;

    break;

  case 0xf2:
    status.dsp_addr = data;
    break;

  case 0xf3:
    if(status.dsp_addr & 0x80) break;
    break;

  case 0xf4:
  case 0xf5:
  case 0xf6:
  case 0xf7:
    port_write(addr, data);
    break;

  case 0xf8:
    status.ram00f8 = data;
    break;

  case 0xf9:
    status.ram00f9 = data;
    break;

  case 0xfa:
    timer0.target = data;
    break;

  case 0xfb:
    timer1.target = data;
    break;

  case 0xfc:
    timer2.target = data;
    break;
  }
}





template<unsigned cycle_frequency>
void SMP::Timer<cycle_frequency>::tick() {
  if(++stage1_ticks < cycle_frequency) return;

  stage1_ticks = 0;
  if(enable == false) return;

  if(++stage2_ticks != target) return;

  stage2_ticks = 0;
  stage3_ticks = (stage3_ticks + 1) & 15;
}

template<unsigned cycle_frequency>
void SMP::Timer<cycle_frequency>::tick(unsigned clocks) {
  stage1_ticks += clocks;
  if(stage1_ticks < cycle_frequency) return;

  stage1_ticks -= cycle_frequency;
  if(enable == false) return;

  if(++stage2_ticks != target) return;

  stage2_ticks = 0;
  stage3_ticks = (stage3_ticks + 1) & 15;
}



void SMP::enter() {
  while(clock < 0) op_step();
}

void SMP::power() {
  Processor::clock = 0;

  timer0.target = 0;
  timer1.target = 0;
  timer2.target = 0;

  reset();
}

void SMP::reset() {
  for(unsigned n = 0x0000; n <= 0xffff; n++) apuram[n] = 0x00;

  opcode_number = 0;
  opcode_cycle = 0;

  regs.pc = 0xffc0;
  regs.sp = 0xef;
  regs.B.a = 0x00;
  regs.x = 0x00;
  regs.B.y = 0x00;
  regs.p = 0x02;

  //$00f1
  status.iplrom_enable = true;

  //$00f2
  status.dsp_addr = 0x00;

  //$00f8,$00f9
  status.ram00f8 = 0x00;
  status.ram00f9 = 0x00;

  //timers
  timer0.enable = timer1.enable = timer2.enable = false;
  timer0.stage1_ticks = timer1.stage1_ticks = timer2.stage1_ticks = 0;
  timer0.stage2_ticks = timer1.stage2_ticks = timer2.stage2_ticks = 0;
  timer0.stage3_ticks = timer1.stage3_ticks = timer2.stage3_ticks = 0;
}

SMP::SMP() {
  apuram = new uint8[64 * 1024];
}

SMP::~SMP() {
	delete[] apuram;
}

}
