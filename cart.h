#ifndef CART_H
#define CART_H

#include "common.h"

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

#endif