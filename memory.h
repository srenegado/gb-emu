#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"
#include "cart.h"

class MemoryBus {
    private:
        Cartridge &cart;
    public:
        MemoryBus(Cartridge &cart_);
        ~MemoryBus();
        u8 read(u16 addr);
        void write(u16 addr, u8 val);
};

#endif