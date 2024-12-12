#include "memory.h"

// Constructor
Memory::Memory() {
    std::memset(map, 0, sizeof(map));
}

// Destructor
Memory::~Memory() {}

uint8_t Memory::read(uint16_t addr) {
    return map[addr];
}

void Memory::write(uint16_t addr, uint8_t val) {
    map[addr] = val;
}

void Memory::load_ROM(char *ROM) {
    if (ROM == nullptr)
        return;

    uint8_t *start_addr = &map[0x0000];
    FILE *ROM_file = std::fopen(ROM, "rb");
    if (ROM_file == nullptr)
        throw std::invalid_argument("Opening ROM file failed");

    std::fread(start_addr, 0x8000, 1, ROM_file);
    std::fclose(ROM_file);
}