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
        return ppu.oam_read(addr);

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
        // Writing to ROM
        cart.write(addr, val);

    } else if (addr < 0xA000) {
        // Writing to VRAM
        ppu.vram_write(addr, val);

    } else if (addr < 0xC000) {
        // Writing to Cartridge RAM
        cart.write(addr, val);

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
    u32 size = ifs.tellg();

    // Rewind back to beginning of ROM file
    ifs.seekg(0, std::ios::beg);
    
    // Load ROM data into Cartridge
    rom_data = new u8[size];
    ifs.read((char*)rom_data, size);
    ifs.close();

    cart_type = rom_data[0x147];
    rom_size = 32 * (1 << (u32)rom_data[0x148]);

    switch (rom_data[0x149]) {
        case 0x00: ram_size = 0; break;
        case 0x02: ram_size = 8; break;
        case 0x03: ram_size = 32; break;
        case 0x04: ram_size = 128; break;
        case 0x05: ram_size = 64; break;
    }

    std::cout << "Game cartridge loaded\n";
    // std::cout << "ROM Size: " << std::dec << +(rom_size) << "KB\n";
    // std::cout << "SRAM Size: "<< std::dec << +(ram_size) << "KB\n";
    // std::cout << "Cartridge type code: 0x" << std::hex << +cart_type << std::endl;

    return true;
}

u8 Cartridge::read(u16 addr) {
    switch (cart_type) {
        case 0x00: // No MBC
            // std::cout << "Reading from ROM w/out MBC\n";
            return rom_data[addr];

        case 0x01: // MBC1
        case 0x02: // MBC1 with RAM
        case 0x03: // MBC1 with Battery-buffered RAM

            if (addr <= 0x3FFF) {
                // ROM Bank X0

                if (rom_size >= 1024 && mode_flag == 1) {
                    u8 bank_x0_num = rom_bank_num & (0b11 << 5);
                    
                    // std::cout << "Select Bank X0: 0x" << std::hex << bank_x0_num << std::endl;
                    
                    return rom_data[0x4000 * bank_x0_num + addr];
                }
                
                // std::cout << "Reading ROM Bank 0 at addr: 0x" << std::hex << +addr << std::endl;
                return rom_data[addr];

            } else if (addr <= 0x7FFF) {
                // ROM Bank

                // std::cout << "Addr given to cart.read(addr): 0x" << std::hex << +addr << std::endl;

                if (rom_bank_num == 0 || rom_size >= 1024) {
                    // ROM banks 0x00/0x20/0x40/0x60 are not accessible here
                    switch (rom_bank_num) {
                        case 0x00:
                        case 0x20:
                        case 0x40:
                        case 0x60:
                            // Go 1 bank higher
                            
                            // std::cout << "Translating ROM bank 0x" << std::hex
                            //     << +rom_bank_num << "->0x" << +(rom_bank_num + 0x1) << std::endl;
                            
                            return rom_data[0x4000 * (rom_bank_num + 0x01) + addr - 0x4000];
                    }
                }

                // std::cout << "Reading from ROM Bank " << std::dec << +rom_bank_num 
                //     << " at addr: 0x" << std::hex << (0x4000 * rom_bank_num + addr - 0x4000) << std::endl;
                
                return rom_data[0x4000 * rom_bank_num + addr - 0x4000];

            } else if (0xA000 <= addr && addr <= 0xBFFF) {
                // Reading External RAM/SRAM

                // Only read RAM when enabled
                if (!enable_ram) {
                    // std::cout << "SRAM not enabled: cannot read!\n";
                    break;
                }

                // Read the correct RAM bank
                u16 ram_addr = addr - 0xA000; 
                if (ram_size == 32 && mode_flag == 1) 
                    ram_addr += (0x2000 * ram_bank_num);

                // std::cout << "Reading SRAM at addr: 0x" << std::hex << +ram_addr <<std::endl;
                return sram[ram_addr];

            }

    }

    return 0xFF; // Some garbage value
}

void Cartridge::write(u16 addr, u8 val) {
    switch (cart_type) {
        case 0x00: break; // No MBC -> no writes

        case 0x01: // MBC1
        case 0x02: // MBC1 with RAM
        case 0x03: // MBC1 with Battery-buffered RAM

            if (0xA000 <= addr && addr <= 0xBFFF) {
                // Writing to External RAM/SRAM

                // Only write to RAM when enabled
                if (!enable_ram) {
                    // std::cout << "SRAM not enabled: cannot write!\n";
                    break;
                }
                
                // Write to the correct RAM bank
                u16 ram_addr = addr - 0xA000; 
                if (ram_size == 32 && mode_flag == 1) 
                    ram_addr += (0x2000 * ram_bank_num);

                // std::cout << "Writing to SRAM at addr: 0x" << std::hex << +ram_addr <<std::endl;
                sram[ram_addr] = val;

                break;                
            }

            // Writing to MBC1 Registers

            if (addr <= 0x1FFF) {
                // RAM Enable
                enable_ram = ((val & 0xF) == 0xA) ? true : false;
                // std::cout << "Trying to enable RAM: " << std::hex << +enable_ram << std::endl;

            } else if (addr <= 0x3FFF) {
                // ROM Bank Number
                // std::cout << "Setting ROM bank number: 0x";
                if (val == 0x00) { 
                    rom_bank_num = 0x01;
                    // std::cout << "00 -> 0x0" << std::hex << +rom_bank_num << std::endl;
                    break;
                }

                // Only need n bits to represent 2^n ROM banks
                u8 bit_mask;
                switch (rom_size) {
                    case 2048:
                    case 1024:
                    case 512:
                        bit_mask = 0b11111;
                        break;
                    case 256: bit_mask = 0b1111; break;
                    case 128: bit_mask = 0b111; break;
                    case 64: bit_mask = 0b11; break;
                    case 32: bit_mask = 0b1; break; 
                }
                
                rom_bank_num = val & bit_mask;

                // std::cout << std::hex << +rom_bank_num << " using bit mask 0x" << +bit_mask << std::endl;

            } else if (addr <= 0x5FFF) {
                // RAM Bank Number or Upper Bits of ROM Bank Number
                if (ram_size == 32) {
                    // Choose between 4 RAM banks
                    ram_bank_num = val & 0b11;
                }
                if (rom_size >= 1024) {
                    // Need more bits to represent ROM banks
                    rom_bank_num += ((val & 0b11) << 5);
                }

            } else if (addr <= 0x7FFF) {
                // Banking Mode Select
                mode_flag = val & 0b1;

            } 

            break;
    }
}