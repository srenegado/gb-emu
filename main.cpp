#include <iostream>
#include <cstdint>
#include <cstring>
#include "SDL.h"


class Memory {
    private:
        uint8_t map[0x10000];
    
    public:
        // Constructor
        Memory() {
            std::memset(map, 0, sizeof(map));
        }

        // Destructor
        ~Memory() {}

        uint8_t read(uint16_t addr) {
            return map[addr];
        }

        void write(uint16_t addr, uint8_t val) {
            map[addr] = val;
        }
};


typedef uint8_t Register8;

struct Register16 {
    Register8 hi;
    Register8 lo;

    // Constructor
    Register16() {
        hi = 0x0000;
        lo = 0x0000;
    }

    // Destructor
    ~Register16() {}

    /**
     * Combine values in hi and lo then 
     * return the resulting 2-byte value 
     */
    uint16_t get_data16() const {
        return ((uint16_t)hi << 8) | (uint16_t)lo;
    }

    /** 
     * Store a 16-bit value into the register
     */
    void set_data16(uint16_t data) {
        hi = (uint8_t)(data >> 8);
        lo = (uint8_t)(data & 0xFF);
    }

    // Addition assignment
    Register16& operator+=(Register16 reg) {
        uint16_t sum = this->get_data16() + reg.get_data16();
        this->hi = (uint8_t)(sum >> 8);
        this->lo = (uint8_t)(sum & 0xFF);
        return *this;
    }

    // Postfix increment
    Register16 operator++(int) {
        // Store temp state to return later
        Register16 temp = *this;

        // Combine hi and lo into 16-bit value, and increment it
        uint16_t full_value = ((uint16_t)hi << 8) | (uint16_t)lo;
        full_value++;

        // Update hi and lo
        hi = (uint8_t)(full_value >> 8);   // msb
        lo = (uint8_t)(full_value & 0xFF); // lsb

        return temp;
    }

    // Postfix decrement
    Register16 operator--(int) {
        // Store temp state to return later
        Register16 temp = *this;

        // Combine hi and lo into 16-bit value, and increment it
        uint16_t full_value = ((uint16_t)hi << 8) | (uint16_t)lo;
        full_value--;

        // Update hi and lo
        hi = (uint8_t)(full_value >> 8);   // msb
        lo = (uint8_t)(full_value & 0xFF); // lsb

        return temp;
    }
};


class CPU {
    private:

        // Registers
        Register16 AF;    // Accumulator and flags
        Register16 BC;    // General registers
        Register16 DE;
        Register16 HL;
        Register16 SP;    // Stack pointer

        uint16_t PC;      // Program counter  

        // Memory
        Memory mem;

        /** 
         * Opcodes
         * 
         * Each opcode returns the number of m-cycles they use
         */ 
    
        /**
         * Load value in src register into dest resigter
         */
        uint32_t LD_R8_R8(Register8 &dest, Register8 src) {
            dest = src;
            return 1;
        }

        /**
         * Store value in src register into memory[HL]
         */
        uint32_t LD_HL_R8(Register8 src) {
            mem.write(HL.get_data16(), src); 
            return 2;
        }

        /**
         * Load value in memory[HL] into dest register
         */
        uint32_t LD_R8_HL(Register8 &dest) {
            dest = mem.read(HL.get_data16());
            return 2;
        }

        /**
         * Load immediate 8-bit value n8 into dest register
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t LD_R8_n8(Register8 &dest) {

            // Get n8 and move PC to next instruction
            uint8_t n8 = mem.read(PC++);

            dest = n8;
            return 2;
        }

        /**
         * Store immediate 8-bit value n8 into memory[HL]
         *
         * Assumes PC is pointing to n8 before call 
         */
        uint32_t LD_HL_n8() {

            // Get n8 and move PC to next instruction
            uint8_t n8 = mem.read(PC++);

            mem.write(HL.get_data16(), n8);
            return 3;
        }

        /** 
         * Store value in register A into memory[dest]
         */
        uint32_t LD_R16_A(const Register16 &dest) {
            mem.write(dest.get_data16(), AF.hi);
            return 2;
        }

