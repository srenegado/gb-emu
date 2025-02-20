#ifndef TIMER_H
#define TIMER_H

#include "common.h"

class Timer {
    private:
        // https://gbdev.io/pandocs/Power_Up_Sequence.html#hardware-registers
        u16 DIV = 0xAB00;
        u8 TIMA = 0x00;
        u8 TMA = 0x00;
        u8 TAC = 0xF8;
    public:
        Timer();
        ~Timer();
        bool tick();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
};

#endif