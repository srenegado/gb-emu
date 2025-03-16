#include "memory.h"

MemoryBus::MemoryBus(Cartridge &cart_, IO &io_, PPU &ppu_) : cart(cart_), io(io_), ppu(ppu_) {}
MemoryBus::~MemoryBus() {}

u8 MemoryBus::read(u16 addr) {
    if (addr < 0x8000) {
        // Reading from ROM
        return cart.read(addr);

    } else if (addr < 0xA000) {
        // Reading from VRAM
        return ppu.vram_read(addr);

    } else if (addr < 0xC000) {
        // Reading from Cartridge RAM
        return cart.read(addr);

    } else if (addr < 0xE000) {
        // Reading from WRAM
        return ram.wram_read(addr);

    } else if (addr < 0xFE00) {
        // Echo RAM is reserved
        std::cout << "Echo ram: reading prohibited area at: 0x" << std::hex << addr << std::endl;
        return 0;

    } else if (addr < 0xFEA0) {
        // Reading from OAM
        ppu.oam_read(addr);
        return 0;

    } else if (addr < 0xFF00) {
        // Not usable
        std::cout << "Not usable: reading prohibited area at: 0x" << std::hex << addr << std::endl;
        return 0;

    } else if (addr < 0xFF80) {
        // Reading from I/O registers
        return io.read(addr);

    } else if (addr == 0xFFFF) {
        // Reading IE register
        return io.get_IE();
        
    }

    // Reading from HRAM
    return ram.hram_read(addr);
}

void MemoryBus::write(u16 addr, u8 val) {
    if (addr < 0x8000) {
        // No writes to ROM
        std::cout << "ROM: no writing at: 0x" << std::hex << addr << std::endl;
        return;

    } else if (addr < 0xA000) {
        // Writing to VRAM
        ppu.vram_write(addr, val);

    } else if (addr < 0xC000) {
        // Writing to Cartridge RAM
        std::cout << "Unsupported bus write at: 0x" << std::hex << addr << std::endl;

    } else if (addr < 0xE000) {
        // Writing to WRAM
        ram.wram_write(addr, val);
  
    } else if (addr < 0xFE00) {
        // Echo RAM is reserved
        std::cout << "Echo ram: writing to prohibited area at: 0x" << std::hex << addr << std::endl;
     
    } else if (addr < 0xFEA0) {
        // Writing to OAM
        ppu.oam_write(addr, val);
       
    } else if (addr < 0xFF00) {
        // Not usable
        std::cout << "Not usable: writing to prohibited area at: 0x" << std::hex << addr << std::endl;
       
    } else if (addr < 0xFF80) {
        // Writing to I/O registers
        if (addr == 0xFF46) dma_transfer(val);
        else io.write(addr, val);
        
    } else if (addr == 0xFFFF) {
        // Setting IE register
        io.set_IE(val);
        
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
    return io.get_IE();
}

void MemoryBus::emulate_cycles(int cpu_cycles) {

    // There are 4 "T-cycles" in each "M-cycle"
    int system_clock_ticks = 4 * cpu_cycles; 

    for (int i = 0; i < system_clock_ticks; i++ ) {

        if (io.timer_tick()) { // Timer requested an interrupt
            io.set_IF(io.get_IF() | 0b100); 
        }

        ppu.step();

    }
}

void MemoryBus::dma_transfer(u8 val) {
    // std::cout << "Starting OAM DMA transfer" << std::endl;
    u16 offset = ((u16)val) << 8;
    for (u16 addr_i = 0; addr_i < 0xA0; addr_i++) {
        ppu.oam_write(0xFE00+addr_i, read(offset+addr_i)); 
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

Cartridge::Cartridge() : rom_data(nullptr) {}
Cartridge::~Cartridge() { if (rom_data) delete[] rom_data; }

bool Cartridge::load_rom(char *ROM) {

    // Open ROM file
    std::ifstream ifs;
    ifs.open(ROM);
    if (ifs.fail()) {
        std::cout << "ROM failed to open\n";
        return false;
    }

    // Figure out ROM size
    ifs.seekg(0, std::ios::end);
    rom_size = ifs.tellg();

    // Rewind back to beginning of ROM file
    ifs.seekg(0, std::ios::beg);
    
    // Load ROM data into Cartridge
    rom_data = new u8[rom_size];
    ifs.read((char*)rom_data, rom_size);
    ifs.close();

    std::cout << "Game cartridge loaded\n";

    return true;
}

u8 Cartridge::read(u16 addr) {
    return rom_data[addr];
}