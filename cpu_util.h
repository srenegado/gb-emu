#ifndef CPU_UTIL_H
#define CPU_UTIL_H

#include "common.h"
#include "memory.h"

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
    u8 IE = 0;
    u8 IF = 0;
    CpuContext();
    ~CpuContext();
};

typedef enum {
    VBlank,
    LCD_STAT,
    Timer,
    Serial,
    Joypad
} interrupt_type;

class InterruptHandler {
    private:
        Registers &regs;    
        CpuContext &ctx;
        MemoryBus &bus;
    public:
        InterruptHandler(Registers &regs_, CpuContext &ctx, MemoryBus &bus_);
        ~InterruptHandler();
        void handle_interrupts();
        void service_interrupt(interrupt_type type);
};

#endif