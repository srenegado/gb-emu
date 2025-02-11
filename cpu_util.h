#ifndef CPU_UTIL_H
#define CPU_UTIL_H

#include "common.h"
#include "memory.h"

struct Registers {
    u8 A = 0;
    u8 F = 0;
    u8 B = 0;
    u8 C = 0;
    u8 D = 0;
    u8 E = 0;
    u8 H = 0;
    u8 L = 0;
    u16 PC = 0x100;
    u16 SP = 0;
    Registers();
    ~Registers();
}; 

struct CpuContext {
    bool halted = false;
    bool IME = false;
    CpuContext();
    ~CpuContext();
};

typedef enum {
    DEFAULT,
    LDI,
    LDD,
    LDH_A8,
    LDH_C,
    POP_AF,
    RET_CC
} addr_mode;

class InstructionSet {
    private:
        Registers &regs;
        CpuContext &ctx;
        MemoryBus &bus;
    public:
        InstructionSet(Registers &regs_, CpuContext &ctx_, MemoryBus &bus_);
        ~InstructionSet();

        void emulate_cycles(int cpu_cycles);
        u8 get_n8();
        u16 get_n16();

        void nop();

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

};

#endif