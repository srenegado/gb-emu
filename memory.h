#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"
#include "cart.h"

class IO {
    private:
        char serial_data[2];
        u8 IF = 0xE1; // Interrupt flag register
    public:
        IO();
        ~IO();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
        u8 get_IF();
        void set_IF(u8 val);
};

class RAM {
    private:
        u8 wram[0x2000]; // Work RAM
        u8 hram[0x80];   // High RAM
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
        IO io;
        RAM ram;
        u8 IE = 0x00; // Interrupt enable register
    public:
        MemoryBus(Cartridge &cart_);
        ~MemoryBus();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
        u8 get_IF();
        void set_IF(u8 val);
        u8 get_IE();
};

#endif