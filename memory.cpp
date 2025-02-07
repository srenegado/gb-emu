#include "memory.h"

MemoryBus::MemoryBus(Cartridge &cart_) : cart(cart_) {}
MemoryBus::~MemoryBus() {}

u8 MemoryBus::read(u16 addr) {
    if (addr < 0x8000) {
        // Reading from ROM
        return cart.read(addr);
    }

    std::cout << "TODO: Implement the rest of memory\n";
}

void MemoryBus::write(u16 addr, u8 val) {
    if (addr < 0x8000) {
        // No writes to ROM
        return;
    }

    std::cout << "TODO: Implement the rest of memory\n";
}