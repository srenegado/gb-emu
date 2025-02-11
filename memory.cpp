#include "memory.h"

MemoryBus::MemoryBus(Cartridge &cart_) : cart(cart_) {}
MemoryBus::~MemoryBus() {}

u8 MemoryBus::read(u16 addr) {
    if (addr < 0x8000) {
        // Reading from ROM
        return cart.read(addr);
    } else if (addr < 0xA000) {
        // Reading from VRAM
        std::cout << "Unsupported bus read at: " << addr << std::endl;
        return 0;
    } else if (addr < 0xC000) {
        // Reading from Cartridge RAM
        return cart.read(addr);
    } else if (addr < 0xE000) {
        // Reading from WRAM
        return ram.wram_read(addr);
    } else if (addr < 0xFE00) {
        // Echo RAM is reserved
        return 0;
    } else if (addr < 0xFEA0) {
        // Reading from OAM
        std::cout << "Unsupported bus read at: " << addr << std::endl;
        return 0;
    } else if (addr < 0xFF00) {
        // Not usable
        return 0;
    } else if (addr < 0xFF80) {
        // Reading from I/O registers
        std::cout << "Unsupported bus read at: " << addr << std::endl;
        return 0;
    } else if (addr == 0xFF00) {
        // Reading IE register
        std::cout << "Unsupported bus read at: " << addr << std::endl;
        return 0;
    }

    // Reading from HRAM
    return ram.hram_read(addr);
}

void MemoryBus::write(u16 addr, u8 val) {
    if (addr < 0x8000) {
        // No writes to ROM
        return;

    } else if (addr < 0xA000) {
        // Writing to VRAM
        std::cout << "Unsupported bus write at: " << addr << std::endl;
    } else if (addr < 0xC000) {
        // Writing to Cartridge RAM
        std::cout << "Unsupported bus write at: " << addr << std::endl;

    } else if (addr < 0xE000) {
        // Writing to WRAM
        ram.wram_write(addr, val);
  
    } else if (addr < 0xFE00) {
        // Echo RAM is reserved
     
    } else if (addr < 0xFEA0) {
        // Writing to OAM
        std::cout << "Unsupported bus write at: " << addr << std::endl;
       
    } else if (addr < 0xFF00) {
        // Not usable
       
    } else if (addr < 0xFF80) {
        // Writing to I/O registers
        std::cout << "Unsupported bus write at: " << addr << std::endl;
        
    } else if (addr == 0xFF00) {
        // Setting IE register
        std::cout << "Unsupported bus write at: " << addr << std::endl;
        
    } else {
        // Writing to HRAM
        ram.hram_write(addr, val);

    }

}

RAM::RAM() {}
RAM::~RAM() {}

u8 RAM::wram_read(u16 addr) {
    u16 offset = 0xC000;
    addr -= offset;
    return wram[addr];
}

void RAM::wram_write(u16 addr, u8 val) {
    u16 offset = 0xC000;
    addr -= offset;
    wram[addr] = val;
}

u8 RAM::hram_read(u16 addr) {
    u16 offset = 0xFF80;
    addr -= offset;
    return hram[addr];
}

void RAM::hram_write(u16 addr, u8 val) {
    u16 offset = 0xFF80;
    addr -= offset;
    hram[addr] = val;
}