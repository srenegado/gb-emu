#include "ppu.h"

PPU::PPU(IO &io_) : io(io_) {
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

void PPU::tick() {

    // Do nothing if LCD is off
    if (!BIT(io.get_LCDC(), 7))
        return;

    dots++; // Tick PPU

    // Change PPU mode depending on dots
    ppu_mode prev_mode = (ppu_mode)(io.get_STAT() & 0b11);
    switch (prev_mode) {
        case Mode_OAM_Scan:

            // Set to Drawing mode if OAM scan is finished
            if (dots > 80) 
                io.set_STAT((io.get_STAT() & ~0b11) | 0b11);

            break;
        case Mode_Drawing:

            // Set to HBlank once drawing is finished
            if (dots > 80 + 172) {
                io.set_STAT((io.get_STAT() & ~0b11) | 0b00);   
            
                // Request HBlank STAT interrupt
                if (BIT(io.get_STAT(), 3)) 
                        io.set_IF(io.get_IF() | 0b10); 
            }
            break;
        case Mode_HBlank:
            
            // Go to next scanline and change mode once HBlank is done
            if (dots > dots_per_line) {

                // Update scanline and handle any interrupts and flags
                io.set_LY(io.get_LY() + 1); 
                bool coincidence = io.get_LYC() == io.get_LY();
                if (coincidence) {

                    // Set coincidence flag in STAT
                    io.set_STAT(io.get_STAT() | 0b100);

                    // Request a LYC==LY STAT interrupt
                    if (BIT(io.get_STAT(), 6))
                        io.set_IF(io.get_IF() | 0b10 );
                }
                dots = 0; // Reset for next scanline

                // Change modes to VBlank or OAM Scan
                if (io.get_LY() > lcd_height) {
                    // Gone out of the visible LCD 
    
                    // Set to VBlank 
                    io.set_STAT((io.get_STAT() & ~0b11) | 0b01);

                    // Request a VBlank interrupt
                    io.set_IF(io.get_IF() | 0b0);
    
                    // Request a VBlank STAT interrupt
                    if (BIT(io.get_STAT(), 4))
                        io.set_IF(io.get_IF() | 0b10);
    
                    // Render to SDL display
    
                } else {
                    // Still within the LCD
    
                    // Set to OAM Scan
                    io.set_STAT((io.get_STAT() & ~0b11) | 0b10);

                    // Request an OAM Scan STAT interrupt
                    if (BIT(io.get_STAT(), 5))
                        io.set_IF(io.get_IF() | 0b10);
                }
            }
            
            break;
        case Mode_VBlank:
            if (dots > dots_per_line) {

                // Update scanline and handle any interrupts and flags
                io.set_LY(io.get_LY() + 1); 
                bool coincidence = io.get_LYC() == io.get_LY();
                if (coincidence) {

                    // Set coincidence flag in STAT
                    io.set_STAT(io.get_STAT() | 0b100);

                    // Request a LYC==LY STAT interrupt if enabled
                    if (BIT(io.get_STAT(), 6))
                        io.set_IF(io.get_IF() | 0b10 );
                }
                dots = 0; // Reset for next scanline

                if (io.get_LY() > lines_per_frame) {
                    // VBlank is done
                    
                    // Set to OAM Scan
                    io.set_STAT((io.get_STAT() & ~0b11) | 0b10); 

                    // Request an OAM Scan STAT interrupt if enabled
                    if (BIT(io.get_STAT(), 5))
                        io.set_IF(io.get_IF() | 0b10);

                    io.set_LY(0); // Reset scanline number
                }
            }
            
            break;
    }  
}

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