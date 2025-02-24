#ifndef PPU_H
#define PPU_H

#include "common.h"
#include "SDL.h"

class PPU {
    private:
        SDL_Window *lcd = nullptr;
        SDL_Renderer *renderer = nullptr;
        int lcd_width = 160;
        int lcd_height = 144;
        int lcd_scale = 4;
        u8 vram[0x2000]; // Video Ram: 0x8000 - 0x9FFF
    public:
        PPU();
        ~PPU();
        void tick();   
        u8 vram_read(u16 addr);
        void vram_write(u16 addr, u8 val);    
};

#endif