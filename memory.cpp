#include "memory.h"

MemoryBus::MemoryBus(Cartridge &cart_) : cart(cart_) {}
MemoryBus::~MemoryBus() {}

u8 MemoryBus::read(u16 addr) {
    if (addr < 0x8000) {
        // Reading from ROM
        return cart.read(addr);

    } else if (addr < 0xA000) {
        // Reading from VRAM
        std::cout << "Unsupported bus read at: 0x" << addr << std::endl;
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
        std::cout << "Unsupported bus read at: 0x" << addr << std::endl;
        return 0;

    } else if (addr < 0xFF00) {
        // Not usable
        return 0;

    } else if (addr < 0xFF80) {
        // Reading from I/O registers
        return io.read(addr);

    } else if (addr == 0xFFFF) {
        // Reading IE register
        return IE;
        
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
        std::cout << "Unsupported bus write at: 0x" << addr << std::endl;
    } else if (addr < 0xC000) {
        // Writing to Cartridge RAM
        std::cout << "Unsupported bus write at: 0x" << addr << std::endl;

    } else if (addr < 0xE000) {
        // Writing to WRAM
        ram.wram_write(addr, val);
  
    } else if (addr < 0xFE00) {
        // Echo RAM is reserved
     
    } else if (addr < 0xFEA0) {
        // Writing to OAM
        std::cout << "Unsupported bus write at: 0x" << addr << std::endl;
       
    } else if (addr < 0xFF00) {
        // Not usable
       
    } else if (addr < 0xFF80) {
        // Writing to I/O registers
        io.write(addr, val);
        
    } else if (addr == 0xFFFF) {
        // Setting IE register
        IE = val;
        
    } else {
        // Writing to HRAM
        ram.hram_write(addr, val);

    }

}

u8 MemoryBus::get_IF() {
    return io.get_IF();
}

void MemoryBus::set_IF(u8 val) {
    io.set_IF(val);
}

u8 MemoryBus::get_IE() {
    return IE;
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

IO::IO() {}
IO::~IO() {}

u8 IO::read(u16 addr) {
    if (addr == 0xFF01) {
        // std::cout << "Reading from SB!" << std::endl;
        return serial_data[0];

    } else if (addr == 0xFF02) {
        // std::cout << "Reading from SC!" << std::endl;
        return serial_data[1];

    } else if (0xFF04 <= addr && addr <= 0xFF07) {
        // Reading timers

    } else if (addr == 0xFF0F) {
        // Reading Interrupt flags (IF)
        return IF;

    } else {
        std::cout << "Unsupported bus read at: 0x" << addr << std::endl;

    }

    return 0;
}

void IO::write(u16 addr, u8 val) {
    if (addr == 0xFF01) {
        // std::cout << "Writing to SB!" << std::endl;
        serial_data[0] = val;

    } else if (addr == 0xFF02) {
        // std::cout << "Writing to SC!" << std::endl;
        serial_data[1] = val;

    } else if (0xFF04 <= addr && addr <= 0xFF07) {
        // Writing to timers
        std::cout << "Unsupported bus write at: 0x" << addr << std::endl;

    } else if (addr == 0xFF0F) {
        // Writing to Interrupt flags (IF)
        IF = val;
        
    } else {
        std::cout << "Unsupported bus write at: 0x" << addr << std::endl;
    }
}

u8 IO::get_IF() {
    return IF;
}

void IO::set_IF(u8 val) {
    IF = val;
}