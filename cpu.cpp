#include "cpu.h"

CPU::CPU(MemoryBus &bus_) : bus(bus_), instr_set(regs, ctx, bus), int_handler(regs, ctx, bus) {}
CPU::~CPU() {}

bool CPU::step() {
    
    if (!ctx.halted) {

        std::cout << "PC = 0x" << std::hex << std::setw(4) << std::setfill('0') << regs.PC << ":";

        // Fetch opcode
        u8 opcode = bus.read(regs.PC++); 
        instr_set.emulate_cycles(1);
        std::cout << " Opcode: 0x" << std::hex << std::setw(2) << std::setfill('0') << +opcode;

        // Debugging flags and registers
        char z = BIT(regs.F, 7) ? 'Z' : '-';
        char n = BIT(regs.F, 6) ? 'N' : '-';
        char h = BIT(regs.F, 5) ? 'H' : '-';
        char c = BIT(regs.F, 4) ? 'C' : '-';
        std::cout << " Flags set: " << z << n << h << c;

        std::cout << " AF: 0x" 
            << std::hex << std::setw(2) << std::setfill('0') << +regs.A 
            << std::hex << std::setw(2) << std::setfill('0') << +regs.F;
        std::cout << " BC: 0x" 
            << std::hex << std::setw(2) << std::setfill('0') << +regs.B 
            << std::hex << std::setw(2) << std::setfill('0') << +regs.C; 
        std::cout << " DE: 0x" 
            << std::hex << std::setw(2) << std::setfill('0') << +regs.D 
            << std::hex << std::setw(2) << std::setfill('0') << +regs.E; 
        std::cout << " HL: 0x" 
            << std::hex << std::setw(2) << std::setfill('0') << +regs.H 
            << std::hex << std::setw(2) << std::setfill('0') << +regs.L;  
        std::cout << " SP: 0x" 
            << std::hex << std::setw(4) << std::setfill('0') << +regs.SP; 

        std::cout << std::endl;
    
        // Decode and execute opcode
        if (!decode_and_execute(opcode)) {
            std::cout << "CPU could not decode or execute an instruction\n";
            return false;
        }

        // Printing from serial port for blargg tests
        if (bus.read(0xFF02) == 0x81) {
            char debug_c = bus.read(0xFF01);
            debug_msg[debug_msg_size++] = debug_c;
            bus.write(0xFF02, 0);
        }

        if (debug_msg[0]) {
            std::cout << "Serial port: " << debug_msg << std::endl;
        }
    }

    if (ctx.IME) {
        int_handler.handle_interrupts();
    }

    if (ctx.IME_next) { // Enable interrupts
        ctx.IME = true;
        ctx.IME_next = false;
    }
    
    return true;
}

