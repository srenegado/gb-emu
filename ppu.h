#ifndef PPU_H
#define PPU_H

#include "common.h"
#include "io.h"
#include "SDL.h"

typedef enum {
    Mode_HBlank,
    Mode_VBlank,
    Mode_OAM_Scan,
    Mode_Drawing
} lcd_mode;

class PPU {
    private:
        SDL_Window *lcd = nullptr;
        SDL_Renderer *renderer = nullptr;
        int lcd_width = 160;
        int lcd_height = 144;
        int lcd_scale = 4;
        int dots = 0;
        int dots_per_line = 456;
        int lines_per_frame = 154;

        u8 vram[0x2000]; // Video Ram: 0x8000 - 0x9FFF
        
        IO &io;
    public:
        PPU(IO &io_);
        ~PPU();
        void tick();   
        u8 vram_read(u16 addr);
        void vram_write(u16 addr, u8 val);    
};

#endif