        /**
         * Load value in memory[dest] into register A 
         */
        uint32_t LD_A_R16(const Register16 &dest) {
            AF.hi = mem.read(dest.get_data16());
            return 2;    
        }

        /**
         * Load the immediate little-endian 16-bit value n16 into dest register
         *
         * Assumes PC is pointing to n16 before call 
         */
        uint32_t LD_R16_n16(Register16 &dest) {
            
            // Get n16 and move PC to next instruction
            uint8_t lsb = mem.read(PC++);
            uint8_t msb = mem.read(PC++);
            uint16_t n16 = ((uint16_t)msb << 8) | (uint16_t)lsb; 
            
            dest.set_data16(n16);
            return 3;
        }

        /** 
         * Store the value in SP into memory[n16], where n16 is the
         * immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t LD_n16_SP() {
            
            // Get n16 and move PC to next instruction
            uint8_t lsb = mem.read(PC++);
            uint8_t msb = mem.read(PC++);
            uint16_t n16 = ((uint16_t)msb << 8) | (uint16_t)lsb;

            // Store SP least-significant byte first
            mem.write(n16, SP.lo);
            mem.write(n16 + 1, SP.hi);

            return 5;
        }

        /** 
         * Store the value in register A into memory[n16], where n16 is the
         * immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t LD_n16_A() {
            
            // Get n16 and move PC to next instruction
            uint8_t lsb = mem.read(PC++);
            uint8_t msb = mem.read(PC++);
            uint16_t n16 = ((uint16_t)msb << 8) | (uint16_t)lsb;

            mem.write(n16, AF.hi);

            return 4;
        }

        /** 
         * Load the value in memory[n16] into register A, where n16 is the
         * immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t LD_A_n16() {
            
            // Get n16 and move PC to next instruction
            uint8_t lsb = mem.read(PC++);
            uint8_t msb = mem.read(PC++);
            uint16_t n16 = ((uint16_t)msb << 8) | (uint16_t)lsb;

            AF.hi = mem.read(n16);

            return 4;
        }

        /**
         * Load value in HL into SP 
         */
        uint32_t LD_SP_HL() {
            SP.set_data16(HL.get_data16());
            return 2;
        }

        /**
         * Store value in register A into memory[0xFF00 + C]
         */
        uint32_t LDH_C_A() {
            mem.write(0xFF00 + (uint16_t)BC.lo, AF.hi);
            return 2;
        }

        /**
         * Load value in memory[0xFF00 + C] into register A 
         */
        uint32_t LDH_A_C() {
            AF.hi = mem.read(0xFF00 + (uint16_t)BC.lo);
            return 2;
        }

        /*
         * Store value in register A into memory[0xFF00 + n8] where n8 is
         * the immediate 8-bit value
         * 
         * Assumes PC is pointing to n8 before call
         */
        uint32_t LDH_n8_A() {

            // Get n8 and move PC to next instruction
            uint8_t n8 = mem.read(PC++);

            mem.write(0xFF00 + (uint16_t)n8, AF.hi);
            return 3;
        }


        /**
         * Load value in memory[0xFF00 + n8] into register A where n8 is
         * the immediate 8-bit value
         * 
         * Assumes PC is pointing to n8 before call
         */
        uint32_t LDH_A_n8() {

            // Get n8 and move PC to next instruction
            uint8_t n8 = mem.read(PC++);

            AF.hi = mem.read(0xFF00 + (uint16_t)n8);
            return 3;
        }

        /**
         * Load SP + e8 into HL, where e8 is the immediate signed 8-bit value
         * Reset Z and N flags.  The flags H and C are set accordingly
         * 
         * Assumes PC is point to n8 before call
         */
        uint32_t LD_HL_SP_e8() {
            
            // Get e8 and move PC to next instruction
            int8_t e8 = (int8_t) mem.read(PC++);

            AF.lo &= 0x7F; // Reset Z
            AF.lo &= 0xBF; // Reset N

            uint8_t e8_lo_nib = ((uint8_t)e8) & 0x0F;
            uint8_t SP_lo_nib = SP.lo & 0x0F;

            uint8_t set_H = SP_lo_nib + e8_lo_nib > 0xF;
            uint8_t set_C = ((uint16_t)e8 + (uint16_t)SP.lo) > 0xFF;

            if (set_H)
                AF.lo |= 0x20;
            
            if (set_C) 
                AF.lo |= 0x10;

            // Do signed arithmetic
            HL.set_data16((uint16_t)((int16_t)SP.get_data16() + (int16_t)e8));

            return 3;
        }


