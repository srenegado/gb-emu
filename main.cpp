#include <iostream>
#include <cstdint>
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
    ~Register16() {
        
    }
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


        // Memory
        uint8_t mem[0x10000];

        // Opcodes
        void LD_R8_R8(Register8 dest, Register8 src) {
            dest = src;
        }

    public:
    
        // Constructor
        CPU(): AF(), BC(), DE(), HL()  
        {
            PC = 0x0000;
            SP = 0x0000;
        }

        // Destructor
        ~CPU() {

        }

    
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
                    break;
                case 0x42:
                    break;
                case 0x44:
                    break;
                case 0x45:
                    break;
                case 0x47:
                    break;
                case 0x48:
                    break;
                case 0x49:
                    break;
                case 0x4A:
                    break;
                case 0x4B:
                    break;
                case 0x4C:
                    break;
                case 0x4D:
                    break;
                case 0x4F:
                    break;
                case 0x50:
                    break;
                case 0x51:
                    break;
                case 0x52:
                    break;
                case 0x54:
                    break;
                case 0x55:
                    break;
                case 0x57:
                    break;
                case 0x58:
                    break;
                case 0x59:
                    break;
                case 0x5A:
                    break;
                case 0x5B:
                    break;
                case 0x5C:
                    break;
                case 0x5D:
                    break;
                case 0x5F:
                    break;
                case 0x60:
                    break;
                case 0x61:
                    break;
                case 0x62:
                    break;
                case 0x64:
                    break;
                case 0x65:
                    break;
                case 0x67:
                    break;
                case 0x68:
                    break;
                case 0x69:
                    break;
                case 0x6A:
                    break;
                case 0x6B:
                    break;
                case 0x6C:
                    break;
                case 0x6D:
                    break;
                case 0x6F:
                    break;
                case 0x78:
                    break;
                case 0x79:
                    break;
                case 0x7A:
                    break;
                case 0x7B:
                    break;
                case 0x7C:
                    break;
                case 0x7D:
                    break;
                case 0x7F:
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