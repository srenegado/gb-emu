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
} ppu_mode;

class PPU {
    private:
        SDL_Window *lcd = nullptr;
        SDL_Renderer *renderer = nullptr;
        
        u32 frames = 0;
        float accum_frame_time_ms = 0;
        const u32 frame_ms = 1000 / 60;
        u32 start_ms = 0;

        const u8 lcd_width = 160;
        const u8 lcd_height = 144;
        const u8 lcd_scale = 4;
        int lcd_buf[144][160] = {0};
        int dots = 0;
        const u16 dots_per_line = 456;
        const u8 lines_per_frame = 154;

        const u8 oam_duration = 80;
        const u8 draw_duration = 80 + 172;

        bool lcd_enabled = 1;
        bool bgw_addr_mode = 1;
        bool bg_map_select = 0;
        bool bgw_enabled = 1;    
        bool sprites_enabled = 0;    

        // 0x9800 - 0x9BFF: Tile Map 0
        // 0x9C00 - 0x9FFF: Tile Map 1
        u8 vram[0x2000] = {0}; // Video Ram: 0x8000 - 0x9FFF
        u8 oam[0xA0]; // Object attribue memory: 0xFE00 - 0xFE9F

        std::vector<u8> sprite_buffer;
        u8 sprite_limit = 10;
        
        IO &io;
    public:
        PPU(IO &io_);
        ~PPU();
        void step();   
        void render_scanline();
        void render_frame();
        u8 vram_read(u16 addr);
        void vram_write(u16 addr, u8 val);  
        void print_vram();
        u8 oam_read(u16 addr);
        void oam_write(u16 addr, u8 val);
        void oam_scan();

};

#endif