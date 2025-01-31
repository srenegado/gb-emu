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
    
    // Main emulation loop
    int running = 1;
    unsigned int m_cycles = 0;
    unsigned int m_cycles_carry = 0;
    while (running) {

        // Quit if user pressed ESC
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case (SDL_KEYDOWN):
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) 
                        running = 0;
                    break;
            }
        }

        // Emulate and track M-cycles for instruction timing
        try {
            m_cycles = cpu.emulate_cycles(mem);
        } catch (std::invalid_argument &e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
       
        // System counter is incremented every M-cycle
        for (int i = 0; i < m_cycles; i++ ) 
            cpu.inc_DIV(mem);

        // Adjust M-cycles for accuracy
        m_cycles += m_cycles_carry; 

        // CPU timer is incremented every  m_cycles_per_tick  M-cycles
        if (cpu.is_timer_started(mem)) {
            int m_cycles_per_tick = cpu.get_timer_clock_speed(mem);
            int timer_ticks = m_cycles / m_cycles_per_tick;
            for (int i = 0; i < timer_ticks; i++)
                cpu.inc_TIMA(mem);
            
            m_cycles_carry = m_cycles % m_cycles_per_tick;
        }

    }

    SDL_Quit();
    return 0;
}   