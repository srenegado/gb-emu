#ifndef CPU_H
#define CPU_H

#include "common.h"
#include "memory.h"
#include "cpu_util.h"
#include "instruction_set.h" 
#include "interrupt_handler.h"

class CPU {
    private:
        MemoryBus &bus;
        Registers regs;
        CpuContext ctx;
        InstructionSet instr_set;
        InterruptHandler int_handler;

        char debug_msg[1024] = {0};
        int debug_msg_size = 0;
    public:
        CPU(MemoryBus &bus_);
        ~CPU();
        bool step();
        bool decode_and_execute(u8 opcode);
};

#endif