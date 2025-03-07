#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"
#include "ppu.h"
#include "io.h"

class Cartridge {
    private:
        u8 *rom_data;
        u32 rom_size;

    public:
        Cartridge();
        ~Cartridge();
        bool load_rom(char *ROM);
        u8 read(u16 addr);
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
        IO &io;
        PPU &ppu;
        RAM ram;

    public:
        MemoryBus(Cartridge &cart_, IO &io_, PPU &ppu_);
        ~MemoryBus();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
        u8 get_IF();
        void set_IF(u8 val);
        u8 get_IE();

        void emulate_cycles(int cpu_cycles); // For cycle timing
};

#endif