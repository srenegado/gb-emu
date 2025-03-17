#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

#include "common.h"
#include "memory.h"
#include "cpu_util.h"

typedef enum {
    Int_VBlank,
    Int_LCD_STAT,
    Int_Timer,
    Int_Serial,
    Int_Joypad
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