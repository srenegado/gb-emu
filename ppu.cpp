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

void PPU::step() {

    u8 lcd_enabled = BIT(io.get_LCDC(), 7);
    if (!lcd_enabled) {
        return;
    }

    dots++;

    // Change PPU mode depending on dots
    ppu_mode prev_mode = (ppu_mode)(io.get_STAT() & 0b11);
    switch (prev_mode) {
        case Mode_OAM_Scan:

            // Change to Drawing (mode 3)
            if (dots > oam_duration) {
                // std::cout << "PPU: Changing from OAM to Drawing" << std::endl;
                // std::cout << "Dots: " << std::dec << +dots << std::endl;

                io.set_STAT((io.get_STAT() & ~0b11) | 0b11);

                // oam_scan();
            }

            break;
        case Mode_Drawing:

            // Change to HBlank (mode 0)
            if (dots > draw_duration) {
                // std::cout << "PPU: Changing from Drawing to HBlank" << std::endl;
                // std::cout << "Dots: " << std::dec << +dots << std::endl;

                io.set_STAT((io.get_STAT() & ~0b11) | 0b00); // Set mode
                if (BIT(io.get_STAT(), 3)) {                 // Request interrupt
                    io.set_IF(io.get_IF() | 0b10); 
                }

                render_scanline(); // at the start HBlank
            }
            break;
        case Mode_HBlank:
            
            // Go to next scanline and change mode once HBlank is done
            if (dots > dots_per_line) {

                // Update scanline and handle any interrupts and flags
                io.set_LY(io.get_LY() + 1); 
                if (io.get_LYC() == io.get_LY()) {
                    io.set_STAT(io.get_STAT() | 0b100); // Set coincidence flag
                    if (BIT(io.get_STAT(), 6)) {        // Request interrupt
                        io.set_IF(io.get_IF() | 0b10 );
                    }
                }

                // Change modes to VBlank or OAM Scan
                if (io.get_LY() >= lcd_height) {
                    // Scanline out of viewport: Change to VBlank (mode 1)
                    // std::cout << "PPU: Changing from HBlank to VBlank" << std::endl;  
                        
                    io.set_STAT((io.get_STAT() & ~0b11) | 0b01); // Set mode
                    io.set_IF(io.get_IF() | 0b0);                // Request interrupts
                    if (BIT(io.get_STAT(), 4)) {
                        io.set_IF(io.get_IF() | 0b10);
                    }

                    render_frame(); // at the start of VBlank
                } else {
                    // Scanline in viewport: Change to OAM Scan (mode 2)
                    // std::cout << "PPU: Changing from HBlank to OAM scan" << std::endl;
    
                    io.set_STAT((io.get_STAT() & ~0b11) | 0b10); // Set mode
                    if (BIT(io.get_STAT(), 5)) {                 // Request interrupt
                        io.set_IF(io.get_IF() | 0b10);
                    }
                }

                // std::cout << "Dots: " << std::dec << +dots << std::endl;
                // std::cout << "Scanline (LY): " << +io.get_LY() << std::endl;

                dots = 0;
            }
            
            break;
        case Mode_VBlank:

            // Change mode to OAM Scan or stay in VBlank
            if (dots > dots_per_line) {

                // Update scanline and handle any interrupts and flags
                io.set_LY(io.get_LY() + 1); 
                if (io.get_LYC() == io.get_LY()) {
                    io.set_STAT(io.get_STAT() | 0b100); // Set coincidence flag
                    if (BIT(io.get_STAT(), 6)) {        // Request interrupt
                        io.set_IF(io.get_IF() | 0b10 );
                    }
                }

                if (io.get_LY() >= lines_per_frame) {
                    // VBlank is done: Change to OAM scan
                    // std::cout << "PPU: Changing from VBlank to OAM scan" << std::endl;
                    
                    io.set_STAT((io.get_STAT() & ~0b11) | 0b10); // Set mode
                    if (BIT(io.get_STAT(), 5)) {                 // Request interrupt
                        io.set_IF(io.get_IF() | 0b10);
                    }

                    // std::cout << "Dots: " << std::dec << +dots << std::endl;
                    // std::cout << "Scanline (LY): " << +io.get_LY() << std::endl;

                    io.set_LY(0);
                }

                dots = 0;
            }
            
            break;
    }  
}

