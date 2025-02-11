#include "cpu_util.h"

Registers::Registers() {}
Registers::~Registers() {}

InstructionSet::InstructionSet(Registers &regs_, MemoryBus &bus_) : regs(regs_), bus(bus_) {}
InstructionSet::~InstructionSet() {}

void InstructionSet::emulate_cycles(int cpu_cycles) {}

u8 InstructionSet::get_n8() {
    u8 n8 = bus.read(regs.PC++); 
    emulate_cycles(1);

    return n8;
}

u16 InstructionSet::get_n16() {
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
    hi_reg = (n16 & 0xFF00) >> 8;
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

void InstructionSet::ld_from_HL(u8 &reg) {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;

    reg = bus.read(addr);
    emulate_cycles(1);
}

void InstructionSet::ld_to_mem(u8 hi_reg, u8 lo_reg) {
    u16 addr = ((u16)hi_reg << 8) | (u16)lo_reg;

    bus.write(addr, regs.A);
    emulate_cycles(1);
}

void InstructionSet::ld_to_mem() {
    u16 n16 = get_n16();

    bus.write(n16, regs.A);
    emulate_cycles(1);
}

void InstructionSet::ld_from_mem(u8 hi_reg, u8 lo_reg) {
    u16 addr = ((u16)hi_reg << 8) | (u16)lo_reg;

    regs.A = bus.read(addr);
    emulate_cycles(1);
}

void InstructionSet::ld_from_mem() {
    u16 n16 = get_n16();

    regs.A = bus.read(n16);
    emulate_cycles(1);
}

void InstructionSet::ld_HLI(addr_mode mode) {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;

    if (mode == TO_HL) {
        bus.write(addr, regs.A);
    } else if (mode == FROM_HL) {
        regs.A = bus.read(addr);
    }
    emulate_cycles(1);

    if (regs.L == 0xFF) {
        regs.H++;
    }
    regs.L++;
}

void InstructionSet::ld_HLD(addr_mode mode) {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;

    if (mode == TO_HL) {
        bus.write(addr, regs.A);
    } else if (mode == FROM_HL) { 
        regs.A = bus.read(addr);
    }
    emulate_cycles(1);

    if (regs.L == 0) {
        regs.H--;
    }
    regs.L--;
}

void InstructionSet::ld_from_SP() {
    u16 n16 = get_n16();

    bus.write(n16, regs.SP & 0xFF);
    emulate_cycles(1);
    bus.write(n16 + 1, regs.SP >> 8);
    emulate_cycles(1);
}

void InstructionSet::ldh(addr_mode mode) {
    u8 a8 = get_n8() + 0xFF00;
    if (mode == FROM_A) {
        bus.write(a8, regs.A);
    } else if (mode == TO_A) {
        regs.A = bus.read(a8);
    }
    emulate_cycles(1);
}

void InstructionSet::ldh_C(addr_mode mode) {
    if (mode == FROM_A) {
        bus.write(regs.C + 0xFF00, regs.A);
    } else if (mode == TO_A) {
        regs.A = bus.read(regs.C + 0xFF00);
    }
    emulate_cycles(1);
}