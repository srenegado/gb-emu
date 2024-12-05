#include <iostream>
#include <cstdint>
#include <cstring>
#include "SDL.h"


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
};


class CPU {
    private:

        // Registers
        Register16 AF;    // Accumulator and flags
        Register16 BC;    // General registers
        Register16 DE;
        Register16 HL;

        uint16_t PC;    // Program counter
        uint16_t SP;    // Stack pointer        

        // Memory: 0x0000 - 0xFFFF
        uint8_t mem[0x10000];

        // Opcodes
        void LD_R8_R8(Register8 &dest, Register8 &src) {
            dest = src;
        }

    public:
    
        // Constructor
        CPU(): AF(), BC(), DE(), HL() // Initialize registers 
        {
            PC = 0x0000;
            SP = 0x0000;

            // Initialize memory
            std::memset(mem, 0, sizeof(mem));
        }

        // Destructor
        ~CPU() {}

        void emulate_cycle() {
            // Fetch
            uint8_t opcode = mem[PC];

            // Decode and execute
            switch (opcode) {
                
                case 0x00: // NOP
                    break;

                // LD R8, R8
                case 0x40:
                    LD_R8_R8(BC.hi, BC.hi);
                    break;
                case 0x41:
                    LD_R8_R8(BC.hi, BC.lo);
                    break;
                case 0x42: 
                    LD_R8_R8(BC.hi, DE.hi);
                    break;
                case 0x43:
                    LD_R8_R8(BC.hi, DE.lo);
                    break;
                case 0x44:
                    LD_R8_R8(BC.hi, HL.hi);
                    break;
                case 0x45:
                    LD_R8_R8(BC.hi, HL.lo);
                    break;
                case 0x47:
                    LD_R8_R8(BC.hi, AF.hi);
                    break;
                case 0x48:
                    LD_R8_R8(BC.lo, BC.hi);
                    break;
                case 0x49:
                    LD_R8_R8(BC.lo, BC.lo);
                    break;
                case 0x4A:
                    LD_R8_R8(BC.lo, DE.hi);
                    break;
                case 0x4B:
                    LD_R8_R8(BC.lo, DE.lo);
                    break;
                case 0x4C:
                    LD_R8_R8(BC.lo, HL.hi);
                    break;
                case 0x4D:
                    LD_R8_R8(BC.lo, HL.lo);
                    break;
                case 0x4F:
                    LD_R8_R8(BC.lo, AF.hi);
                    break;
                case 0x50:
                    LD_R8_R8(DE.hi, BC.hi);
                    break;
                case 0x51:
                    LD_R8_R8(DE.hi, BC.lo);
                    break;
                case 0x52:
                    LD_R8_R8(DE.hi, DE.hi);
                    break;
                case 0x53:
                    LD_R8_R8(DE.hi, DE.lo);
                    break;
                case 0x54:
                    LD_R8_R8(DE.hi, HL.hi);
                    break;
                case 0x55:
                    LD_R8_R8(DE.hi, HL.lo);
                    break;
                case 0x57:
                    LD_R8_R8(DE.hi, AF.hi);
                    break;
                case 0x58:
                    LD_R8_R8(DE.lo, BC.hi);
                    break;
                case 0x59:
                    LD_R8_R8(DE.lo, BC.lo);
                    break;
                case 0x5A:
                    LD_R8_R8(DE.lo, DE.hi);
                    break;
                case 0x5B:
                    LD_R8_R8(DE.lo, DE.lo);
                    break;
                case 0x5C:
                    LD_R8_R8(DE.lo, HL.hi);
                    break;
                case 0x5D:
                    LD_R8_R8(DE.lo, HL.lo);
                    break;
                case 0x5F:
                    LD_R8_R8(DE.lo, AF.hi);
                    break;
                case 0x60:
                    LD_R8_R8(HL.hi, BC.hi);
                    break;
                case 0x61:
                    LD_R8_R8(HL.hi, BC.lo);
                    break;
                case 0x62:
                    LD_R8_R8(HL.hi, DE.hi);
                    break;
                case 0x63:
                    LD_R8_R8(HL.hi, DE.lo);
                    break;
                case 0x64:
                    LD_R8_R8(HL.hi, HL.hi);
                    break;
                case 0x65:
                    LD_R8_R8(HL.hi, HL.lo);
                    break;
                case 0x67:
                    LD_R8_R8(HL.hi, AF.hi);
                    break;
                case 0x68:
                    LD_R8_R8(HL.lo, BC.hi);
                    break;
                case 0x69:
                    LD_R8_R8(HL.lo, BC.lo);
                    break;
                case 0x6A:
                    LD_R8_R8(HL.lo, DE.hi);
                    break;
                case 0x6B:
                    LD_R8_R8(HL.lo, DE.lo);
                    break;
                case 0x6C:
                    LD_R8_R8(HL.lo, HL.hi);
                    break;
                case 0x6D:
                    LD_R8_R8(HL.lo, HL.lo);
                    break;
                case 0x6F:
                    LD_R8_R8(HL.lo, AF.hi);
                    break;
                case 0x78:
                    LD_R8_R8(AF.hi, BC.hi);
                    break;
                case 0x79:
                    LD_R8_R8(AF.hi, BC.lo);
                    break;
                case 0x7A:
                    LD_R8_R8(AF.hi, DE.hi);
                    break;
                case 0x7B:
                    LD_R8_R8(AF.hi, DE.lo);
                    break;
                case 0x7C:
                    LD_R8_R8(AF.hi, HL.hi);
                    break;
                case 0x7D:
                    LD_R8_R8(AF.hi, HL.lo);
                    break;
                case 0x7F:
                    LD_R8_R8(AF.hi, AF.hi);
                    break;
            }
        
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