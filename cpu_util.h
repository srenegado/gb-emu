#ifndef CPU_UTIL_H
#define CPU_UTIL_H

#include "common.h"

struct Registers {
    u8 A = 0;
    u8 F = 0;
    u8 B = 0;
    u8 C = 0;
    u8 D = 0;
    u8 E = 0;
    u8 H = 0;
    u8 L = 0;
    u16 PC = 0x100;
    u16 SP = 0;
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