        /** 
         * Increment value in register by 1
         */
        uint32_t INC_R16(Register16 &reg) {
            reg++;
            return 2;
        }

        /** 
         * Decrement value in register by 1
         */
        uint32_t DEC_R16(Register16 &reg) {
            reg--;
            return 2;
        }

        /**
         * Add value in register to HL
         *
         * Reset N flag. Set H and C flag appropriately
         */
        uint32_t ADD_HL_R16(Register16 &reg) {
            
            AF.lo &= 0xBF; // Reset N

            uint16_t HL_data16 = HL.get_data16();
            uint16_t reg_data16 = reg.get_data16();

            uint8_t set_H = HL_data16 & 0x0FFF + reg_data16 & 0x0FFF > 0x0FFF;
            uint8_t set_C = (uint32_t)HL_data16 + (uint32_t)reg_data16 > 0xFFFF;

            if (set_H) 
                AF.lo |= 0x20;
            
            if (set_C) 
                AF.lo |= 0x10;
            
            HL += reg;
            
            return 2;
        }

        /**
         * Increment register by 1
         *
         * Reset N flag. Set Z and H flag accordingly
         */
        uint32_t INC_R8(Register8 &reg) {

            AF.lo &= 0xBF; // Reset N
            
            uint8_t set_Z = (reg + 0x01) == 0;
            uint8_t set_H = (reg & 0xF) + 0x1 >= 0xF;

            if (set_Z)
                AF.lo |= 0x80;            

            if (set_H) 
                AF.lo |= 0x20;
            
            reg++;
            return 1;
        }

        /**
         * Increment memory[HL] by 1
         *
         * Reset N flag. Set Z and H flag accordingly
         */
        uint32_t INC_HL() {

            AF.lo &= 0xBF; // Reset N

            uint8_t HLmem = mem.read(HL.get_data16());
            
            uint8_t set_Z = HLmem + 0x01 == 0;
            uint8_t set_H = (HLmem & 0xF) + 0x1 > 0xF;

            if (set_Z)
                AF.lo |= 0x80;            

            if (set_H) 
                AF.lo |= 0x20;
            
            mem.write(HL.get_data16(), HLmem + 0x01);
            return 3;
        }
        
        /**
         * Decrement register by 1
         *
         * Set N flag. Set Z and H flag accordingly
         */
        uint32_t DEC_R8(Register8 &reg) {

            AF.lo != 0x40; // Set N

            uint8_t set_Z = reg - 0x01 == 0;
            uint8_t set_H = ((int8_t)(reg & 0xF)) - 0x1 < 0; 

            if (set_Z)
                AF.lo |= 0x80;            

            if (set_H) 
                AF.lo |= 0x20;

            reg--;
            return 1;
        }

        /**
         * Decrement memory[HL] by 1
         *
         * Set N flag. Set Z and H flag accordingly
         */
        uint32_t DEC_HL() {

            AF.lo != 0x40; // Set N

            uint8_t HLmem = mem.read(HL.get_data16());
            
            uint8_t set_Z = HLmem - 0x01 == 0;
            uint8_t set_H = (int8_t)(HLmem & 0xF) < 0;

            if (set_Z)
                AF.lo |= 0x80;            

            if (set_H) 
                AF.lo |= 0x20;

            mem.write(HL.get_data16(), HLmem - 0x01);
            return 3;
        }

    

    public:
    
        // Constructor
        CPU(): AF(), BC(), DE(), HL(), SP(), mem() 
        {
            PC = 0x0100;
            SP.set_data16(0xFFFE);
        }

        // Destructor
        ~CPU() {}


