#include <cstdint>
#include "graphics.h"

Tile::Tile() {};

Tile::~Tile() {};

/** 
  * Store tile into memory starting from its address
  */
void Tile::store_tile(Memory &mem) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
           mem.write(addr + 8*i + j, pixels[i][j]);
        }
    }
}



PPU::PPU() {};

PPU::~PPU() {};