bool CPU::decode_and_execute(u8 opcode) {

    switch (opcode) {
        
        case 0x00: instr_set.nop();                     break;
        case 0x01: instr_set.ld16(regs.B, regs.C);      break;
        case 0x02: instr_set.ld_from_A(regs.B, regs.C); break;
        case 0x03: instr_set.inc(regs.B, regs.C);       break;
        case 0x04: instr_set.inc(regs.B);               break;
        case 0x05: instr_set.dec(regs.B);               break;
        case 0x06: instr_set.ld(regs.B);                break;
        // case 0x07: instr_set.rlca();                    break;
        case 0x08: instr_set.ld_from_SP();              break;
        // case 0x09: instr_set.add16(regs.B, regs.C);     break;
        // case 0x0A: instr_set.ld_to_A(regs.B, regs.C);   break;
        // case 0x0B: instr_set.dec(regs.B, regs.C);       break;
        case 0x0C: instr_set.inc(regs.C);               break;
        case 0x0D: instr_set.dec(regs.C);               break;
        case 0x0E: instr_set.ld(regs.C);                break;
        // case 0x0F: instr_set.rrca();                    break;

        // case 0x10: instr_set.stop();                    break;
        case 0x11: instr_set.ld16(regs.D, regs.E);      break;
        case 0x12: instr_set.ld_from_A(regs.D, regs.E); break;
        case 0x13: instr_set.inc(regs.D, regs.E);       break;
        case 0x14: instr_set.inc(regs.D);               break;
        // case 0x15: instr_set.dec(regs.D);               break;
        // case 0x16: instr_set.ld(regs.D);                break;
        // case 0x17: instr_set.rla();                     break;
        case 0x18: instr_set.jr();                      break;
        // case 0x19: instr_set.add16(regs.D, regs.E);     break;
        case 0x1A: instr_set.ld_to_A(regs.D, regs.E);   break;
        // case 0x1B: instr_set.dec(regs.D, regs.E);       break;
        case 0x1C: instr_set.inc(regs.E);               break;
        case 0x1D: instr_set.dec(regs.E);               break;
        // case 0x1E: instr_set.ld(regs.E);                break;
        case 0x1F: instr_set.rra();                     break;
        
        case 0x20: instr_set.jr(!BIT(regs.F,7));             break;
        case 0x21: instr_set.ld16(regs.H, regs.L);           break;
        case 0x22: instr_set.ld_from_A(regs.H, regs.L, LDI); break;
        case 0x23: instr_set.inc(regs.H, regs.L);            break;
        case 0x24: instr_set.inc(regs.H);                    break;
        case 0x25: instr_set.dec(regs.H);                    break;
        case 0x26: instr_set.ld(regs.H);                     break;
        case 0x27: instr_set.daa();                          break;
        case 0x28: instr_set.jr(BIT(regs.F,7));              break;
        case 0x29: instr_set.add16(regs.H, regs.L);          break;
        case 0x2A: instr_set.ld_to_A(regs.H, regs.L, LDI);   break;
        // case 0x2B: instr_set.dec(regs.H, regs.L);            break;
        case 0x2C: instr_set.inc(regs.L);                    break;
        case 0x2D: instr_set.dec(regs.L);                    break;
        case 0x2E: instr_set.ld(regs.L);                     break;
        case 0x2F: instr_set.cpl();                          break;
        
        case 0x30: instr_set.jr(!BIT(regs.F,4));             break;
        case 0x31: instr_set.ld16(regs.SP);                  break;
        case 0x32: instr_set.ld_from_A(regs.H, regs.L, LDD); break;
        case 0x33: instr_set.inc_SP();                       break;
        // case 0x34: instr_set.inc_HL();                       break;
        case 0x35: instr_set.dec_HL();                       break;
        // case 0x36: instr_set.ld_to_HL();                     break;
        // case 0x37: instr_set.scf();                          break;
        case 0x38: instr_set.jr(BIT(regs.F,4));              break;
        case 0x39: instr_set.add16();                        break;
        // case 0x3A: instr_set.ld_to_A(regs.H, regs.L, LDD);   break;
        case 0x3B: instr_set.dec_SP();                       break;
        case 0x3C: instr_set.inc(regs.A);                    break;
        case 0x3D: instr_set.dec(regs.A);                    break;
        case 0x3E: instr_set.ld(regs.A);                     break;
        // case 0x3F: instr_set.ccf();                          break;
        
        // case 0x40: instr_set.ld(regs.B, regs.B); break;
        // case 0x41: instr_set.ld(regs.B, regs.C); break;
        // case 0x42: instr_set.ld(regs.B, regs.D); break;
        // case 0x43: instr_set.ld(regs.B, regs.E); break;
        // case 0x44: instr_set.ld(regs.B, regs.H); break;
        // case 0x45: instr_set.ld(regs.B, regs.L); break;
        case 0x46: instr_set.ld_from_HL(regs.B); break;
        case 0x47: instr_set.ld(regs.B, regs.A); break;
        // case 0x48: instr_set.ld(regs.C, regs.B); break;
        // case 0x49: instr_set.ld(regs.C, regs.C); break;
        // case 0x4A: instr_set.ld(regs.C, regs.D); break;
        // case 0x4B: instr_set.ld(regs.C, regs.E); break;
        // case 0x4C: instr_set.ld(regs.C, regs.H); break;
        // case 0x4D: instr_set.ld(regs.C, regs.L); break;
        case 0x4E: instr_set.ld_from_HL(regs.C); break;
        case 0x4F: instr_set.ld(regs.C, regs.A); break;

        // case 0x50: instr_set.ld(regs.D, regs.B); break;
        // case 0x51: instr_set.ld(regs.D, regs.C); break;
        // case 0x52: instr_set.ld(regs.D, regs.D); break;
        // case 0x53: instr_set.ld(regs.D, regs.E); break;
        // case 0x54: instr_set.ld(regs.D, regs.H); break;
        // case 0x55: instr_set.ld(regs.D, regs.L); break;
        case 0x56: instr_set.ld_from_HL(regs.D); break;
        case 0x57: instr_set.ld(regs.D, regs.A); break;
        // case 0x58: instr_set.ld(regs.E, regs.B); break;
        // case 0x59: instr_set.ld(regs.E, regs.C); break;
        // case 0x5A: instr_set.ld(regs.E, regs.D); break;
        // case 0x5B: instr_set.ld(regs.E, regs.E); break;
        // case 0x5C: instr_set.ld(regs.E, regs.H); break;
        case 0x5D: instr_set.ld(regs.E, regs.L); break;
        case 0x5E: instr_set.ld_from_HL(regs.E); break;
        case 0x5F: instr_set.ld(regs.E, regs.A); break;

        // case 0x60: instr_set.ld(regs.H, regs.B); break;
        // case 0x61: instr_set.ld(regs.H, regs.C); break;
        case 0x62: instr_set.ld(regs.H, regs.D); break;
        // case 0x63: instr_set.ld(regs.H, regs.E); break;
        // case 0x64: instr_set.ld(regs.H, regs.H); break;
        // case 0x65: instr_set.ld(regs.H, regs.L); break;
        case 0x66: instr_set.ld_from_HL(regs.H); break;
        case 0x67: instr_set.ld(regs.H, regs.A); break;
        // case 0x68: instr_set.ld(regs.L, regs.B); break;
        // case 0x69: instr_set.ld(regs.L, regs.C); break;
        // case 0x6A: instr_set.ld(regs.L, regs.D); break;
        case 0x6B: instr_set.ld(regs.L, regs.E); break;
        // case 0x6C: instr_set.ld(regs.L, regs.H); break;
        // case 0x6D: instr_set.ld(regs.L, regs.L); break;
        case 0x6E: instr_set.ld_from_HL(regs.L); break;
        case 0x6F: instr_set.ld(regs.L, regs.A); break;
        
        case 0x70: instr_set.ld_to_HL(regs.B); break;
        case 0x71: instr_set.ld_to_HL(regs.C); break;
        case 0x72: instr_set.ld_to_HL(regs.D); break;
        case 0x73: instr_set.ld_to_HL(regs.E); break;
        // case 0x74: instr_set.ld_to_HL(regs.H); break;
        // case 0x75: instr_set.ld_to_HL(regs.L); break;
        // case 0x76: instr_set.halt();           break;
        case 0x77: instr_set.ld_to_HL(regs.A); break;
        case 0x78: instr_set.ld(regs.A, regs.B); break;
        case 0x79: instr_set.ld(regs.A, regs.C); break;
        case 0x7A: instr_set.ld(regs.A, regs.D); break;
        case 0x7B: instr_set.ld(regs.A, regs.E); break;
        case 0x7C: instr_set.ld(regs.A, regs.H); break;
        case 0x7D: instr_set.ld(regs.A, regs.L); break;
        case 0x7E: instr_set.ld_from_HL(regs.A); break;
        // case 0x7F: instr_set.ld(regs.A, regs.A); break;

        // case 0x80: instr_set.add(regs.B); break;
        // case 0x81: instr_set.add(regs.C); break;
        // case 0x82: instr_set.add(regs.D); break;
        // case 0x83: instr_set.add(regs.E); break;
        // case 0x84: instr_set.add(regs.H); break;
        // case 0x85: instr_set.add(regs.L); break;
        // case 0x86: instr_set.add_HL();    break;
        // case 0x87: instr_set.add(regs.A); break;
        // case 0x88: instr_set.adc(regs.B); break;
        // case 0x89: instr_set.adc(regs.C); break;
        // case 0x8A: instr_set.adc(regs.D); break;
        // case 0x8B: instr_set.adc(regs.E); break;
        // case 0x8C: instr_set.adc(regs.H); break;
        // case 0x8D: instr_set.adc(regs.L); break;
        // case 0x8E: instr_set.adc_HL();    break;
        // case 0x8F: instr_set.adc(regs.A); break;

        // case 0x90: instr_set.sub(regs.B); break;
        // case 0x91: instr_set.sub(regs.C); break;
        // case 0x92: instr_set.sub(regs.D); break;
        // case 0x93: instr_set.sub(regs.E); break;
        // case 0x94: instr_set.sub(regs.H); break;
        // case 0x95: instr_set.sub(regs.L); break;
        // case 0x96: instr_set.sub_HL();    break;
        // case 0x97: instr_set.sub(regs.A); break;
        // case 0x98: instr_set.sbc(regs.B); break;
        // case 0x99: instr_set.sbc(regs.C); break;
        // case 0x9A: instr_set.sbc(regs.D); break;
        // case 0x9B: instr_set.sbc(regs.E); break;
        // case 0x9C: instr_set.sbc(regs.H); break;
        // case 0x9D: instr_set.sbc(regs.L); break;
        // case 0x9E: instr_set.sbc_HL();    break;
        // case 0x9F: instr_set.sbc(regs.A); break;

        // case 0xA0: instr_set.and_A(regs.B); break;
        // case 0xA1: instr_set.and_A(regs.C); break;
        // case 0xA2: instr_set.and_A(regs.D); break;
        // case 0xA3: instr_set.and_A(regs.E); break;
        // case 0xA4: instr_set.and_A(regs.H); break;
        // case 0xA5: instr_set.and_A(regs.L); break;
        // case 0xA6: instr_set.and_A_HL();    break;
        // case 0xA7: instr_set.and_A(regs.A); break;
        // case 0xA8: instr_set.xor_A(regs.B); break;
        case 0xA9: instr_set.xor_A(regs.C); break;
        // case 0xAA: instr_set.xor_A(regs.D); break;
        // case 0xAB: instr_set.xor_A(regs.E); break;
        // case 0xAC: instr_set.xor_A(regs.H); break;
        case 0xAD: instr_set.xor_A(regs.L); break;
        case 0xAE: instr_set.xor_A_HL();    break;
        case 0xAF: instr_set.xor_A(regs.A); break;

        case 0xB0: instr_set.or_A(regs.B);  break;
        case 0xB1: instr_set.or_A(regs.C);  break;
        // case 0xB2: instr_set.or_A(regs.D);  break;
        // case 0xB3: instr_set.or_A(regs.E);  break;
        // case 0xB4: instr_set.or_A(regs.H);  break;
        // case 0xB5: instr_set.or_A(regs.L);  break;
        case 0xB6: instr_set.or_A_HL();     break;
        case 0xB7: instr_set.or_A(regs.A);  break;
        case 0xB8: instr_set.cp(regs.B);    break;
        case 0xB9: instr_set.cp(regs.C);    break;
        case 0xBA: instr_set.cp(regs.D);    break;
        case 0xBB: instr_set.cp(regs.E);    break;
        // case 0xBC: instr_set.cp(regs.H);    break;
        // case 0xBD: instr_set.cp(regs.L);    break;
        // case 0xBE: instr_set.cp_HL();       break;
        // case 0xBF: instr_set.cp(regs.A);    break;

        // case 0xC0: instr_set.ret(!BIT(regs.F, 7), RET_CC); break;
        case 0xC1: instr_set.pop(regs.B, regs.C);          break;
        case 0xC2: instr_set.jp(!BIT(regs.F, 7));          break;
        case 0xC3: instr_set.jp();                         break;
        case 0xC4: instr_set.call(!BIT(regs.F, 7));        break;
        case 0xC5: instr_set.push(regs.B, regs.C);         break;
        case 0xC6: instr_set.add();                        break;
        // case 0xC7: instr_set.rst(0x00);                    break;
        case 0xC8: instr_set.ret(BIT(regs.F, 7), RET_CC);  break;
        case 0xC9: instr_set.ret();                        break;
        // case 0xCA: instr_set.jp(BIT(regs.F, 7));           break;
        // case 0xCC: instr_set.call(BIT(regs.F, 7));         break;
        case 0xCD: instr_set.call();                       break;
        case 0xCE: instr_set.adc();                        break;
        // case 0xCF: instr_set.rst(0x08);                    break;

        case 0xD0: instr_set.ret(!BIT(regs.F, 4), RET_CC); break;
        case 0xD1: instr_set.pop(regs.D, regs.E);          break;
        // case 0xD2: instr_set.jp(!BIT(regs.F, 4));          break;
        // case 0xD4: instr_set.call(!BIT(regs.F, 4));        break;
        case 0xD5: instr_set.push(regs.D, regs.E);         break;
        case 0xD6: instr_set.sub();                        break;
        // case 0xD7: instr_set.rst(0x10);                    break;
        case 0xD8: instr_set.ret(BIT(regs.F, 4), RET_CC);  break;
        // case 0xD9: instr_set.reti();                       break;
        // case 0xDA: instr_set.jp(BIT(regs.F, 4));           break;
        // case 0xDC: instr_set.call(BIT(regs.F, 4));         break;
        // case 0xDE: instr_set.sbc();                        break;
        // case 0xDF: instr_set.rst(0x18);                    break;

        case 0xE0: instr_set.ldh_from_A(LDH_A8);   break;
        case 0xE1: instr_set.pop(regs.H, regs.L);  break;
        // case 0xE2: instr_set.ldh_from_A(LDH_C);    break;
        case 0xE5: instr_set.push(regs.H, regs.L); break;
        case 0xE6: instr_set.and_A();              break;
        // case 0xE7: instr_set.rst(0x20);            break;
        case 0xE8: instr_set.add_to_SP();          break;
        case 0xE9: instr_set.jp_HL();              break;
        case 0xEA: instr_set.ld_from_A();          break;
        case 0xEE: instr_set.xor_A();              break;
        // case 0xEF: instr_set.rst(0x28);            break;

        case 0xF0: instr_set.ldh_to_A(LDH_A8);            break;
        case 0xF1: instr_set.pop(regs.A, regs.F, POP_AF); break;
        // case 0xF2: instr_set.ldh_to_A(LDH_C);             break;
        case 0xF3: instr_set.di();                        break;
        case 0xF5: instr_set.push(regs.A, regs.F & 0xF0); break;
        // case 0xF6: instr_set.or_A();                      break;
        // case 0xF7: instr_set.rst(0x30);                   break;
        case 0xF8: instr_set.ld_SP_signed();              break;
        case 0xF9: instr_set.ld_SP_HL();                  break;
        case 0xFA: instr_set.ld_to_A();                   break;
        // case 0xFB: instr_set.ei();                        break;
        case 0xFE: instr_set.cp();                        break;
        // case 0xFF: instr_set.rst(0x38);                   break;
        
        case 0xCB:
            opcode = bus.read(regs.PC++);
            std::cout << "Encountered prefixed 0xCB code: 0x" 
                << std::hex << +opcode << std::endl;

            switch (opcode) {
        //         case 0x00: instr_set.shift(RLC, regs.B); break;
        //         case 0x01: instr_set.shift(RLC, regs.C); break;
        //         case 0x02: instr_set.shift(RLC, regs.D); break;
        //         case 0x03: instr_set.shift(RLC, regs.E); break;
        //         case 0x04: instr_set.shift(RLC, regs.H); break;
        //         case 0x05: instr_set.shift(RLC, regs.L); break;
        //         case 0x06: instr_set.shift_HL(RLC);      break;
        //         case 0x07: instr_set.shift(RLC, regs.A); break;
        //         case 0x08: instr_set.shift(RRC, regs.B); break;
        //         case 0x09: instr_set.shift(RRC, regs.C); break;
        //         case 0x0A: instr_set.shift(RRC, regs.D); break;
        //         case 0x0B: instr_set.shift(RRC, regs.E); break;
        //         case 0x0C: instr_set.shift(RRC, regs.H); break;
        //         case 0x0D: instr_set.shift(RRC, regs.L); break;
        //         case 0x0E: instr_set.shift_HL(RRC);      break;
        //         case 0x0F: instr_set.shift(RRC, regs.A); break;

        //         case 0x10: instr_set.shift(RL, regs.B); break;
        //         case 0x11: instr_set.shift(RL, regs.C); break;
        //         case 0x12: instr_set.shift(RL, regs.D); break;
        //         case 0x13: instr_set.shift(RL, regs.E); break;
        //         case 0x14: instr_set.shift(RL, regs.H); break;
        //         case 0x15: instr_set.shift(RL, regs.L); break;
        //         case 0x16: instr_set.shift_HL(RL);      break;
        //         case 0x17: instr_set.shift(RL, regs.A); break;
        //         case 0x18: instr_set.shift(RR, regs.B); break;
                case 0x19: instr_set.shift(RR, regs.C); break;
                case 0x1A: instr_set.shift(RR, regs.D); break;
                case 0x1B: instr_set.shift(RR, regs.E); break;
        //         case 0x1C: instr_set.shift(RR, regs.H); break;
        //         case 0x1D: instr_set.shift(RR, regs.L); break;
        //         case 0x1E: instr_set.shift_HL(RR);      break;
        //         case 0x1F: instr_set.shift(RR, regs.A); break;

        //         case 0x20: instr_set.shift(SLA, regs.B); break;
        //         case 0x21: instr_set.shift(SLA, regs.C); break;
        //         case 0x22: instr_set.shift(SLA, regs.D); break;
        //         case 0x23: instr_set.shift(SLA, regs.E); break;
        //         case 0x24: instr_set.shift(SLA, regs.H); break;
        //         case 0x25: instr_set.shift(SLA, regs.L); break;
        //         case 0x26: instr_set.shift_HL(SLA);      break;
        //         case 0x27: instr_set.shift(SLA, regs.A); break;
        //         case 0x28: instr_set.shift(SRA, regs.B); break;
        //         case 0x29: instr_set.shift(SRA, regs.C); break;
        //         case 0x2A: instr_set.shift(SRA, regs.D); break;
        //         case 0x2B: instr_set.shift(SRA, regs.E); break;
        //         case 0x2C: instr_set.shift(SRA, regs.H); break;
        //         case 0x2D: instr_set.shift(SRA, regs.L); break;
        //         case 0x2E: instr_set.shift_HL(SRA);      break;
        //         case 0x2F: instr_set.shift(SRA, regs.A); break;

        //         case 0x30: instr_set.shift(SWAP, regs.B); break;
        //         case 0x31: instr_set.shift(SWAP, regs.C); break;
        //         case 0x32: instr_set.shift(SWAP, regs.D); break;
        //         case 0x33: instr_set.shift(SWAP, regs.E); break;
        //         case 0x34: instr_set.shift(SWAP, regs.H); break;
        //         case 0x35: instr_set.shift(SWAP, regs.L); break;
        //         case 0x36: instr_set.shift_HL(SWAP);      break;
                case 0x37: instr_set.shift(SWAP, regs.A); break;
                case 0x38: instr_set.shift(SRL, regs.B); break;
        //         case 0x39: instr_set.shift(SRL, regs.C); break;
        //         case 0x3A: instr_set.shift(SRL, regs.D); break;
        //         case 0x3B: instr_set.shift(SRL, regs.E); break;
        //         case 0x3C: instr_set.shift(SRL, regs.H); break;
        //         case 0x3D: instr_set.shift(SRL, regs.L); break;
        //         case 0x3E: instr_set.shift_HL(SRL);      break;
        //         case 0x3F: instr_set.shift(SRL, regs.A); break;

        //         case 0x40: instr_set.bit_flag(BIT, 0, regs.B); break;
        //         case 0x41: instr_set.bit_flag(BIT, 0, regs.C); break;
        //         case 0x42: instr_set.bit_flag(BIT, 0, regs.D); break;
        //         case 0x43: instr_set.bit_flag(BIT, 0, regs.E); break;
        //         case 0x44: instr_set.bit_flag(BIT, 0, regs.H); break;
        //         case 0x45: instr_set.bit_flag(BIT, 0, regs.L); break;
        //         case 0x46: instr_set.bit_flag_HL(BIT, 0);      break;
        //         case 0x47: instr_set.bit_flag(BIT, 0, regs.A); break;
        //         case 0x48: instr_set.bit_flag(BIT, 1, regs.B); break;
        //         case 0x49: instr_set.bit_flag(BIT, 1, regs.C); break;
        //         case 0x4A: instr_set.bit_flag(BIT, 1, regs.D); break;
        //         case 0x4B: instr_set.bit_flag(BIT, 1, regs.E); break;
        //         case 0x4C: instr_set.bit_flag(BIT, 1, regs.H); break;
        //         case 0x4D: instr_set.bit_flag(BIT, 1, regs.L); break;
        //         case 0x4E: instr_set.bit_flag_HL(BIT, 1);      break;
        //         case 0x4F: instr_set.bit_flag(BIT, 1, regs.A); break;

        //         case 0x50: instr_set.bit_flag(BIT, 2, regs.B); break;
        //         case 0x51: instr_set.bit_flag(BIT, 2, regs.C); break;
        //         case 0x52: instr_set.bit_flag(BIT, 2, regs.D); break;
        //         case 0x53: instr_set.bit_flag(BIT, 2, regs.E); break;
        //         case 0x54: instr_set.bit_flag(BIT, 2, regs.H); break;
        //         case 0x55: instr_set.bit_flag(BIT, 2, regs.L); break;
        //         case 0x56: instr_set.bit_flag_HL(BIT, 2);      break;
        //         case 0x57: instr_set.bit_flag(BIT, 2, regs.A); break;
        //         case 0x58: instr_set.bit_flag(BIT, 3, regs.B); break;
        //         case 0x59: instr_set.bit_flag(BIT, 3, regs.C); break;
        //         case 0x5A: instr_set.bit_flag(BIT, 3, regs.D); break;
        //         case 0x5B: instr_set.bit_flag(BIT, 3, regs.E); break;
        //         case 0x5C: instr_set.bit_flag(BIT, 3, regs.H); break;
        //         case 0x5D: instr_set.bit_flag(BIT, 3, regs.L); break;
        //         case 0x5E: instr_set.bit_flag_HL(BIT, 3);      break;
        //         case 0x5F: instr_set.bit_flag(BIT, 3, regs.A); break;

        //         case 0x60: instr_set.bit_flag(BIT, 4, regs.B); break;
        //         case 0x61: instr_set.bit_flag(BIT, 4, regs.C); break;
        //         case 0x62: instr_set.bit_flag(BIT, 4, regs.D); break;
        //         case 0x63: instr_set.bit_flag(BIT, 4, regs.E); break;
        //         case 0x64: instr_set.bit_flag(BIT, 4, regs.H); break;
        //         case 0x65: instr_set.bit_flag(BIT, 4, regs.L); break;
        //         case 0x66: instr_set.bit_flag_HL(BIT, 4);      break;
        //         case 0x67: instr_set.bit_flag(BIT, 4, regs.A); break;
        //         case 0x68: instr_set.bit_flag(BIT, 5, regs.B); break;
        //         case 0x69: instr_set.bit_flag(BIT, 5, regs.C); break;
        //         case 0x6A: instr_set.bit_flag(BIT, 5, regs.D); break;
        //         case 0x6B: instr_set.bit_flag(BIT, 5, regs.E); break;
        //         case 0x6C: instr_set.bit_flag(BIT, 5, regs.H); break;
        //         case 0x6D: instr_set.bit_flag(BIT, 5, regs.L); break;
        //         case 0x6E: instr_set.bit_flag_HL(BIT, 5);      break;
        //         case 0x6F: instr_set.bit_flag(BIT, 5, regs.A); break;

        //         case 0x70: instr_set.bit_flag(BIT, 6, regs.B); break;
        //         case 0x71: instr_set.bit_flag(BIT, 6, regs.C); break;
        //         case 0x72: instr_set.bit_flag(BIT, 6, regs.D); break;
        //         case 0x73: instr_set.bit_flag(BIT, 6, regs.E); break;
        //         case 0x74: instr_set.bit_flag(BIT, 6, regs.H); break;
        //         case 0x75: instr_set.bit_flag(BIT, 6, regs.L); break;
        //         case 0x76: instr_set.bit_flag_HL(BIT, 6);      break;
        //         case 0x77: instr_set.bit_flag(BIT, 6, regs.A); break;
        //         case 0x78: instr_set.bit_flag(BIT, 7, regs.B); break;
        //         case 0x79: instr_set.bit_flag(BIT, 7, regs.C); break;
        //         case 0x7A: instr_set.bit_flag(BIT, 7, regs.D); break;
        //         case 0x7B: instr_set.bit_flag(BIT, 7, regs.E); break;
        //         case 0x7C: instr_set.bit_flag(BIT, 7, regs.H); break;
        //         case 0x7D: instr_set.bit_flag(BIT, 7, regs.L); break;
        //         case 0x7E: instr_set.bit_flag_HL(BIT, 7);      break;
        //         case 0x7F: instr_set.bit_flag(BIT, 7, regs.A); break;

        //         case 0x80: instr_set.bit_flag(RES, 0, regs.B); break;
        //         case 0x81: instr_set.bit_flag(RES, 0, regs.C); break;
        //         case 0x82: instr_set.bit_flag(RES, 0, regs.D); break;
        //         case 0x83: instr_set.bit_flag(RES, 0, regs.E); break;
        //         case 0x84: instr_set.bit_flag(RES, 0, regs.H); break;
        //         case 0x85: instr_set.bit_flag(RES, 0, regs.L); break;
        //         case 0x86: instr_set.bit_flag_HL(RES, 0);      break;
        //         case 0x87: instr_set.bit_flag(RES, 0, regs.A); break;
        //         case 0x88: instr_set.bit_flag(RES, 1, regs.B); break;
        //         case 0x89: instr_set.bit_flag(RES, 1, regs.C); break;
        //         case 0x8A: instr_set.bit_flag(RES, 1, regs.D); break;
        //         case 0x8B: instr_set.bit_flag(RES, 1, regs.E); break;
        //         case 0x8C: instr_set.bit_flag(RES, 1, regs.H); break;
        //         case 0x8D: instr_set.bit_flag(RES, 1, regs.L); break;
        //         case 0x8E: instr_set.bit_flag_HL(RES, 1);      break;
        //         case 0x8F: instr_set.bit_flag(RES, 1, regs.A); break;

        //         case 0x90: instr_set.bit_flag(RES, 2, regs.B); break;
        //         case 0x91: instr_set.bit_flag(RES, 2, regs.C); break;
        //         case 0x92: instr_set.bit_flag(RES, 2, regs.D); break;
        //         case 0x93: instr_set.bit_flag(RES, 2, regs.E); break;
        //         case 0x94: instr_set.bit_flag(RES, 2, regs.H); break;
        //         case 0x95: instr_set.bit_flag(RES, 2, regs.L); break;
        //         case 0x96: instr_set.bit_flag_HL(RES, 2);      break;
        //         case 0x97: instr_set.bit_flag(RES, 2, regs.A); break;
        //         case 0x98: instr_set.bit_flag(RES, 3, regs.B); break;
        //         case 0x99: instr_set.bit_flag(RES, 3, regs.C); break;
        //         case 0x9A: instr_set.bit_flag(RES, 3, regs.D); break;
        //         case 0x9B: instr_set.bit_flag(RES, 3, regs.E); break;
        //         case 0x9C: instr_set.bit_flag(RES, 3, regs.H); break;
        //         case 0x9D: instr_set.bit_flag(RES, 3, regs.L); break;
        //         case 0x9E: instr_set.bit_flag_HL(RES, 3);      break;
        //         case 0x9F: instr_set.bit_flag(RES, 3, regs.A); break;

        //         case 0xA0: instr_set.bit_flag(RES, 4, regs.B); break;
        //         case 0xA1: instr_set.bit_flag(RES, 4, regs.C); break;
        //         case 0xA2: instr_set.bit_flag(RES, 4, regs.D); break;
        //         case 0xA3: instr_set.bit_flag(RES, 4, regs.E); break;
        //         case 0xA4: instr_set.bit_flag(RES, 4, regs.H); break;
        //         case 0xA5: instr_set.bit_flag(RES, 4, regs.L); break;
        //         case 0xA6: instr_set.bit_flag_HL(RES, 4);      break;
        //         case 0xA7: instr_set.bit_flag(RES, 4, regs.A); break;
        //         case 0xA8: instr_set.bit_flag(RES, 5, regs.B); break;
        //         case 0xA9: instr_set.bit_flag(RES, 5, regs.C); break;
        //         case 0xAA: instr_set.bit_flag(RES, 5, regs.D); break;
        //         case 0xAB: instr_set.bit_flag(RES, 5, regs.E); break;
        //         case 0xAC: instr_set.bit_flag(RES, 5, regs.H); break;
        //         case 0xAD: instr_set.bit_flag(RES, 5, regs.L); break;
        //         case 0xAE: instr_set.bit_flag_HL(RES, 5);      break;
        //         case 0xAF: instr_set.bit_flag(RES, 5, regs.A); break;

        //         case 0xB0: instr_set.bit_flag(RES, 6, regs.B); break;
        //         case 0xB1: instr_set.bit_flag(RES, 6, regs.C); break;
        //         case 0xB2: instr_set.bit_flag(RES, 6, regs.D); break;
        //         case 0xB3: instr_set.bit_flag(RES, 6, regs.E); break;
        //         case 0xB4: instr_set.bit_flag(RES, 6, regs.H); break;
        //         case 0xB5: instr_set.bit_flag(RES, 6, regs.L); break;
        //         case 0xB6: instr_set.bit_flag_HL(RES, 6);      break;
        //         case 0xB7: instr_set.bit_flag(RES, 6, regs.A); break;
        //         case 0xB8: instr_set.bit_flag(RES, 7, regs.B); break;
        //         case 0xB9: instr_set.bit_flag(RES, 7, regs.C); break;
        //         case 0xBA: instr_set.bit_flag(RES, 7, regs.D); break;
        //         case 0xBB: instr_set.bit_flag(RES, 7, regs.E); break;
        //         case 0xBC: instr_set.bit_flag(RES, 7, regs.H); break;
        //         case 0xBD: instr_set.bit_flag(RES, 7, regs.L); break;
        //         case 0xBE: instr_set.bit_flag_HL(RES, 7);      break;
        //         case 0xBF: instr_set.bit_flag(RES, 7, regs.A); break;

        //         case 0xC0: instr_set.bit_flag(SET, 0, regs.B); break;
        //         case 0xC1: instr_set.bit_flag(SET, 0, regs.C); break;
        //         case 0xC2: instr_set.bit_flag(SET, 0, regs.D); break;
        //         case 0xC3: instr_set.bit_flag(SET, 0, regs.E); break;
        //         case 0xC4: instr_set.bit_flag(SET, 0, regs.H); break;
        //         case 0xC5: instr_set.bit_flag(SET, 0, regs.L); break;
        //         case 0xC6: instr_set.bit_flag_HL(SET, 0);      break;
        //         case 0xC7: instr_set.bit_flag(SET, 0, regs.A); break;
        //         case 0xC8: instr_set.bit_flag(SET, 1, regs.B); break;
        //         case 0xC9: instr_set.bit_flag(SET, 1, regs.C); break;
        //         case 0xCA: instr_set.bit_flag(SET, 1, regs.D); break;
        //         case 0xCB: instr_set.bit_flag(SET, 1, regs.E); break;
        //         case 0xCC: instr_set.bit_flag(SET, 1, regs.H); break;
        //         case 0xCD: instr_set.bit_flag(SET, 1, regs.L); break;
        //         case 0xCE: instr_set.bit_flag_HL(SET, 1);      break;
        //         case 0xCF: instr_set.bit_flag(SET, 1, regs.A); break;

        //         case 0xD0: instr_set.bit_flag(SET, 2, regs.B); break;
        //         case 0xD1: instr_set.bit_flag(SET, 2, regs.C); break;
        //         case 0xD2: instr_set.bit_flag(SET, 2, regs.D); break;
        //         case 0xD3: instr_set.bit_flag(SET, 2, regs.E); break;
        //         case 0xD4: instr_set.bit_flag(SET, 2, regs.H); break;
        //         case 0xD5: instr_set.bit_flag(SET, 2, regs.L); break;
        //         case 0xD6: instr_set.bit_flag_HL(SET, 2);      break;
        //         case 0xD7: instr_set.bit_flag(SET, 2, regs.A); break;
        //         case 0xD8: instr_set.bit_flag(SET, 3, regs.B); break;
        //         case 0xD9: instr_set.bit_flag(SET, 3, regs.C); break;
        //         case 0xDA: instr_set.bit_flag(SET, 3, regs.D); break;
        //         case 0xDB: instr_set.bit_flag(SET, 3, regs.E); break;
        //         case 0xDC: instr_set.bit_flag(SET, 3, regs.H); break;
        //         case 0xDD: instr_set.bit_flag(SET, 3, regs.L); break;
        //         case 0xDE: instr_set.bit_flag_HL(SET, 3);      break;
        //         case 0xDF: instr_set.bit_flag(SET, 3, regs.A); break;

        //         case 0xE0: instr_set.bit_flag(SET, 4, regs.B); break;
        //         case 0xE1: instr_set.bit_flag(SET, 4, regs.C); break;
        //         case 0xE2: instr_set.bit_flag(SET, 4, regs.D); break;
        //         case 0xE3: instr_set.bit_flag(SET, 4, regs.E); break;
        //         case 0xE4: instr_set.bit_flag(SET, 4, regs.H); break;
        //         case 0xE5: instr_set.bit_flag(SET, 4, regs.L); break;
        //         case 0xE6: instr_set.bit_flag_HL(SET, 4);      break;
        //         case 0xE7: instr_set.bit_flag(SET, 4, regs.A); break;
        //         case 0xE8: instr_set.bit_flag(SET, 5, regs.B); break;
        //         case 0xE9: instr_set.bit_flag(SET, 5, regs.C); break;
        //         case 0xEA: instr_set.bit_flag(SET, 5, regs.D); break;
        //         case 0xEB: instr_set.bit_flag(SET, 5, regs.E); break;
        //         case 0xEC: instr_set.bit_flag(SET, 5, regs.H); break;
        //         case 0xED: instr_set.bit_flag(SET, 5, regs.L); break;
        //         case 0xEE: instr_set.bit_flag_HL(SET, 5);      break;
        //         case 0xEF: instr_set.bit_flag(SET, 5, regs.A); break;

        //         case 0xF0: instr_set.bit_flag(SET, 6, regs.B); break;
        //         case 0xF1: instr_set.bit_flag(SET, 6, regs.C); break;
        //         case 0xF2: instr_set.bit_flag(SET, 6, regs.D); break;
        //         case 0xF3: instr_set.bit_flag(SET, 6, regs.E); break;
        //         case 0xF4: instr_set.bit_flag(SET, 6, regs.H); break;
        //         case 0xF5: instr_set.bit_flag(SET, 6, regs.L); break;
        //         case 0xF6: instr_set.bit_flag_HL(SET, 6);      break;
        //         case 0xF7: instr_set.bit_flag(SET, 6, regs.A); break;
        //         case 0xF8: instr_set.bit_flag(SET, 7, regs.B); break;
        //         case 0xF9: instr_set.bit_flag(SET, 7, regs.C); break;
        //         case 0xFA: instr_set.bit_flag(SET, 7, regs.D); break;
        //         case 0xFB: instr_set.bit_flag(SET, 7, regs.E); break;
        //         case 0xFC: instr_set.bit_flag(SET, 7, regs.H); break;
        //         case 0xFD: instr_set.bit_flag(SET, 7, regs.L); break;
        //         case 0xFE: instr_set.bit_flag_HL(SET, 7);      break;
        //         case 0xFF: instr_set.bit_flag(SET, 7, regs.A); break;
                default:
                    std::cout << "Unknown 0xCB opcode: 0x" << +opcode << std::endl;
                    return false;
            }
            break;

        case 0xD3: case 0xE3: case 0xE4: case 0xF4: case 0xDB: 
        case 0xEB: case 0xEC: case 0xFC: case 0xDD: case 0xED: case 0xFD:
            std::cout << "Invalid opcode!\n";
            return false;

        default: 
            std::cout << "Unknown opcode: 0x" << +opcode << std::endl;
            return false;
    }

    return true;

}