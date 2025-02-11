#include "cpu.h"

CPU::CPU(MemoryBus &bus_) : bus(bus_), instr_set(regs, bus) {}
CPU::~CPU() {}

bool CPU::step() {
    
    if (!halted) {

        std::cout << "Current PC = 0x" << std::hex << regs.PC << std::endl;

        // Fetch opcode
        u8 opcode = bus.read(regs.PC++); 
        instr_set.emulate_cycles(1);
        std::cout << "Fetching opcode: 0x" << std::hex << +opcode << std::endl;
        
        // Decode and execute opcode
        std::cout << "Decoding and executing opcode...\n";
        if (!decode_and_execute(opcode)) {
            std::cout << "CPU could not decode or execute an instruction\n";
            return false;
        }
    
    }
    
    return true;
}

bool CPU::decode_and_execute(u8 opcode) {

    switch (opcode) {
        
        case 0x00: instr_set.nop(); break;
        case 0x01: instr_set.ld16(regs.B, regs.C); break;
        case 0x06: instr_set.ld(regs.B); break;
        case 0x08: instr_set.ld_from_SP(); break;
        case 0x0A: instr_set.ld_from_mem(regs.B, regs.C); break;
        case 0x0E: instr_set.ld(regs.C); break;

        case 0x11: instr_set.ld16(regs.D, regs.E); break;
        case 0x16: instr_set.ld(regs.D); break;
        case 0x1A: instr_set.ld_from_mem(regs.D, regs.E); break;
        case 0x1E: instr_set.ld(regs.E); break;
        
        case 0x21: instr_set.ld16(regs.H, regs.L); break;
        case 0x22: instr_set.ld_HLI(TO_HL); break;
        case 0x26: instr_set.ld(regs.H); break;
        case 0x2A: instr_set.ld_HLI(FROM_HL); break;
        case 0x2E: instr_set.ld(regs.L); break;
        
        case 0x31: instr_set.ld16(regs.SP); break;
        case 0x32: instr_set.ld_HLD(TO_HL); break;
        case 0x36: instr_set.ld_to_HL(); break;
        case 0x3A: instr_set.ld_HLD(FROM_HL); break;
        case 0x3E: instr_set.ld(regs.B); break;
        
        case 0x40: instr_set.ld(regs.B, regs.B); break;
        case 0x41: instr_set.ld(regs.B, regs.C); break;
        case 0x42: instr_set.ld(regs.B, regs.D); break;
        case 0x43: instr_set.ld(regs.B, regs.E); break;
        case 0x44: instr_set.ld(regs.B, regs.H); break;
        case 0x45: instr_set.ld(regs.B, regs.L); break;
        case 0x46: instr_set.ld_from_HL(regs.B); break;
        case 0x47: instr_set.ld(regs.B, regs.A); break;
        case 0x48: instr_set.ld(regs.C, regs.B); break;
        case 0x49: instr_set.ld(regs.C, regs.C); break;
        case 0x4A: instr_set.ld(regs.C, regs.D); break;
        case 0x4B: instr_set.ld(regs.C, regs.E); break;
        case 0x4C: instr_set.ld(regs.C, regs.H); break;
        case 0x4D: instr_set.ld(regs.C, regs.L); break;
        case 0x4E: instr_set.ld_from_HL(regs.C); break;
        case 0x4F: instr_set.ld(regs.C, regs.A); break;

        case 0x50: instr_set.ld(regs.D, regs.B); break;
        case 0x51: instr_set.ld(regs.D, regs.C); break;
        case 0x52: instr_set.ld(regs.D, regs.D); break;
        case 0x53: instr_set.ld(regs.D, regs.E); break;
        case 0x54: instr_set.ld(regs.D, regs.H); break;
        case 0x55: instr_set.ld(regs.D, regs.L); break;
        case 0x56: instr_set.ld_from_HL(regs.D); break;
        case 0x57: instr_set.ld(regs.D, regs.A); break;
        case 0x58: instr_set.ld(regs.E, regs.B); break;
        case 0x59: instr_set.ld(regs.E, regs.C); break;
        case 0x5A: instr_set.ld(regs.E, regs.D); break;
        case 0x5B: instr_set.ld(regs.E, regs.E); break;
        case 0x5C: instr_set.ld(regs.E, regs.H); break;
        case 0x5D: instr_set.ld(regs.E, regs.L); break;
        case 0x5E: instr_set.ld_from_HL(regs.E); break;
        case 0x5F: instr_set.ld(regs.E, regs.A); break;

        case 0x60: instr_set.ld(regs.H, regs.B); break;
        case 0x61: instr_set.ld(regs.H, regs.C); break;
        case 0x62: instr_set.ld(regs.H, regs.D); break;
        case 0x63: instr_set.ld(regs.H, regs.E); break;
        case 0x64: instr_set.ld(regs.H, regs.H); break;
        case 0x65: instr_set.ld(regs.H, regs.L); break;
        case 0x66: instr_set.ld_from_HL(regs.H); break;
        case 0x67: instr_set.ld(regs.H, regs.A); break;
        case 0x68: instr_set.ld(regs.L, regs.B); break;
        case 0x69: instr_set.ld(regs.L, regs.C); break;
        case 0x6A: instr_set.ld(regs.L, regs.D); break;
        case 0x6B: instr_set.ld(regs.L, regs.E); break;
        case 0x6C: instr_set.ld(regs.L, regs.H); break;
        case 0x6D: instr_set.ld(regs.L, regs.L); break;
        case 0x6E: instr_set.ld_from_HL(regs.L); break;
        case 0x6F: instr_set.ld(regs.L, regs.A); break;
        
        case 0x70: instr_set.ld_to_HL(regs.B); break;
        case 0x71: instr_set.ld_to_HL(regs.C); break;
        case 0x72: instr_set.ld_to_HL(regs.D); break;
        case 0x73: instr_set.ld_to_HL(regs.E); break;
        case 0x74: instr_set.ld_to_HL(regs.H); break;
        case 0x75: instr_set.ld_to_HL(regs.L); break;
        case 0x77: instr_set.ld_to_HL(regs.A); break;
        case 0x78: instr_set.ld(regs.A, regs.B); break;
        case 0x79: instr_set.ld(regs.A, regs.C); break;
        case 0x7A: instr_set.ld(regs.A, regs.D); break;
        case 0x7B: instr_set.ld(regs.A, regs.E); break;
        case 0x7C: instr_set.ld(regs.A, regs.H); break;
        case 0x7D: instr_set.ld(regs.A, regs.L); break;
        case 0x7E: instr_set.ld_from_HL(regs.A); break;
        case 0x7F: instr_set.ld(regs.A, regs.A); break;

        case 0xE0: instr_set.ldh(FROM_A); break;
        case 0xE2: instr_set.ldh_C(FROM_A); break;
        case 0xEA: instr_set.ld_to_mem(); break;

        case 0xF0: instr_set.ldh(TO_A); break;
        case 0xF2: instr_set.ldh_C(TO_A); break;
        case 0xFA: instr_set.ld_from_mem(); break;

        default: 
            std::cout << "Unknown opcode: unable to decode into an instruction\n";
            return false;
    }

    return true;

}