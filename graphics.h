#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cstdint>
#include "SDL_video.h"
#include "SDL_render.h"

#define LCD_WIDTH 160
#define LCD_HEIGHT 144
#define LCD_SCALE 4

class PPU {
    private:
        SDL_Window   *LCD;
        SDL_Renderer *LCD_renderer;

    public:
        // Constructor
        PPU();

        // Destructor
        ~PPU();

        /** 
         * A Tile is an 8x8 square and the base unit for graphics.
         * A 2-bit color ID (0, 1, 2, or 3) is assigned to each of its pixels (2bpp).
         * So, a Tile in memory is 16 bytes.
         */

};

#endif