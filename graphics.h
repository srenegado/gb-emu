#ifndef GRAPHICS_H
#define GRAPHICS_H

/** 
 * A Tile is an 8x8 square and the base unit for graphics.
 * A 2-bit color ID (0, 1, 2, or 3) is assigned to each of its pixels.
 */
class Tile {
    private:

    public:
        Tile();
        ~Tile();
};

class PPU {
    private:

    public:

        PPU();

        ~PPU();
};

#endif