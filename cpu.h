#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <stdexcept>
#include <iomanip>
#include "memory.h"

enum InterruptType {
    /*   In the interrupt registers (IE and IF), 
         the following bits are mapped to each type:  */
    Joypad, // 4
    Serial, // 3
    Timer,  // 2    
    LCD,    // 1
    VBlank  // 0
};


typedef uint8_t Register8;

struct Register16 {
    Register8 hi;
    Register8 lo;

    // Constructor
    Register16();

    // Destructor
    ~Register16();

    /**
     * Combine values in hi and lo then 
     * return the resulting 2-byte value 
     */
    uint16_t get_data16() const;

    /** 
     * Store a 16-bit value into the register
     */
    void set_data16(uint16_t data);

    // Addition assignment
    Register16& operator+=(Register16 reg);

    // Postfix increment
    Register16 operator++(int);

    // Postfix decrement
    Register16 operator--(int);
};


class CPU {
    private:

        // Registers
        Register16 AF;    // Accumulator and flags
        Register16 BC;    // General registers
        Register16 DE;
        Register16 HL;
        Register16 SP;    // Stack pointer

        uint16_t DIV;     // Divider: system counter

        uint16_t PC;      // Program counter  

        uint8_t IME;      // Interrupt master enable flag
        uint8_t IME_next;

        /** 
         * Opcodes
         * 
         * Each opcode returns the number of M-cycles they use
         */ 
    
        /**
         * Load value in src register into dest resigter
         */
        uint32_t LD_R8_R8(Register8 &dest, Register8 src);

        /**
         * Store value in src register into memory[HL]
         */
        uint32_t LD_HL_R8(Register8 src, Memory &mem);

        /**
         * Load value in memory[HL] into dest register
         */
        uint32_t LD_R8_HL(Register8 &dest, Memory &mem);

        /**
         * Load immediate 8-bit value n8 into dest register
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t LD_R8_n8(Register8 &dest, Memory &mem);

        /**
         * Store immediate 8-bit value n8 into memory[HL]
         *
         * Assumes PC is pointing to n8 before call 
         */
        uint32_t LD_HL_n8(Memory &mem);

        /** 
         * Store value in register A into memory[dest]
         */
        uint32_t LD_R16_A(const Register16 &dest, Memory &mem);

        /**
         * Load value in memory[dest] into register A 
         */
        uint32_t LD_A_R16(const Register16 &dest, Memory &mem);

        /**
         * Load the immediate little-endian 16-bit value n16 into dest register
         *
         * Assumes PC is pointing to n16 before call 
         */
        uint32_t LD_R16_n16(Register16 &dest, Memory &mem);

        /** 
         * Store the value in SP into memory[n16], where n16 is the
         * immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t LD_n16_SP(Memory &mem);
        /** 
         * Store the value in register A into memory[n16], where n16 is the
         * immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t LD_n16_A(Memory &mem);

        /** 
         * Load the value in memory[n16] into register A, where n16 is the
         * immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t LD_A_n16(Memory &mem);

        /**
         * Load value in HL into SP 
         */
        uint32_t LD_SP_HL();
        /**
         * Store value in register A into memory[0xFF00 + C]
         */
        uint32_t LDH_C_A(Memory &mem);

        /**
         * Load value in memory[0xFF00 + C] into register A 
         */
        uint32_t LDH_A_C(Memory &mem);

        /*
         * Store value in register A into memory[0xFF00 + n8] where n8 is
         * the immediate 8-bit value
         * 
         * Assumes PC is pointing to n8 before call
         */
        uint32_t LDH_n8_A(Memory &mem);

        /**
         * Load value in memory[0xFF00 + n8] into register A where n8 is
         * the immediate 8-bit value
         * 
         * Assumes PC is pointing to n8 before call
         */
        uint32_t LDH_A_n8(Memory &mem);

        /**
         * Load SP + e8 into HL, where e8 is the immediate signed 8-bit value
         * Reset Z and N flags.  The flags H and C are set accordingly
         * 
         * Assumes PC is point to n8 before call
         */
        uint32_t LD_HL_SP_e8(Memory &mem);

        /** 
         * Increment value in register by 1
         */
        uint32_t INC_R16(Register16 &reg);

        /** 
         * Decrement value in register by 1
         */
        uint32_t DEC_R16(Register16 &reg);