        /**
         * Fetch, decode, and execute and opcode then 
         * return the number of m-cycles that opcode took
         */
        uint32_t emulate_cycles() {
            uint32_t m_cycles = 0;

            // Fetch
            uint8_t opcode = mem.read(PC);

            PC++; // Point to next byte

            // Decode and execute
            switch (opcode) {
                
                case 0x00: // NOP
                    break;

                // LD R8, R8
                case 0x40:
                    m_cycles += LD_R8_R8(BC.hi, BC.hi);
                    break;
                case 0x41:
                    m_cycles += LD_R8_R8(BC.hi, BC.lo);
                    break;
                case 0x42: 
                    m_cycles += LD_R8_R8(BC.hi, DE.hi);
                    break;
                case 0x43:
                    m_cycles += LD_R8_R8(BC.hi, DE.lo);
                    break;
                case 0x44:
                    m_cycles += LD_R8_R8(BC.hi, HL.hi);
                    break;
                case 0x45:
                    m_cycles += LD_R8_R8(BC.hi, HL.lo);
                    break;
                case 0x47:
                    m_cycles += LD_R8_R8(BC.hi, AF.hi);
                    break;
                case 0x48:
                    m_cycles += LD_R8_R8(BC.lo, BC.hi);
                    break;
                case 0x49:
                    m_cycles += LD_R8_R8(BC.lo, BC.lo);
                    break;
                case 0x4A:
                    m_cycles += LD_R8_R8(BC.lo, DE.hi);
                    break;
                case 0x4B:
                    m_cycles += LD_R8_R8(BC.lo, DE.lo);
                    break;
                case 0x4C:
                    m_cycles += LD_R8_R8(BC.lo, HL.hi);
                    break;
                case 0x4D:
                    m_cycles += LD_R8_R8(BC.lo, HL.lo);
                    break;
                case 0x4F:
                    m_cycles += LD_R8_R8(BC.lo, AF.hi);
                    break;
                case 0x50:
                    m_cycles += LD_R8_R8(DE.hi, BC.hi);
                    break;
                case 0x51:
                    m_cycles += LD_R8_R8(DE.hi, BC.lo);
                    break;
                case 0x52:
                    m_cycles += LD_R8_R8(DE.hi, DE.hi);
                    break;
                case 0x53:
                    m_cycles += LD_R8_R8(DE.hi, DE.lo);
                    break;
                case 0x54:
                    m_cycles += LD_R8_R8(DE.hi, HL.hi);
                    break;
                case 0x55:
                    m_cycles += LD_R8_R8(DE.hi, HL.lo);
                    break;
                case 0x57:
                    m_cycles += LD_R8_R8(DE.hi, AF.hi);
                    break;
                case 0x58:
                    m_cycles += LD_R8_R8(DE.lo, BC.hi);
                    break;
                case 0x59:
                    m_cycles += LD_R8_R8(DE.lo, BC.lo);
                    break;
                case 0x5A:
                    m_cycles += LD_R8_R8(DE.lo, DE.hi);
                    break;
                case 0x5B:
                    m_cycles += LD_R8_R8(DE.lo, DE.lo);
                    break;
                case 0x5C:
                    m_cycles += LD_R8_R8(DE.lo, HL.hi);
                    break;
                case 0x5D:
                    m_cycles += LD_R8_R8(DE.lo, HL.lo);
                    break;
                case 0x5F:
                    m_cycles += LD_R8_R8(DE.lo, AF.hi);
                    break;
                case 0x60:
                    m_cycles += LD_R8_R8(HL.hi, BC.hi);
                    break;
                case 0x61:
                    m_cycles += LD_R8_R8(HL.hi, BC.lo);
                    break;
                case 0x62:
                    m_cycles += LD_R8_R8(HL.hi, DE.hi);
                    break;
                case 0x63:
                    m_cycles += LD_R8_R8(HL.hi, DE.lo);
                    break;
                case 0x64:
                    m_cycles += LD_R8_R8(HL.hi, HL.hi);
                    break;
                case 0x65:
                    m_cycles += LD_R8_R8(HL.hi, HL.lo);
                    break;
                case 0x67:
                    m_cycles += LD_R8_R8(HL.hi, AF.hi);
                    break;
                case 0x68:
                    m_cycles += LD_R8_R8(HL.lo, BC.hi);
                    break;
                case 0x69:
                    m_cycles += LD_R8_R8(HL.lo, BC.lo);
                    break;
                case 0x6A:
                    m_cycles += LD_R8_R8(HL.lo, DE.hi);
                    break;
                case 0x6B:
                    m_cycles += LD_R8_R8(HL.lo, DE.lo);
                    break;
                case 0x6C:
                    m_cycles += LD_R8_R8(HL.lo, HL.hi);
                    break;
                case 0x6D:
                    m_cycles += LD_R8_R8(HL.lo, HL.lo);
                    break;
                case 0x6F:
                    m_cycles += LD_R8_R8(HL.lo, AF.hi);
                    break;
                case 0x78:
                    m_cycles += LD_R8_R8(AF.hi, BC.hi);
                    break;
                case 0x79:
                    m_cycles += LD_R8_R8(AF.hi, BC.lo);
                    break;
                case 0x7A:
                    m_cycles += LD_R8_R8(AF.hi, DE.hi);
                    break;
                case 0x7B:
                    m_cycles += LD_R8_R8(AF.hi, DE.lo);
                    break;
                case 0x7C:
                    m_cycles += LD_R8_R8(AF.hi, HL.hi);
                    break;
                case 0x7D:
                    m_cycles += LD_R8_R8(AF.hi, HL.lo);
                    break;
                case 0x7F:
                    m_cycles += LD_R8_R8(AF.hi, AF.hi);
                    break;

                // LD [HL], R8
                case 0x70:
                    m_cycles += LD_HL_R8(BC.hi);
                    break;
                case 0x71:
                    m_cycles += LD_HL_R8(BC.lo);
                    break;
                case 0x72:
                    m_cycles += LD_HL_R8(DE.hi);
                    break;
                case 0x73:
                    m_cycles += LD_HL_R8(DE.lo);
                    break;
                case 0x74:
                    m_cycles += LD_HL_R8(HL.hi);
                    break;
                case 0x75:
                    m_cycles += LD_HL_R8(HL.lo);
                    break;
                case 0x77:
                    m_cycles += LD_HL_R8(AF.hi);
                    break;

                // LD R8, [HL]
                case 0x46:
                    m_cycles += LD_R8_HL(BC.hi);
                    break;
                case 0x4E:
                    m_cycles += LD_R8_HL(BC.lo);
                    break;
                case 0x56:
                    m_cycles += LD_R8_HL(DE.hi);
                    break;
                case 0x5E:
                    m_cycles += LD_R8_HL(DE.lo);
                    break;
                case 0x66:
                    m_cycles += LD_R8_HL(HL.hi);
                    break;
                case 0x6E:
                    m_cycles += LD_R8_HL(HL.lo);
                    break;
                case 0x7E:
                    m_cycles += LD_R8_HL(AF.hi);
                    break;

                case 0x76: // TODO: HALT
                    break;

                // LD R8, n8
                case 0x06:
                    m_cycles += LD_R8_n8(BC.hi);
                    break;
                case 0x0E:
                    m_cycles += LD_R8_n8(BC.lo);
                    break;
                case 0x16:
                    m_cycles += LD_R8_n8(DE.hi);
                    break;
                case 0x1E:
                    m_cycles += LD_R8_n8(DE.lo);
                    break;
                case 0x26:
                    m_cycles += LD_R8_n8(HL.hi);
                    break;
                case 0x2E:
                    m_cycles += LD_R8_n8(HL.lo);
                    break;
                case 0x3E:
                    m_cycles += LD_R8_n8(BC.hi);
                    break;

                case 0x36: // LD [HL], n8
                    m_cycles += LD_HL_n8();
                    break;

                // LD [R16], A
                case 0x02:
                    m_cycles += LD_R16_A(BC);
                    break;
                case 0x12:
                    m_cycles += LD_R16_A(DE);
                    break;
                case 0x22:
                    m_cycles += LD_R16_A(HL++);
                    break;
                case 0x32:
                    m_cycles += LD_R16_A(HL--);
                    break;

                // LD A, [R16]
                case 0x0A:
                    m_cycles += LD_A_R16(BC);
                    break;
                case 0x1A:
                    m_cycles += LD_A_R16(DE);
                    break;
                case 0x2A:
                    m_cycles += LD_A_R16(HL++);
                    break;
                case 0x3A:
                    m_cycles += LD_A_R16(HL--);
                    break;

                // LD R16, n16
                case 0x01:
                    m_cycles += LD_R16_n16(BC);
                    break;
                case 0x11:
                    m_cycles += LD_R16_n16(DE);
                    break;
                case 0x21:
                    m_cycles += LD_R16_n16(HL);
                    break;
                case 0x31:
                    m_cycles += LD_R16_n16(SP);
                    break;

                case 0x08: // LD [n16], SP
                    m_cycles += LD_n16_SP();
                    break;

                case 0xEA: // LD [n16], A
                    m_cycles += LD_n16_A();
                    break;

                case 0xFA: // LD A, [n16]
                    m_cycles += LD_A_n16();
                    break;

                case 0xF9: // LD SP, HL
                    m_cycles += LD_SP_HL();
                    break;

                case 0xE2: // LDH [C], A
                    m_cycles += LDH_C_A();
                    break;

                case 0xF2: // LDH A, [C]
                    m_cycles += LDH_A_C();
                    break;

                case 0xE0: // LDH [n8], A
                    m_cycles += LDH_n8_A();
                    break;

                case 0xF0: // LDH A, [n8]
                    m_cycles += LDH_A_n8();
                    break;

                case 0xF8: // LD HL, SP + n8
                    m_cycles += LD_HL_SP_e8();
                    break;

                // INC R16
                case 0x03:
                    m_cycles += INC_R16(BC);
                    break;
                case 0x13:
                    m_cycles += INC_R16(DE);
                    break;
                case 0x23:
                    m_cycles += INC_R16(HL);
                    break;
                case 0x33:
                    m_cycles += INC_R16(SP);
                    break;

                // DEC R16
                case 0x0B:
                    m_cycles += DEC_R16(BC);
                    break;
                case 0x1B:
                    m_cycles += DEC_R16(DE);
                    break;
                case 0x2B:
                    m_cycles += DEC_R16(HL);
                    break;
                case 0x3B:
                    m_cycles += DEC_R16(SP);
                    break;

                // ADD HL, R16
                case 0x09:
                    m_cycles += ADD_HL_R16(BC);
                    break;
                case 0x19:
                    m_cycles += ADD_HL_R16(DE);
                    break;
                case 0x29:
                    m_cycles += ADD_HL_R16(HL);
                    break;
                case 0x39:
                    m_cycles += ADD_HL_R16(SP);
                    break;

                // INC R8
                case 0x04:
                    m_cycles += INC_R8(BC.hi);
                    break;
                case 0x0C:
                    m_cycles += INC_R8(BC.lo);
                    break;
                case 0x14:
                    m_cycles += INC_R8(DE.hi);
                    break;
                case 0x1C:
                    m_cycles += INC_R8(DE.lo);
                    break;
                case 0x24:
                    m_cycles += INC_R8(HL.hi);
                    break;
                case 0x2C:
                    m_cycles += INC_R8(HL.lo);
                    break;
                case 0x3C:
                    m_cycles += INC_R8(AF.hi);
                    break;
                
                // INC [HL]
                case 0x34:
                    m_cycles += INC_HL();
                    break;

                // DEC R8
                case 0x05:
                    m_cycles += DEC_R8(BC.hi);
                    break;
                case 0x0D:
                    m_cycles += DEC_R8(BC.lo);
                    break;
                case 0x15:
                    m_cycles += DEC_R8(DE.hi);
                    break;
                case 0x1D:
                    m_cycles += DEC_R8(DE.lo);
                    break;
                case 0x25:
                    m_cycles += DEC_R8(HL.hi);
                    break;
                case 0x2D:
                    m_cycles += DEC_R8(HL.lo);
                    break;
                case 0x3D:
                    m_cycles += DEC_R8(AF.hi);
                    break;
                        
                // DEC [HL]
                case 0x35:
                    m_cycles += DEC_HL();
                    break;
                
                
            }

            return m_cycles;
        
        }

};


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("gb-emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 4, 144 * 4, 0);

    CPU cpu;
    
    // Main loop
    int running = 1;
    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case (SDL_KEYDOWN):
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) 
                        running = 0;
                    break;
            }
        }
    }

    return 0;
}   