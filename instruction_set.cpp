#include "instruction_set.h"

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

void InstructionSet::nop() {} // Do nothing

void InstructionSet::stop() {
    std::cout << "STOPPING" << std::endl;
}

void InstructionSet::daa() {
    // https://rgbds.gbdev.io/docs/v0.9.1/gbz80.7#DAA

    u8 adjust = 0;
    if (BIT(regs.F, 6)) {
        if (BIT(regs.F, 5)) { adjust += 6; }
        if (BIT(regs.F, 4)) { adjust += 0x60; }
        regs.A -= adjust;
    } else {
        if (BIT(regs.F, 5) || ((regs.A & 0xF) > 0x9)) { adjust += 6; }
        if (BIT(regs.F, 4) || (regs.A > 0x99)) { 
            adjust += 0x60; 
            BIT_SET(regs.F, 4);
        }
    }

    if (regs.A == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); }
    BIT_RESET(regs.F, 5);
}

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

void InstructionSet::ld_SP_signed() {
    emulate_cycles(1);

    char e8 = (char)get_n8();

    BIT_RESET(regs.F, 7);
    BIT_RESET(regs.F, 6);
    if ((regs.SP & 0xF) + (e8 & 0xF) > 0xF) { BIT_SET(regs.F, 5); }
    if ((regs.SP & 0xFF) + (e8 & 0xFF) > 0xFF) { BIT_SET(regs.F, 4); }

    regs.H = ((regs.SP + e8) >> 8) & 0xFF;
    regs.L = (regs.SP + e8) & 0xFF;
}

void InstructionSet::ld_SP_HL() {
    emulate_cycles(1);
    regs.SP = ((u16)regs.H << 8) | (u16)regs.L;
}

// note: Stack grows "upside down" memory, and Game Boy memory stores
// lowest byte first (little endian)

void InstructionSet::push(u8 hi_reg, u8 lo_reg) {

    // Higher byte pushed first
    bus.write(--regs.SP, hi_reg);
    emulate_cycles(1);
    bus.write(--regs.SP, lo_reg);
    emulate_cycles(1);

    emulate_cycles(1);
}

void InstructionSet::pop(u8 &hi_reg, u8 &lo_reg, addr_mode mode) {

    // Lower byte popped first
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

// note: Before calling a subroutine, you have to store the
// current address onto stack to come back to where you left off

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
    bus.write(--regs.SP, (regs.PC >> 8) & 0xFF);
    emulate_cycles(1);
    bus.write(--regs.SP, regs.PC & 0xFF);
    emulate_cycles(1);

    regs.PC = addr;
    emulate_cycles(1);
}

void InstructionSet::inc(u8 &reg) {
    reg += 1;
    u8 val = reg;

    // flag calculations
    if (val == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); }     
    BIT_RESET(regs.F, 6);
    if ((val & 0x0F) == 0) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); }
}

void InstructionSet::inc(u8 &hi_reg, u8 &lo_reg) {
    emulate_cycles(1);

    if (lo_reg == 0xFF) {
        hi_reg++;
    }
    lo_reg++;
}

void InstructionSet::inc_SP() {
    emulate_cycles(1);
    regs.SP++;
}

void InstructionSet::inc_HL() {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;

    u8 val = bus.read(addr) + 1;
    emulate_cycles(1);

    bus.write(addr, val);
    emulate_cycles(1);

    // flag calculations
    if (val == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); } 
    BIT_RESET(regs.F, 6);
    if ((val & 0x0F) == 0) { BIT_SET(regs.F, 5); }  
    else { BIT_RESET(regs.F, 5); }  
}

void InstructionSet::dec(u8 &reg) {
    reg -= 1;
    u8 val = reg;

    // flag calculations
    if (val == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); }      
    BIT_SET(regs.F, 6);
    if ((val & 0x0F) == 0x0F) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); } 
}

void InstructionSet::dec(u8 &hi_reg, u8 &lo_reg) {
    emulate_cycles(1);

    if (lo_reg == 0x00) {
        hi_reg--;
    }
    lo_reg--;
}