        /**
         * Add value in register to HL
         *
         * Reset N flag. Set H and C flag appropriately
         */
        uint32_t ADD_HL_R16(Register16 &reg);

        /**
         * Increment register by 1
         *
         * Reset N flag. Set Z and H flag accordingly
         */
        uint32_t INC_R8(Register8 &reg);

        /**
         * Increment memory[HL] by 1
         *
         * Reset N flag. Set Z and H flag accordingly
         */
        uint32_t INC_HL(Memory &mem);
        /**
         * Decrement register by 1
         *
         * Set N flag. Set Z and H flag accordingly
         */
        uint32_t DEC_R8(Register8 &reg);

        /**
         * Decrement memory[HL] by 1
         *
         * Set N flag. Set Z and H flag accordingly
         */
        uint32_t DEC_HL(Memory &mem);

        /** 
         * Add value in register to A
         *
         * Reset N flag. Set Z, H, and C flags accordingly
         */
        uint32_t ADD_A_R8(Register8 reg);

        /** 
         * Add value in memory[HL] to A
         *
         * Reset N flag. Set Z, H, and C flags accordingly
         */
        uint32_t ADD_A_HL(Memory &mem);

        /** 
         * Add value in register plus the carry flag to A
         *
         * Reset N flag. Set Z, H, and C flags accordingly
         */
        uint32_t ADC_A_R8(Register8 reg);

        /** 
         * Add value in memory[HL] plus the carry flag to A
         *
         * Reset N flag. Set Z, H, and C flags accordingly
         */
        uint32_t ADC_A_HL(Memory &mem);

        /** 
         * Subtract value in register from A
         *
         * Set N flag. Set Z, H, and C flags accordingly
         */
        uint32_t SUB_A_R8(Register8 reg);

        /** 
         * Subtract value in memory[HL] from A
         *
         * Set N flag. Set Z, H, and C flags accordingly
         */
        uint32_t SUB_A_HL(Memory &mem);

        /** 
         * Subtract value in register and carry flag from A
         *
         * Set N flag. Set Z, H, and C flags accordingly
         */
        uint32_t SBC_A_R8(Register8 reg);

        /** 
         * Subtract value in memory[HL] and carry flag from A
         *
         * Set N flag. Set Z, H, and C flags accordingly
         */
        uint32_t SBC_A_HL(Memory &mem) ;

        /** 
         * Bitwise AND the value in register to A
         *
         * Reset N and C flags. Set H flag. Set Z flag accordingly.
         */
        uint32_t AND_A_R8(Register8 reg);

        /** 
         * Bitwise AND the value in memory[HL] to A
         *
         * Reset N and C flags. Set H flag. Set Z flag accordingly.
         */
        uint32_t AND_A_HL(Memory &mem);

        /** 
         * Bitwise XOR the value in register to A
         *
         * Reset N, H, and C flags. Set Z flag accordingly.
         */
        uint32_t XOR_A_R8(Register8 reg);

        /** 
         * Bitwise XOR the value in memory[HL] to A
         *
         * Reset N, H, and C flags. Set Z flag accordingly.
         */
        uint32_t XOR_A_HL(Memory &mem) ;

        /** 
         * Bitwise OR the value in register to A
         *
         * Reset N, H, and C flags. Set Z flag accordingly.
         */
        uint32_t OR_A_R8(Register8 reg) ;

        /** 
         * Bitwise OR the value in memory[HL] to A
         *
         * Reset N, H, and C flags. Set Z flag accordingly.
         */
        uint32_t OR_A_HL(Memory &mem);

        /** 
         * Subtract value in register to A, but don't store the result
         *
         * Set N flag. Set Z, H, and C flags accordingly
         */
        uint32_t CP_A_R8(Register8 reg) ;

        /** 
         * Subtract value in memory[HL] from A, but don't store the result
         *
         * Set N flag. Set Z, H, and C flags accordingly
         */
        uint32_t CP_A_HL(Memory &mem) ;

