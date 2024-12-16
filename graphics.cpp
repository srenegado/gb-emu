#include "graphics.h"

// Constructor
PPU::PPU() {
    LCD = SDL_CreateWindow("gb-emu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LCD_WIDTH * LCD_SCALE, LCD_HEIGHT * LCD_SCALE, 0);
    // Using hardware acceleration
    LCD_renderer = SDL_CreateRenderer(LCD, -1, SDL_RENDERER_ACCELERATED);
}

// Destructor
PPU::~PPU() {
    SDL_DestroyRenderer(LCD_renderer);
    SDL_DestroyWindow(LCD);
    LCD = nullptr;
    LCD_renderer = nullptr;
}
