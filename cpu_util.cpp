#include "cpu_util.h"

Registers::Registers() {}
Registers::~Registers() {}

CpuContext::CpuContext() {}
CpuContext::~CpuContext() {}

InstructionSet::InstructionSet(
    Registers &regs_, CpuContext &ctx_, MemoryBus &bus_
) : regs(regs_), ctx(ctx_), bus(bus_) {}
InstructionSet::~InstructionSet() {}

void InstructionSet::emulate_cycles(int cpu_cycles) {}

u8 InstructionSet::get_n8() {

    // Data is immediately after opcode
    u8 n8 = bus.read(regs.PC++); 
    emulate_cycles(1);

    return n8;
}

u16 InstructionSet::get_n16() {
    
    // Data is immediately after opcode
    u16 lo = bus.read(regs.PC++);
    emulate_cycles(1);
    u16 hi = bus.read(regs.PC++);
    emulate_cycles(1);

    u16 n16 = (hi << 8) | lo;
    return n16;
}

void InstructionSet::nop() {}; // Do nothing

void InstructionSet::ld(u8 &reg1, u8 reg2) {
    reg1 = reg2;
}

void InstructionSet::ld(u8 &reg) {
    reg = get_n8();
}

void InstructionSet::ld16(u8 &hi_reg, u8 &lo_reg) {
    u16 n16 = get_n16();

    lo_reg = n16 & 0xFF;
    hi_reg = (n16 >> 8) & 0xFF;
}

void InstructionSet::ld16(u16 &SP) {
    SP = get_n16();
}

void InstructionSet::ld_to_HL(u8 reg) {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;

    bus.write(addr, reg);
    emulate_cycles(1);
}

void InstructionSet::ld_to_HL() {
    u8 n8 = get_n8();

    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;

    bus.write(addr, n8);
    emulate_cycles(1);
}

void InstructionSet::ld_to_A(u8 hi_reg, u8 lo_reg, addr_mode mode) {
    u16 addr = ((u16)hi_reg << 8) | (u16)lo_reg;

    regs.A = bus.read(addr);
    emulate_cycles(1);

    // Register HL is either incremented or decremented
    if (mode == LDI) {
        if (regs.L == 0xFF) {
            regs.H++;
        }
        regs.L++;
    } else if (mode == LDD) {
        if (regs.L == 0x00) {
            regs.H--;
        }
        regs.L--;
    }
}

void InstructionSet::ld_to_A() {
    u16 n16 = get_n16();

    regs.A = bus.read(n16);
    emulate_cycles(1);
}

void InstructionSet::ld_from_HL(u8 &reg) {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;

    reg = bus.read(addr);
    emulate_cycles(1);
}

void InstructionSet::ld_from_A(u8 hi_reg, u8 lo_reg, addr_mode mode) {
    u16 addr = ((u16)hi_reg << 8) | (u16)lo_reg;

    bus.write(addr, regs.A);
    emulate_cycles(1);

    // Register HL is either incremented or decremented
    if (mode == LDI) {
        if (regs.L == 0xFF) {
            regs.H++;
        }
        regs.L++;
    } else if (mode == LDD) {
        if (regs.L == 0x00) {
            regs.H--;
        }
        regs.L--;
    }
}

void InstructionSet::ld_from_A() {
    u16 n16 = get_n16();

    bus.write(n16, regs.A);
    emulate_cycles(1);
}

void InstructionSet::ld_from_SP() {
    u16 n16 = get_n16();

    bus.write(n16, regs.SP & 0xFF);
    emulate_cycles(1);
    bus.write(n16 + 1, (regs.SP >> 8) & 0xFF);
    emulate_cycles(1);
}

void InstructionSet::ldh_to_A(addr_mode mode) {
    if (mode == LDH_A8) {
        regs.A = bus.read(0xFF00 + (u16)get_n8());
    } else if (mode == LDH_C) {
        regs.A = bus.read(0xFF00 + regs.C);
    }
    emulate_cycles(1);
}

void InstructionSet::ldh_from_A(addr_mode mode) {
    if (mode == LDH_A8) {
        bus.write(0xFF00 + (u16)get_n8(), regs.A);
    } else if (mode == LDH_C) {
        bus.write(0xFF00 + regs.C, regs.A);
    }
    emulate_cycles(1);
}

void InstructionSet::push(u8 hi_reg, u8 lo_reg) {
    // Decrement SP, then write to memory
    bus.write(--regs.SP, hi_reg);
    emulate_cycles(1);
    bus.write(--regs.SP, lo_reg);
    emulate_cycles(1);

    emulate_cycles(1);
}

void InstructionSet::pop(u8 &hi_reg, u8 &lo_reg, addr_mode mode) {
    // Load to register nibble, then increment SP
    if (mode == DEFAULT) {
        lo_reg = bus.read(regs.SP++);
    } else if (mode == POP_AF) {
        // Only want the top nibble when loading into F register
        lo_reg = bus.read(regs.SP++) & 0xF0;
    }  
    emulate_cycles(1);
    hi_reg = bus.read(regs.SP++);
    emulate_cycles(1);
}

void InstructionSet::jp(bool cond_code) {
    u16 n16 = get_n16();
    if (cond_code) {
        regs.PC = n16;
        emulate_cycles(1);
    }
}

void InstructionSet::jp_HL() {
    regs.PC = ((u16)regs.H << 8) | (u16)regs.L;
}

void InstructionSet::jr(bool cond_code) {
    char e8 = (char)get_n8();
    u16 addr = regs.PC + e8;
    if (cond_code) {
        regs.PC = addr;
        emulate_cycles(1);
    }
}

void InstructionSet::call(bool cond_code) {
    u16 n16 = get_n16();

    if (cond_code) {
        // After getting n16, PC is now pointing to the next instruction
        bus.write(--regs.SP, (regs.PC >> 8) & 0xFF);
        emulate_cycles(1);
        bus.write(--regs.SP, regs.PC & 0xFF);
        emulate_cycles(1);

        regs.PC = n16;
        emulate_cycles(1);
    }
}

void InstructionSet::ret(bool cond_code, addr_mode mode) {
    if (mode == RET_CC) {
        emulate_cycles(1);
    }

    if (cond_code) {
        u16 lo = bus.read(regs.SP++);
        emulate_cycles(1);
        u16 hi = bus.read(regs.SP++);
        emulate_cycles(1);

        u16 addr = (hi << 8) | lo;
        regs.PC = addr;
        emulate_cycles(1); 
    }
}

void InstructionSet::reti() {
    ctx.IME = true;
    ret();
}

void InstructionSet::rst(u16 addr) {
    // Doing a CALL to addr
    bus.write(--regs.SP, (regs.PC >> 8) & 0xFF);
    emulate_cycles(1);
    bus.write(--regs.SP, regs.PC & 0xFF);
    emulate_cycles(1);

    regs.PC = addr;
    emulate_cycles(1);
}