        /** 
         * Add value n8 to A, where n8 is the immediate 8-bit value
         *
         * Reset N flag. Set Z, H, and C flags accordingly
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t ADD_A_n8(Memory &mem);

        /** 
         * Add value n8 plus the carry flag to A, where n8 is the immediate 8-bit value
         *
         * Reset N flag. Set Z, H, and C flags accordingly
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t ADC_A_n8(Memory &mem) ;

        /** 
         * Subtract value n8 to A, where n8 is the immediate 8-bit value
         *
         * Set N flag. Set Z, H, and C flags accordingly
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t SUB_A_n8(Memory &mem) ;

        /** 
         * Subtract value n8 and carry flag from A, where n8 is the immediate 8-bit value
         *
         * Set N flag. Set Z, H, and C flags accordingly
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t SBC_A_n8(Memory &mem) ;

        /** 
         * Bitwise AND the value n8 to A, where n8 is the immediate 8-bit value
         *
         * Reset N and C flags. Set H flag. Set Z flag accordingly.
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t AND_A_n8(Memory &mem) ;

        /** 
         * Bitwise XOR the value n8 to A, where n8 is the immediate 8-bit value
         *
         * Reset N, H, and C flags. Set Z flag accordingly.
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t XOR_A_n8(Memory &mem);

        /** 
         * Bitwise OR the value n8 to A,  where n8 is the immediate 8-bit value
         *
         * Reset N, H, and C flags. Set Z flag accordingly.
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t OR_A_n8(Memory &mem) ;

        /** 
         * Subtract value n8 to A, but don't store the result, 
         * where n8 is the immediate 8-bit value
         *
         * Set N flag. Set Z, H, and C flags accordingly
         *
         * Assumes PC is pointing to n8 before call
         */
        uint32_t CP_A_n8(Memory &mem) ;

        /** 
         * Push address of next instruction onto the stack, then jump to address n16,
         * where n16 is the immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t CALL_n16(Memory &mem) ;

        /** 
         * Push address n16 onto the stack if condition cc is met 
         * where n16 is the immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t CALL_cc_n16(uint8_t cc, Memory &mem);

        /** 
         * Load PC with address in HL
         */
        uint32_t JP_HL() ;

        /** 
         * Load PC with value n16, where n16 is the 
         * immediate little-endian 16-bit value
         *
         * Assumes PC is pointing to n16 before call
         */
        uint32_t JP_n16(Memory &mem) ;

        /** 
         * Add value e8 to PC where e8 is the immediate
         * signed 8-bit value
         *
         * Assumes PC is pointing to e8 before call
         */
        uint32_t JR_e8(Memory &mem) ;

        /** 
         * Add value e8 to PC if condition cc is met, 
         * where e8 is the immediate signed 8-bit value
         *
         * Assumes PC is pointing to e8 before call
         */
        uint32_t JR_cc_e8(uint8_t cc, Memory &mem) ;

        /** 
         * Return from subroutine.
         */
        uint32_t RET(Memory &mem) ;

        /** 
         * Return from subroutine and enable interupts
         */
        uint32_t RETI(Memory &mem);

        /** 
         * Return from subroutine if condition c is met
         */
        uint32_t RET_cc(uint8_t cc, Memory &mem) ;

        /** 
         * Call address vec
         */
        uint32_t RST_vec(uint16_t vec, Memory &mem) ;

        /**
         * Add immediate 8-bit signed value e8 to SP
         *
         * Reset Z and N flag. Sets H and C flags accordingly
         *
         * Assumes PC is pointing to e8 before call
         */
        uint32_t ADD_SP_e8(Memory &mem) ;

        /** 
         * Load register with address pointed by the stack pointer
         * Loading register AF modifies all flags Z, N, H, and C by the popped low byte
         */
        uint32_t POP_R16(Register16 &reg, Memory &mem) ;

        /** 
         * Store stack with address in register
         */
        uint32_t PUSH_R16(Register16 &reg, Memory &mem);

        /** 
         * Flip the carry flag and reset N and H flags
         */
        uint32_t CCF() ;

        /** 
         * Flip the A register and sets N and H flags
         */
        uint32_t CPL() ;

        /** 
         * Disable interrupts
         */
        uint32_t DI() ;

        /** 
         * Enable interrupts after the instruction following EI
         */
        uint32_t EI() ;

        /** 
         * Set the carry flag and reset the N and H flags
         */
        uint32_t SCF() ;

        /** 
         * Rotate register left
         *
         * Load C flag with register's most significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t RLC_R8(Register8 &r8) ;

        /** 
         * Rotate memory[HL] left
         *
         * Load C flag with memory[HL]'s most significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t RLC_HL(Memory &mem) ;

        /** 
         * Rotate register left through the carry flag
         *
         * Load C flag with register's most significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t RL_R8(Register8 &r8) ;

        /** 
        * Rotate register left through the carry flag
        *
        * Load C flag with most significant bit (before shift)
        * 
        * Reset N and H flags. Set Z accordingly
        */
        uint32_t RL_HL(Memory &mem) ;