void InstructionSet::dec_SP() {
    emulate_cycles(1);
    regs.SP--;
}

void InstructionSet::dec_HL() {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;

    u8 val = bus.read(addr) - 1;
    emulate_cycles(1);

    bus.write(addr, val);
    emulate_cycles(1);

    // flag calculations
    if (val == 0) { BIT_SET(regs.F, 7); }  
    else { BIT_RESET(regs.F, 7); } 
    BIT_SET(regs.F, 6);
    if ((val & 0x0F) == 0x0F) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); } 
}

void InstructionSet::add(u8 reg) {
    u16 val = (u16)regs.A + (u16)reg;
    
    // flag calculations
    if ((val & 0xFF) == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); } 
    BIT_RESET(regs.F, 6);
    if ((regs.A & 0xF) + (reg & 0xF) > 0xF) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); } 
    if (val > 0xFF) { BIT_SET(regs.F, 4); }
    else { BIT_RESET(regs.F, 4); } 

    regs.A += reg;
} 

void InstructionSet::add() {
    add(get_n8());
}    

void InstructionSet::add_HL() {
    u8 byte = bus.read(((u16)regs.H << 8) | (u16)regs.L);
    emulate_cycles(1);
    add(byte);
}

void InstructionSet::add16(u8 hi_reg, u8 lo_reg) {
    emulate_cycles(1);

    // Let the compiler do the carrying
    u16 reg = ((u16)hi_reg << 8) | (u16)lo_reg;
    u16 HL = ((u16)regs.H << 8) | (u16)regs.L;
    u32 val = HL + reg;

    // flag calculations
    BIT_RESET(regs.F, 6);
    if ((reg & 0xFFF) + (HL & 0xFFF) > 0xFFF) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); } 
    if (val > 0xFFFF) { BIT_SET(regs.F, 4); }
    else { BIT_RESET(regs.F, 4); } 

    regs.H = (u8)((val >> 8) & 0xFF);
    regs.L = (u8)(val & 0xFF);
}      

void InstructionSet::add16() {
    emulate_cycles(1);

    // Let the compiler do the carrying
    u16 HL = ((u16)regs.H << 8) | (u16)regs.L;
    u32 val = HL + regs.SP;

    BIT_RESET(regs.F, 6);
    if ((HL & 0xFFF) + (regs.SP & 0xFFF) > 0xFFF) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); } 
    if (val > 0xFFFF) { BIT_SET(regs.F, 4); }
    else { BIT_RESET(regs.F, 4); } 

    regs.H = (u8)((val >> 8) & 0xFF);
    regs.L = (u8)(val & 0xFF);
}                         

void InstructionSet::add_to_SP() {
    char e8 = (char)get_n8();

    BIT_RESET(regs.F, 7);     
    BIT_RESET(regs.F, 6);
    if ((regs.SP & 0xF) + (e8 & 0xF) > 0xF) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); } 
    if ((regs.SP & 0xFF) + (e8 & 0xFF) > 0xFF) { BIT_SET(regs.F, 4); }
    else { BIT_RESET(regs.F, 4); } 

    regs.SP += e8;
}                     

void InstructionSet::sub(u8 reg) {
    
    // flag calculations
    if (regs.A - reg == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); }    
    BIT_SET(regs.F, 6);
    if ((regs.A & 0xF) < (reg & 0xF)) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); } 
    if (regs.A < reg) { BIT_SET(regs.F, 4); }
    else { BIT_RESET(regs.F, 4); } 

    regs.A -= reg;
}    

void InstructionSet::sub() {
    sub(get_n8());
}         

void InstructionSet::sub_HL() {
    u8 byte = bus.read(((u16)regs.H << 8) | (u16)regs.L);
    emulate_cycles(1);
    sub(byte);
} 

void InstructionSet::adc(u8 reg) {
    u16 c = BIT(regs.F, 4);
    add(reg + c);
}

void InstructionSet::adc() {
    adc(get_n8());
}

void InstructionSet::adc_HL() {
    u8 byte = bus.read(((u16)regs.H << 8) | (u16)regs.L);
    emulate_cycles(1);
    adc(byte);
}