void PPU::render_scanline() {
    // std::cout << "Rendering scanline " << std::dec << +io.get_LY() << std::endl;
    // std::cout << "LCDC: 0x" << std::hex << +io.get_LCDC() << std::endl;

    // Rendering background
    bgw_enabled = BIT(io.get_LCDC(), 0);
    if (bgw_enabled) {

        // Figure out which addressing mode and which tile map to use 
        bgw_addr_mode = BIT(io.get_LCDC(), 4);
        u16 base_ptr = bgw_addr_mode ? 0x8000 : 0x9000;

        bg_map_select = BIT(io.get_LCDC(), 3);
        u16 bg_map = bg_map_select ? 0x9C00 : 0x9800;

        // std::cout << "Using 0x" << +base_ptr << " addressing mode" << std::endl; 
        // std::cout << "Using tilemap at 0x" << +bg_map << std::endl; 

        // std::cout << "Viewport pos.: x: "  << std::dec << +(io.get_SCX()/8)
        //     << " y: " << +(io.get_SCY()/8) << std::endl; 

        // Iterate by tile then by pixel
        for (int tile_i = 0; tile_i < (int)(lcd_width / 8); tile_i++) {

            // Fetching tile number
            u16 tile_x = (io.get_SCX() / 8 + tile_i) % 32;
            u16 tile_y = ((io.get_LY() + io.get_SCY()) % 256) / 8;
            u16 tile_offset = (32 * tile_y + tile_x) % 1024;
            u16 bg_tile_num_addr = bg_map + tile_offset;
            u8 bg_tile_num = vram[bg_tile_num_addr - 0x8000];

            // std::cout << "Grabbing tile " << std::dec << +bg_tile_num
            //     << " at x: " << +tile_x << " y: " << +tile_y 
            //     << " addr in tilemap: 0x" << std::hex << +bg_tile_num_addr << std::endl; 

            // Fetch tile data
            u16 bg_tile_addr;
            switch (base_ptr) {
                case 0x8000: 
                    bg_tile_addr = base_ptr + ((u16)bg_tile_num * 16);
                    break;
                case 0x9000:
                    int8_t signed_tile_num = (int8_t)bg_tile_num;
                    bg_tile_addr = base_ptr + ((int16_t)signed_tile_num * 16); 
                    break;
            }
            u16 byte_offset = 2 * ((io.get_LY() + io.get_SCY()) % 8);
            u8 lo_byte = vram[bg_tile_addr + byte_offset - 0x8000];
            u8 hi_byte = vram[bg_tile_addr + byte_offset + 1 - 0x8000];

            // std::cout << "Tile addr: 0x" << std::hex << +bg_tile_addr << std::endl; 

            // std::cout << "Grabbing bytes lo: " << std::hex << "0x" << +lo_byte
            //     << " hi: 0x" << +hi_byte
            //     << " starting at 0x" << +(bg_tile_addr + byte_offset) << std::endl; 

            // Render pixels to LCD buffer
            // std::cout << "BGP: 0x" << std::hex << +io.get_BGP() << std::endl;
            for (int pxl_i = 0; pxl_i < 8; pxl_i++) {
                u8 pxl_id = (BIT(hi_byte, 7 - pxl_i) << 1) | BIT(lo_byte, 7 - pxl_i);
                u8 colour = (io.get_BGP() >> (pxl_id * 2)) & 0b11;

                // std::cout << "Pixel id: " << std::dec << +pxl_id 
                //     << " and colour: " << +colour 
                //     << " at (viewport) x: " << +(8 * tile_i + pxl_i) 
                //     << " y: " << +io.get_LY() << std::endl;

                lcd_buf[io.get_LY()][8 * tile_i + pxl_i] = colour;

            }
            
        }

    }

    // Rendering sprites
    sprites_enabled = BIT(io.get_LCDC(), 1);
    if (sprites_enabled) {
        
    }

}