        /** 
         * Rotate register right
         *
         * Load C flag with register's least significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t RRC_R8(Register8 &r8) ;

        /** 
         * Rotate memory[HL] right
         *
         * Load C flag with least significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t RRC_HL(Memory &mem) ;

        /** 
         * Rotate register right through the carry flag
         *
         * Load C flag with register's least significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t RR_R8(Register8 &r8) ;

        /** 
        * Rotate memory[HL] through the carry flag
        *
        * Load C flag with most significant bit (before shift)
        * 
        * Reset N and H flags. Set Z accordingly
        */
        uint32_t RR_HL(Memory &mem) ;

        /** 
         * Shift register arithmetically left (pad with a 0)
         *
         * Load C flag with most significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t SLA_R8(Register8 &r8) ;

         /** 
         * Shift memory [HL] arithmetically left
         *
         * Load C flag with most significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t SLA_HL(Memory &mem) ;

        /** 
         * Shift register arithmetically right (most significant bit is unchanged)
         *
         * Load C flag with least significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t SRA_R8(Register8 &r8) ;

        /** 
         * Shift memory[HL] arithmetically right (most significant bit is unchanged)
         *
         * Load C flag with least significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t SRA_HL(Memory &mem) ;

        /** 
         * Shift register arithmetically right (pad with a 0)
         *
         * Load C flag with least significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t SRL_R8(Register8 &r8) ;

        /** 
         * Shift memory[HL] logically right (pad with a 0)
         *
         * Load C flag with least significant bit (before shift)
         * 
         * Reset N and H flags. Set Z accordingly
         */
        uint32_t SRL_HL(Memory &mem) ;

        /** 
         * Swap upper 4 bits in register with lowers 4 bits
         *
         * Reset N, H, and C flags. Set Z flag accordingly
         */
        uint32_t SWAP_R8(Register8 &r8) ;

        /** 
         * Swap upper 4 bits in memory[HL] with lowers 4 bits
         *
         * Reset N, H, and C flags. Set Z flag accordingly
         */
        uint32_t SWAP_HL(Memory &mem) ;

        /** 
         * Set Z flag if register bit at index b3 is not set. Reset N flag. Set H flag
         */
        uint32_t BIT_b3_R8(uint8_t b3, const Register8 r8) ;

        /** 
         * Set Z flag if bit at index b3 is not set. Reset N flag. Set H flag
         */
        uint32_t BIT_b3_HL(uint8_t b3, Memory &mem);

        /** 
         * Reset register bit at position b3
         */
        uint32_t RES_b3_R8(uint8_t b3, Register8 &r8) ;

        /** 
         * Reset memory[HL] bit at position b3
         */
        uint32_t RES_b3_HL(uint8_t b3, Memory &mem) ;

        /** 
         * Set register bit at position b3
         */
        uint32_t SET_b3_R8(uint8_t b3, Register8 &r8) ;

        /** 
         * Set memory[HL] bit at position b3
         */
        uint32_t SET_b3_HL(uint8_t b3, Memory &mem) ;

        /** 
         * Sets bit in Interrupt flag (IF) based on requested interrupt
         */
        void request_interrupt(Memory &mem, InterruptType type);

        /** 
         * Give control to interrupt handler
         */
        uint32_t service_interrupt(Memory &mem, InterruptType type); 

    public:
    
        // Constructor
        CPU();

        // Destructor
        ~CPU();

         /** 
         * Timer and divider methods
         *
         * I/O Registers:
         * FF04 - DIV Divider (system counter)
         * FF05 - TIMA Timer counter
         * FF06 - TMA Timer modulo
         * FF07 - TAC Timer control
         */
        
        void inc_DIV(Memory &mem) ;

        void inc_TIMA(Memory &mem) ;

        uint32_t get_timer_clock_speed(Memory &mem) ;

        uint8_t is_timer_started(Memory &mem) ;


        /**
         * Fetch, decode, and execute an instruction then 
         * return the number of M-cycles that it took
         */
        uint32_t emulate_cycles(Memory &mem);

};

#endif