void InstructionSet::sbc(u8 reg) {
    u16 c = BIT(regs.F, 4);
    sub(reg + c);
}

void InstructionSet::sbc() {
    sbc(get_n8());
}

void InstructionSet::sbc_HL() {
    u8 byte = bus.read(((u16)regs.H << 8) | (u16)regs.L);
    emulate_cycles(1);
    sbc(byte);
}

void InstructionSet::and_A(u8 reg) {
    regs.A &= reg;

    if (regs.A == 0) { BIT_SET(regs.F,7); }
    else { BIT_RESET(regs.F, 7); } 
    BIT_RESET(regs.F, 6);
    BIT_SET(regs.F, 5);
    BIT_RESET(regs.F, 4);
}

void InstructionSet::and_A() {
    and_A(get_n8());
}

void InstructionSet::and_A_HL() {
    and_A(bus.read(((u16)regs.H << 8) | (u16)regs.L));
}

void InstructionSet::or_A(u8 reg) {
    regs.A |= reg;

    if (regs.A == 0) { BIT_SET(regs.F,7); }
    else { BIT_RESET(regs.F, 7); } 
    BIT_RESET(regs.F, 6);
    BIT_RESET(regs.F, 5);
    BIT_RESET(regs.F, 4);
}

void InstructionSet::or_A() {
    or_A(get_n8());
}

void InstructionSet::or_A_HL() {
    or_A(bus.read(((u16)regs.H << 8) | (u16)regs.L));
}

void InstructionSet::xor_A(u8 reg) {
    regs.A ^= reg;

    if (regs.A == 0) { BIT_SET(regs.F,7); }
    else { BIT_RESET(regs.F, 7); } 
    BIT_RESET(regs.F, 6);
    BIT_RESET(regs.F, 5);
    BIT_RESET(regs.F, 4);
}

void InstructionSet::xor_A() {
    xor_A(get_n8());
}

void InstructionSet::xor_A_HL() {
    xor_A(bus.read(((u16)regs.H << 8) | (u16)regs.L));
}

void InstructionSet::cp(u8 reg) {
    if (regs.A - reg == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); }    
    BIT_SET(regs.F, 6);
    if ((regs.A & 0xF) < (reg & 0xF)) { BIT_SET(regs.F, 5); }
    else { BIT_RESET(regs.F, 5); } 
    if (regs.A < reg) { BIT_SET(regs.F, 4); }
    else { BIT_RESET(regs.F, 4); } 
}

void InstructionSet::cp() {
    cp(get_n8());
}

void InstructionSet::cp_HL() {
    cp(bus.read(((u16)regs.H << 8) | (u16)regs.L));
} 

void InstructionSet::cpl() {
    regs.A = ~regs.A;
    BIT_SET(regs.F, 6);
    BIT_SET(regs.F, 5);
}

void InstructionSet::ccf() {
    BIT_RESET(regs.F, 6);
    BIT_RESET(regs.F, 5);
    if (BIT(regs.F, 4)) { BIT_RESET(regs.F, 4); }
    else { BIT_SET(regs.F, 4); }
}

void InstructionSet::scf() {
    BIT_RESET(regs.F, 6);
    BIT_RESET(regs.F, 5);
    BIT_SET(regs.F, 4);
}

void InstructionSet::rlca() {
    shift(RLC, regs.A);
    BIT_RESET(regs.F, 7);
}

void InstructionSet::rrca() {
    shift(RRC, regs.A);
    BIT_RESET(regs.F, 7);
}

void InstructionSet::rla() {
    shift(RL, regs.A);
    BIT_RESET(regs.F, 7);
}

void InstructionSet::rra() {
    shift(RR, regs.A);
    BIT_RESET(regs.F, 7);
}

void InstructionSet::di() {
    ctx.IME = false;
}

void InstructionSet::ei() {
    ctx.IME_next = true;
}

void InstructionSet::halt() {
    ctx.halted = true;
}