void PPU::render_frame() {
    // std::cout << "Rendering frame" << std::endl;

    // Timing 
    u32 end_ms = SDL_GetTicks();
    u32 time_taken_ms = end_ms - start_ms; 

    // Calculate and show FPS every second
    frames++;
    accum_frame_time_ms += time_taken_ms;
    if (accum_frame_time_ms >= 1000) {
        std::cout << "FPS: " << std::dec << +frames << std::endl;
        frames = 0;
        accum_frame_time_ms = 0;
    }

    // Each frame should take a fixed number of seconds
    if (time_taken_ms < frame_ms) {
        u32 delay_ms = frame_ms - time_taken_ms;
        std::cout << "Delaying for " << std::dec << +delay_ms << " ms" << std::endl; 
        SDL_Delay(delay_ms);
    }

    start_ms = SDL_GetTicks();

    // Rendering pixels from buffer to SDL window
    for (int y = 0; y < lcd_height; y++) {
        for (int x = 0; x < lcd_width; x++) {

            SDL_Rect pxl; 
            pxl.x = x * lcd_scale;
            pxl.y = y * lcd_scale;
            pxl.w = lcd_scale;
            pxl.h = lcd_scale;
            
            switch (lcd_buf[y][x]) {
                case 0: 
                    SDL_SetRenderDrawColor(renderer, 154, 158, 63, 255); // "White"
                    break;
                case 1: 
                    SDL_SetRenderDrawColor(renderer, 73, 107, 34, 255); // "Light gray"
                    break;
                case 2: 
                    SDL_SetRenderDrawColor(renderer, 14, 69, 11, 255); // "Dark gray"
                    break;
                case 3: 
                    SDL_SetRenderDrawColor(renderer, 27, 42, 9, 255); // "Black"
                    break;

            }
            SDL_RenderFillRect(renderer, &pxl); 
        }
    }

    SDL_RenderPresent(renderer);
}

u8 PPU::vram_read(u16 addr) {
    ppu_mode curr_mode = (ppu_mode)(io.get_STAT() & 0b11);
    if (curr_mode == Mode_Drawing) {
        // std::cout << "PPU: Locked out of reading VRAM -> returning garbage\n";
        return 0xFF;
    }

    u16 offset = 0x8000;
    addr -= offset;
    return vram[addr];
}

void PPU::vram_write(u16 addr, u8 val) {
    ppu_mode curr_mode = (ppu_mode)(io.get_STAT() & 0b11);
    if (curr_mode == Mode_Drawing) {
        // std::cout << "PPU: Locked out of writing VRAM\n";
        return; 
    }
    
    u16 offset = 0x8000;
    addr -= offset;
    vram[addr] = val;
}

void PPU::print_vram() {
    u16 start = 0x8000;
    std::cout << "0x" << std::hex << +start << ": ";
    for (int i = 0; i < 0x2000; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << +vram[i] << " ";
        if (i % 16 == 15 and i != 0) {
            std::cout << std::endl;
            start += 0x10;
            std::cout << "0x" << std::hex << +start << ": ";
        }
    }
}

u8 PPU::oam_read(u16 addr) {
    ppu_mode curr_mode = (ppu_mode)(io.get_STAT() & 0b11);
    if (curr_mode == Mode_Drawing || curr_mode == Mode_OAM_Scan) {
        // std::cout << "PPU: Locked out of reading OAM -> returning garbage\n";
        return 0xFF;
    }

    u16 offset = 0xFE00;
    addr -= offset;
    return vram[addr];
}

void PPU::oam_write(u16 addr, u8 val) {
    ppu_mode curr_mode = (ppu_mode)(io.get_STAT() & 0b11);
    if (curr_mode == Mode_Drawing || curr_mode == Mode_OAM_Scan) {
        // std::cout << "PPU: Locked out of writing OAM\n";
        return; 
    }
    
    u16 offset = 0xFE00;
    addr -= offset;
    vram[addr] = val;
}

void PPU::oam_scan() {
    int sprites = 0;

    // std::cout << "Doing OAM scan" << std::endl;
    
    bool adding_sprite = false;
    u8 sprite_size = BIT(io.get_LCDC(), 2);

    // There are 40 sprites in OAM
    for (int sprite_i = 0; sprite_i < 40; sprite_i++) {
        
        // Grabbing sprite 
        u8 sprite_addr = 4 * sprite_i;
        u8 y_pos = oam[sprite_addr];
        u8 x_pos = oam[sprite_addr + 1];
        u8 tile_ind = oam[sprite_addr + 2];
        u8 attribs = oam[sprite_addr + 3];
        
        // std::cout << "Scanning sprite at addr: 0x" << std::hex << +(0xFE00+sprite_addr) << std::endl;
        
        u8 height = sprite_size ? 16 : 8;
        bool add_sprite = ((io.get_LY() + 16) >= y_pos) && ((io.get_LY() + 16) < (y_pos + height));
        if (add_sprite) {
            std::cout << "Adding sprite at addr: 0x" << std::hex << +(0xFE00+sprite_addr) << std::endl;
            std::cout << "LY + 16: " << std::dec << +(io.get_LY() + 16)
                << " sprite y: " << +y_pos << " sprite height: " << +height << std::endl;
        } 


        if (adding_sprite) sprites++;
        if (sprites >= sprite_limit) break;     // only up to 10 sprites per scanline
    }

    sprite_limit = 10;
}