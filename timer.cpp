#include "timer.h"

Timer::Timer() {}
Timer::~Timer() {}

bool Timer::tick() {
    // https://hacktix.github.io/GBEDG/timers/
    
    // Get bit position from DIV determined by TAC
    u16 bit_pos = 0;
    u8 clock_select = TAC & 0xb11;
    switch (clock_select) {
        case 0b00: bit_pos = (1 << 9); break;
        case 0b01: bit_pos = (1 << 3); break;
        case 0b10: bit_pos = (1 << 5); break;
        case 0b11: bit_pos = (1 << 7); break;
    }

    u8 timer_enabled = TAC & 0b100; 
    u16 prev_DIV = DIV;

    DIV++; // DIV is ticked every time (every "T-cycle")

    // Detect a "falling edge": if the bit goes from 1 to 0
    bool timer_update = (prev_DIV & bit_pos) && !(DIV & bit_pos);

    if (timer_enabled && timer_update) {
        TIMA++;

        if (TIMA == 0xFF) { 
            
            // Reset TIMA if it overflows
            TIMA = TMA;

            return true; // Request a timer interrupt
        }
    }

    return false; // No interrupt requested
}

u8 Timer::read(u16 addr) {
    switch (addr) {
        case 0xFF04:
            return DIV >> 8; // only the upper byte of DIV is seen
            break;
        case 0xFF05:
            return TIMA;
            break;
        case 0xFF06:
            return TMA;
            break;
        case 0xFF07:
            return TAC;
            break;
    }

    return 0;
}

void Timer::write(u16 addr, u8 val) {
    switch (addr) {
        case 0xFF04:
            DIV = 0; // any write to DIV resets it
            break;
        case 0xFF05:
            TIMA = val;
            break;
        case 0xFF06:
            TMA = val;
            break;
        case 0xFF07:
            TAC = val;
            break;
    }
}