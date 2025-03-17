#ifndef IO_H
#define IO_H

#include "common.h"
#include "timer.h"
#include "joypad.h"

class IO {
    private:
        char serial_data[2];
        u8 IE = 0x00;   // 0xFFFF: Interrupt enable register
        u8 IF = 0xE1;   // 0xFF0F: Interrupt flag register
        u8 LCDC = 0x91; // 0xFF40: LCD control
        u8 STAT = 0x85; // 0xFF41: LCD status
        u8 SCY = 0x00;  // 0xFF42: Background viewport Y pos.
        u8 SCX = 0x00;  // 0xFF43: Background viewport X pos.
        u8 LY = 0x00;   // 0xFF44: LCD Y coordinate
        u8 LYC = 0x00;  // 0xFF45: LY compare
        u8 BGP = 0xFC;  // 0xFF47: Background and window palette
        u8 OBP0 = 0x00; // 0xFF48: Sprite palette 0
        u8 OBP1 = 0x00; // 0xFF49: Sprite palette 1
        u8 WY = 0x00;   // 0xFF4A: Window Y position
        u8 WX = 0x00;   // 0xFF4B: Window X position plus 7
        Timer timer;
        Joypad &joypad;
    public:
        IO(Joypad &joypad_);
        ~IO();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
        u8 get_IF();
        void set_IF(u8 val);
        u8 get_IE();
        void set_IE(u8 val);
        u8 get_LCDC();
        void set_LCDC(u8 val);
        u8 get_STAT();
        void set_STAT(u8 val);
        u8 get_SCY();
        void set_SCY(u8 val);
        u8 get_SCX();
        void set_SCX(u8 val);
        u8 get_LY();
        void set_LY(u8 val);
        u8 get_LYC();
        void set_LYC(u8 val);
        u8 get_BGP();
        void set_BGP(u8 val);
        u8 get_OBP0();
        void set_OBP0(u8 val);
        u8 get_OBP1();
        void set_OBP1(u8 val);
        u8 get_WY();
        void set_WY(u8 val);
        u8 get_WX();
        void set_WX(u8 val);
        bool timer_tick();
};

#endif