#ifndef CPU_H
#define CPU_H

#include "common.h"
#include "memory.h"
#include "cpu_util.h"

class CPU {
    private:
        MemoryBus &bus;
        Registers regs;
        Instructions instrs;
        bool halted = false;
    public:
        CPU(MemoryBus &bus_);
        ~CPU();
        bool step();
};


#endif