void InstructionSet::shift(addr_mode mode, u8 &reg) {
    u8 bit7 = (reg >> 7) & 0x1;
    u8 bit0 = reg & 0x1;
    u8 c = BIT(regs.F, 4);

    switch (mode) {
        case RLC:
            reg = (reg << 1) | bit7;
            if (bit7) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case RRC:
            reg = (reg >> 1) | (bit0 << 7);
            if (bit0) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case RL:
            reg = (reg << 1) | c;
            if (c) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case RR:
            reg = (reg >> 1) | (c << 7);
            if (c) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case SLA:
            reg <<= 1;
            if (bit7) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case SRA:
            reg = (reg >> 1) | (bit7 << 7);
            if (bit0) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case SWAP:
            reg = (reg & 0x0F) << 4 | (reg & 0xF0) >> 4;
            BIT_RESET(regs.F, 4);
            break;
        case SRL:
            reg = (reg >> 1);
            if (bit0) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
    }

    if (reg == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); } 
    BIT_RESET(regs.F, 6);
    BIT_RESET(regs.F, 5);
}

void InstructionSet::shift_HL(addr_mode mode) {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;
    u8 byte = bus.read(addr);
    emulate_cycles(1);

    u8 bit7 = (byte >> 7) & 0x1;
    u8 bit0 = byte & 0x1;
    u8 c = BIT(regs.F, 4);

    switch (mode) {
        case RLC:
            byte = (byte << 1) | bit7;
            bus.write(addr, byte);
            emulate_cycles(1);

            if (bit7) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case RRC:
            byte = (byte >> 1) | (bit0 << 7);
            bus.write(addr, byte);
            emulate_cycles(1);

            if (bit0) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case RL:
            byte = (byte << 1) | c;
            bus.write(addr, byte);
            emulate_cycles(1);

            if (c) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case RR:
            byte = (byte >> 1) | (c << 7);
            bus.write(addr, byte);
            emulate_cycles(1);

            if (c) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case SLA:
            byte <<= 1;
            bus.write(addr, byte);
            emulate_cycles(1);

            if (bit7) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case SRA:
            byte = (byte >> 1) | (bit7 << 7);
            bus.write(addr, byte);
            emulate_cycles(1);

            if (bit0) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
        case SWAP:
            byte = (byte & 0x0F) << 4 | (byte & 0xF0) >> 4;
            bus.write(addr, byte);
            emulate_cycles(1);

            BIT_RESET(regs.F, 4);
            break;
        case SRL:
            byte = (byte >> 1);
            bus.write(addr, byte);
            emulate_cycles(1);

            if (bit0) { BIT_SET(regs.F, 4); }
            else { BIT_RESET(regs.F, 4); }
            break;
    }

    if (byte == 0) { BIT_SET(regs.F, 7); }
    else { BIT_RESET(regs.F, 7); } 
    BIT_RESET(regs.F, 6);
    BIT_RESET(regs.F, 5);
}

void InstructionSet::bit_flag(addr_mode mode, u8 bit, u8 &reg) {
    switch (mode) {
        case BIT:
            if (!BIT(reg, bit)) { BIT_SET(regs.F, 7); }
            else { BIT_RESET(regs.F, 7); } 
            BIT_RESET(regs.F, 6);
            BIT_SET(regs.F, 5);
            break;
        case RES:
            BIT_RESET(reg, bit);
            break;
        case SET:
            BIT_SET(reg, bit);
            break;
    }
}

void InstructionSet::bit_flag_HL(addr_mode mode, u8 bit) {
    u16 addr = ((u16)regs.H << 8) | (u16)regs.L;
    u8 byte = bus.read(addr);
    emulate_cycles(1);

    switch (mode) {
        case BIT:
            if (!BIT(byte, bit)) { BIT_SET(regs.F, 7); }
            else { BIT_RESET(regs.F, 7); } 
            BIT_RESET(regs.F, 6);
            BIT_SET(regs.F, 5);
            break;
        case RES:
            BIT_RESET(byte, bit);
            bus.write(addr, byte);
            emulate_cycles(1);
            break;
        case SET:
            BIT_SET(byte, bit);
            bus.write(addr, byte);
            emulate_cycles(1);
            break;
    }
}