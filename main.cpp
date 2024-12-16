#include <iostream>
#include <stdexcept>
#include "SDL.h"
#include "cpu.h"
#include "memory.h"
#include "graphics.h"

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);

    CPU cpu;
    PPU ppu;
    Memory mem;

    // Load game into ROM
    try {
        char *ROM = argv[1];
        mem.load_ROM(ROM);
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    
    // Main loop
    int running = 1;
    unsigned int m_cycles = 0;
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

        // m_cycles += cpu.emulate_cycles()

        // System counter is incremented every M-cycle
        for (int i = 0; i < m_cycles; i++ ) 
            cpu.inc_DIV(mem); 

        // CPU timer is incremented every timer_clock_speed M-cycles
        if (cpu.is_timer_started(mem)) {
            int timer_clock_speed = cpu.get_timer_clock_speed(mem);
            int timer_ticks = m_cycles / timer_clock_speed;
            for (int i = 0; i < timer_ticks; i++)
                cpu.inc_TIMA(mem);
        }
    
    }

    SDL_Quit();
    return 0;
}   