#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"
#include "cart.h"
#include "ppu.h"
#include "timer.h"

class IO {
    private:
        char serial_data[2];
        u8 IF = 0xE1; // Interrupt flag register
        Timer timer;
    public:
        IO();
        ~IO();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
        u8 get_IF();
        void set_IF(u8 val);
        bool timer_tick();
};

class RAM {
    private:
        u8 wram[0x2000]; // Work RAM: 0xC000 - 0xDFFF
        u8 hram[0x80];   // High RAM: 0xFF80 - 0xFFFE
    public:
        RAM();
        ~RAM();
        u8 wram_read(u16 addr);
        void wram_write(u16 addr, u8 val);
        u8 hram_read(u16 addr);
        void hram_write(u16 addr, u8 val);
};

class MemoryBus {
    private:
        Cartridge &cart;
        PPU &ppu;
        IO io;
        RAM ram;
        u8 IE = 0x00; // Interrupt enable register
    public:
        MemoryBus(Cartridge &cart_, PPU &ppu_);
        ~MemoryBus();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
        u8 get_IF();
        void set_IF(u8 val);
        u8 get_IE();

        void emulate_cycles(int cpu_cycles); // For cycle timing
};

#endif