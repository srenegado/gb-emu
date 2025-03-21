#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include "common.h"
#include "cpu_util.h"
#include "memory.h"

// Addressing modes that help out with parsing in some instructions
// note: not used by all instructions
typedef enum {
    DEFAULT,
    LDI,
    LDD,
    LDH_A8,
    LDH_C,
    POP_AF,
    RET_CC,
    RLC,
    RRC,
    RL,
    RR,
    SLA,
    SRA,
    SWAP,
    SRL,
    BIT,
    RES,
    SET
} addr_mode;

class InstructionSet {
    private:
        Registers &regs;
        CpuContext &ctx;
        MemoryBus &bus;

        // Helper functions for getting bytes in opcodes
        u8 get_n8();
        u16 get_n16();
    public:
        InstructionSet(Registers &regs_, CpuContext &ctx_, MemoryBus &bus_);
        ~InstructionSet();

        /** 
         * CPU Instruction Set
         */

        // Misc.
        void nop();
        void stop();
        void daa();

        // Load instructions
        void ld(u8 &reg1, u8 reg2);             // LD r8,r8
        void ld(u8 &reg);                       // LD r8,n8
        void ld16(u8 &hi_reg, u8 &lo_reg);      // LD r16,n16
        void ld16(u16 &SP);                     // LD SP,n16
        void ld_to_HL(u8 reg);                  // LD [HL],r8
        void ld_to_HL();                        // LD [HL],n8
        void ld_from_HL(u8 &reg);               // LD r8,[HL]
        void ld_to_A(u8 hi_reg, u8 lo_reg, 
            addr_mode mode = DEFAULT);          // LD A,[r16]
        void ld_to_A();                         // LD A,[n16]    
        void ld_from_A(u8 hi_reg, u8 lo_reg, 
            addr_mode mode = DEFAULT);          // LD [r16],A
        void ld_from_A();                       // LD [n16],A
        void ld_from_SP();                      // LD [n16],SP
        void ldh_to_A(addr_mode mode);          // LD A,[a8] or LD A,[C]
        void ldh_from_A(addr_mode mode);        // LD [a8],A or LD [C],A
        void ld_SP_signed();                    // LD HL,SP + e8
        void ld_SP_HL();                        // LD SP,HL

        // Jumping and stack-related
        void push(u8 hi_reg, u8 lo_reg);        // PUSH r16
        void pop(u8 &hi_reg, u8 &lo_reg,
            addr_mode mode = DEFAULT);          // POP r16
        void jp(bool cond_code = true);         // JP n16 or JP cc,n16
        void jp_HL();                           // JP HL
        void jr(bool cond_code = true);         // JR e8 or JR cc,e8
        void call(bool cond_code = true);       // CALL n16 or CALL cc,n16
        void ret(bool cond_code = true, 
            addr_mode mode = DEFAULT);          // RET or RET cc
        void reti();                            // RETI
        void rst(u16 addr);                     // RST vec

        // Arithmetic instructions
        void inc(u8 &reg);                      // INC r8
        void inc(u8 &hi_reg, u8 &lo_reg);       // INC r16
        void inc_SP();                          // INC SP
        void inc_HL();                          // INC [HL]
        void dec(u8 &reg);                      // DEC r8
        void dec(u8 &hi_reg, u8 &lo_reg);       // DEC r16
        void dec_SP();                          // DEC SP
        void dec_HL();                          // DEC [HL]
        void add(u8 reg);                       // ADD A,r8
        void add();                             // ADD A,n8
        void add_HL();                          // ADD A,[HL]
        void add16(u8 hi_reg, u8 lo_reg);       // ADD HL,r16
        void add16();                           // ADD HL,SP
        void add_to_SP();                       // ADD SP,e8
        void sub(u8 reg);                       // SUB A,r8
        void sub();                             // SUB A,n8
        void sub_HL();                          // SUB A,[HL]
        void adc(u8 reg);                       // ADC A,r8
        void adc();                             // ADC A,n8
        void adc_HL();                          // ADC A,[HL]
        void sbc(u8 reg);                       // SBC A,r8
        void sbc();                             // SBC A,n8
        void sbc_HL();                          // SBC A,[HL]

        // Bit instructions
        void and_A(u8 reg);                     // AND A,r8                  
        void and_A();                           // AND A,n8
        void and_A_HL();                        // AND A,[HL]
        void or_A(u8 reg);                      // OR A,r8                  
        void or_A();                            // OR A,n8
        void or_A_HL();                         // OR A,[HL]
        void xor_A(u8 reg);                     // XOR A,r8                  
        void xor_A();                           // XOR A,n8
        void xor_A_HL();                        // XOR A,[HL]
        void cp(u8 reg);                        // CP A,r8
        void cp();                              // CP A,n8
        void cp_HL();                           // CP A,[HL]
        void cpl();                             // CPL

        // Carry flag instrs.
        // CCF, SCF
        void ccf();                             
        void scf();

        // Bit shift instrs. (non-0xCB)
        // RLCA, RRCA, RLA, RRA
        void rlca();
        void rrca();
        void rla();
        void rra();
            
        // Interrupt-related
        void di();                              // DI
        void ei();                              // EI
        void halt();                            // HALT

        // Prefixed 0xCB instructions

        // Bit shift instructions: 
        // RLC, RRC, RL, RR, SLA, SRA, SWAP, SRL
        void shift(addr_mode mode, u8 &reg);
        void shift_HL(addr_mode mode);
        
        // Bit flag instructions:
        // BIT, RES, SET
        void bit_flag(addr_mode mode, u8 bit, u8 &reg);
        void bit_flag_HL(addr_mode mode, u8 bit);
};

#endif 