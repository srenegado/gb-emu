#ifndef IO_H
#define IO_H

#include "common.h"
#include "timer.h"

class IO {
    private:
        char serial_data[2];
        u8 IE = 0x00;   // 0xFFFF: Interrupt enable register
        u8 IF = 0xE1;   // 0xFF0F: Interrupt flag register
        u8 LCDC = 0x91; // 0xFF40: LCD control
        u8 STAT = 0x85; // 0xFF41: LCD status
        u8 LY = 0x00;   // 0xFF44: LCD Y coordinate
        u8 LYC = 0x00;  // 0xFF45: LY compare
        Timer timer;
    public:
        IO();
        ~IO();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
        u8 get_IF();
        void set_IF(u8 val);
        u8 get_IE();
        void set_IE(u8 val);
        u8 get_LCDC();
        void set_LCDC(u8 val);
        bool timer_tick();
};

#endif