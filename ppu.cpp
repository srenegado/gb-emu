#include "ppu.h"

PPU::PPU() {
    SDL_Init(SDL_INIT_VIDEO);

    // Set up display
    lcd = SDL_CreateWindow("gb-emu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        lcd_width * lcd_scale, lcd_height * lcd_scale, 0);
    renderer = SDL_CreateRenderer(lcd, -1, SDL_RENDERER_ACCELERATED);
}

PPU::~PPU() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(lcd);
    lcd = nullptr;
    renderer = nullptr;

    SDL_Quit();
}

void PPU::tick() {}

u8 PPU::vram_read(u16 addr) {
    u16 offset = 0x8000;
    addr -= offset;
    return vram[addr];
}

void PPU::vram_write(u16 addr, u8 val) {
    u16 offset = 0x8000;
    addr -= offset;
    vram[addr] = val;
}