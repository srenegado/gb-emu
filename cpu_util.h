#ifndef CPU_UTIL_H
#define CPU_UTIL_H

#include "common.h"

struct Registers {
    // https://gbdev.io/pandocs/Power_Up_Sequence.html#cpu-registers
    u8 A = 0x01;
    u8 F = 0xB0;
    u8 B = 0x00;
    u8 C = 0x13;
    u8 D = 0x00;
    u8 E = 0xD8;
    u8 H = 0x01;
    u8 L = 0x4D;
    u16 PC = 0x100;
    u16 SP = 0xFFFE;
    Registers();
    ~Registers();
}; 

struct CpuContext {
    bool halted = false;
    bool IME = false;
    bool IME_next = false;
    CpuContext();
    ~CpuContext();
};

#endif