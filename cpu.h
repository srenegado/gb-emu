#ifndef CPU_H
#define CPU_H

#include "common.h"
#include "memory.h"
#include "cpu_util.h"

class CPU {
    private:
        MemoryBus &bus;
        Registers regs;
        CpuContext ctx;
        InstructionSet instr_set;
    public:
        CPU(MemoryBus &bus_);
        ~CPU();
        bool step();
        bool decode_and_execute(u8 opcode);
};

#endif