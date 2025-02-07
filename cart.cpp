#include "cart.h"

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
