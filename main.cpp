#include <iostream>
#include <cstdint>
#include "SDL.h"



class Register {
    private:
        uint8_t hi;
        uint8_t lo;

    public:
        // Constructor
        Register() {
            hi = 0x0000;
            lo = 0x0000;
        }

        // Destructor
        ~Register() {
            
        }

        void set_hi(uint8_t val) {
            hi = val;
        }

        void set_lo(uint8_t val) {
            lo = val;
        }

};


class CPU {
    private:

        // Registers
        Register AF;    // Accumulator and flags
        Register BC;    // General registers
        Register DE;
        Register HL;

        uint16_t PC;    // Program counter
        uint16_t SP;    // Stack pointer        


        // Memory
        uint8_t mem[0x10000];

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

            // Decode

            // Execute
        
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