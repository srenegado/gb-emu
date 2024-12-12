#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>

class Memory {
    private:
        uint8_t map[0x10000];
    
    public:
        // Constructor
        Memory();

        // Destructor
        ~Memory();

        uint8_t read(uint16_t addr);

        void write(uint16_t addr, uint8_t val);

        void load_ROM(char *ROM);
};

#endif