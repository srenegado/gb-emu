#include "ppu.h"

PPU::PPU(IO &io_, EventHandler &event_handler_) 
    : io(io_), event_handler(event_handler_) {
    SDL_Init(SDL_INIT_VIDEO);

    // Set up display
    lcd = SDL_CreateWindow("gb-emu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        lcd_width * lcd_scale, lcd_height * lcd_scale, 0);
    renderer = SDL_CreateRenderer(lcd, -1, SDL_RENDERER_ACCELERATED);

    for (int y = 0; y < lcd_height; y++) {
        for (int x = 0; x < lcd_width; x++) {
            lcd_buf[y][x] = BGW_ID_0;
        }
    }
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
                    io.set_IF(io.get_IF() | 0b1);                // Request interrupts
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
        colour_id bg_palette[4] = {BGW_ID_0, BGW_ID_1, BGW_ID_2, BGW_ID_3}; 

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
            for (int pxl_i = 0; pxl_i < 8; pxl_i++) {
                u8 pxl_id = (BIT(hi_byte, 7 - pxl_i) << 1) | BIT(lo_byte, 7 - pxl_i);

                // std::cout << "Pixel id: " << std::dec << +pxl_id 
                //     << " at (viewport) x: " << +(8 * tile_i + pxl_i) 
                //     << " y: " << +io.get_LY() << std::endl;

                lcd_buf[io.get_LY()][8 * tile_i + pxl_i] = bg_palette[pxl_id];
            }
        }

        // Rendering window
        win_enabled = BIT(io.get_LCDC(), 5);
        if (win_enabled && (io.get_WY() <= io.get_LY())) {

            // Figure out which window tile map to use
            win_map_select = BIT(io.get_LCDC(), 6);
            u16 win_map = (win_map_select) ? 0x9C00 : 0x9800; 

            // Iterate by tile then by pixel
            for (
                int tile_i = 0;
                (tile_i + (io.get_WX() / 8)) < (int)((lcd_width + 7) / 8);
                tile_i++
            ) {
                // Fetching tile number
                u16 tile_y = ((io.get_LY() - io.get_WY())) / 8;
                u16 tile_offset = (32 * tile_y + tile_i);
                u16 win_tile_num_addr = win_map + tile_offset;
                u8 win_tile_num = vram[win_tile_num_addr - 0x8000];

                // Fetch tile data
                u16 win_tile_addr;
                switch (base_ptr) {
                    case 0x8000: 
                        win_tile_addr = base_ptr + ((u16)win_tile_num * 16);
                        break;
                    case 0x9000:
                        int8_t signed_tile_num = (int8_t)win_tile_num;
                        win_tile_addr = base_ptr + ((int16_t)signed_tile_num * 16); 
                        break;
                }
                u16 byte_offset = 2 * ((io.get_LY() - io.get_WY()) % 8);
                u8 lo_byte = vram[win_tile_addr + byte_offset - 0x8000];
                u8 hi_byte = vram[win_tile_addr + byte_offset + 1 - 0x8000];

                // Render pixels to LCD buffer
                for (int pxl_i = 0; pxl_i < 8; pxl_i++) {
                    u8 pxl_id = (BIT(hi_byte, 7 - pxl_i) << 1) | BIT(lo_byte, 7 - pxl_i);
                    lcd_buf[io.get_LY()][(io.get_WX() - 7) + 8 * tile_i + pxl_i] = bg_palette[pxl_id];
                }
            }
        }
    }    

    // Rendering sprites
    sprites_enabled = BIT(io.get_LCDC(), 1);
    if (sprites_enabled) {

        // Fill up sprite buffer
        oam_scan();

        // Sort sprite buffer by descending drawing priority
        std::sort(sprite_buffer.begin(), sprite_buffer.end(), 
            [&](u8 const &addr1, u8 const &addr2) {
                u8 x_pos1 = oam[addr1 + 1];
                u8 x_pos2 = oam[addr2 + 1];
                if (x_pos1 == x_pos2) return addr1 < addr2;
                else return x_pos1 < x_pos2;
            }
        );

        // if (!sprite_buffer.empty()) {
        //     std::cout << "Printing sprite buffer contents: ";
        //     std::deque<u8>::iterator it = sprite_buffer.begin();
        //     while (it != sprite_buffer.end()) {
        //         std::cout << " 0x" << std::hex << +(*it+0xFE00);
        //         it++;
        //     }
        //     std::cout << std::endl;
        // }

        colour_id temp_scanline[lcd_width];
        for (int i = 0; i < lcd_width; i++) temp_scanline[i] = None_Transparent;

        // Sprite size is global
        sprite_size = BIT(io.get_LCDC(), 2);
        u8 sprite_height = sprite_size ? 16 : 8; 

        // Go through sprite buffer (which was filled by OAM Scan)
        while (!sprite_buffer.empty()) {
            // std::cout << "Drawing sprite" << std::endl;

            // Grabbing sprite
            u8 sprite_addr = sprite_buffer.back();
            sprite_buffer.pop_back();

            u8 y_pos = oam[sprite_addr];
            u8 x_pos = oam[sprite_addr + 1];
            u8 tile_num = oam[sprite_addr + 2];
            u8 attribs = oam[sprite_addr + 3];

            // Don't draw a hidden sprite
            if (x_pos == 0 || x_pos >= 168) continue; 

            // Grabbing sprite attributes
            bool behind_bgw = BIT(attribs, 7);
            bool y_flip = BIT(attribs, 6);
            bool x_flip = BIT(attribs, 5);
            bool palette_select = BIT(attribs, 4);

            // std::cout << "Rendering sprite at addr: 0x" << std::hex << +(0xFE00+sprite_addr)
            //     << " LY: " << std::dec << +io.get_LY()
            //     << " sprite x: " << +x_pos << " sprite y: " << +y_pos << " tile num: " << +tile_num
            //     << " y_flip: " << +y_flip << " x_flip: " << +x_flip
            //     << " behind_bgw: " << +behind_bgw << std::endl;
                
            // Fetch tile data
            u16 sprite_tile_addr = (u16)tile_num * 16;
            if (sprite_height == 16) {
                bool grab_top_tile = 
                    (!y_flip && io.get_LY() + 16 < y_pos + 8)
                    || (y_flip && io.get_LY() + 16 >= y_pos + 8);

                sprite_tile_addr = (grab_top_tile) 
                    ? ((u16)(tile_num & 0xFE)) * 16
                    : ((u16)(tile_num | 0x01)) * 16;   
            }

            // std::cout << "Sprite tile addr (in VRAM): 0x" << std::hex << +(0x8000+sprite_tile_addr) << std::endl;

            u16 byte_offset = (y_flip) 
                ? 2 * ((y_pos + 7 * (io.get_LY() + 16 + 1)) % 8)
                : 2 * ((io.get_LY() + 16 - y_pos) % 8);

            // u16 byte_offset = 2 * ((io.get_LY() + 16 - y_pos) % 8);

            // std::cout << "Byte offset in sprite tile: 0x" << std::hex << +(byte_offset) << std::endl;

            u8 lo_byte = vram[sprite_tile_addr + byte_offset];
            u8 hi_byte = vram[sprite_tile_addr + byte_offset + 1];
            
            // Render pixels to a temporary buffer
            colour_id sprite_palette0[4] = 
                {None_Transparent, OBP0_ID_1, OBP0_ID_2, OBP0_ID_3};
            colour_id sprite_palette1[4] = 
                {None_Transparent, OBP1_ID_1, OBP1_ID_2, OBP1_ID_3};
            for (
                int pxl_i = 0; 
                pxl_i < 8 && ((x_pos + pxl_i) < (lcd_width + 8)); 
                pxl_i++      // Don't render when pixel is hidden on the "right side"
            ) {
                // Don't render when pixel is hidden on the "left side"
                if (x_pos + pxl_i < 8) continue; 

                // Get colour ID for pixel
                u8 pxl_id = (x_flip) 
                    ? (BIT(hi_byte, pxl_i) << 1) | BIT(lo_byte, pxl_i)
                    : (BIT(hi_byte, 7 - pxl_i)  << 1) | BIT(lo_byte, 7 - pxl_i);  

                // Scanline doesn't account for offscreen coordinates
                temp_scanline[x_pos - 8 + pxl_i] = (palette_select) 
                    ? sprite_palette1[pxl_id] : sprite_palette0[pxl_id];   

                // Mask sprite by BG/W colours 1-3 when enabled 
                if (behind_bgw) {
                    colour_id bgw_cid = lcd_buf[io.get_LY()][x_pos - 8 + pxl_i];
                    if (bgw_cid != BGW_ID_0)
                        temp_scanline[x_pos - 8 + pxl_i] = None_Transparent;
                }       
            }    
        }

        // Render temporary buffer to LCD buffer
        for (int pxl_i = 0; pxl_i < lcd_width; pxl_i++) {
            if (temp_scanline[pxl_i] != None_Transparent)
                lcd_buf[io.get_LY()][pxl_i] = temp_scanline[pxl_i];
        }

    }

}

void PPU::render_frame() {

    // std::cout << "Rendering frame" << std::endl;

    // Timing 
    u32 end_ms = SDL_GetTicks();
    u32 time_taken_ms = end_ms - start_ms; 

    // std::cout << "Frame time taken (ms): " << std::dec << +time_taken_ms << std::endl;

    // Show FPS every second
    frames++;
    accum_frame_time_ms += time_taken_ms;
    if (accum_frame_time_ms >= 1000) { // 1000 ms = 1 s
        std::cout << "FPS: " << std::dec << frames << std::endl;
        frames = 0;
        accum_frame_time_ms = 0;
    }

    // Each frame should take a fixed number of seconds
    if (time_taken_ms < frame_ms) {
        u32 delay_ms = frame_ms - time_taken_ms;
        // std::cout << "Delaying for " << std::dec << +delay_ms << " ms" << std::endl; 
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

            u8 colour;
            switch (lcd_buf[y][x]) {
                case BGW_ID_0: 
                    colour = io.get_BGP() & 0b11;
                    break;
                case BGW_ID_1: 
                    colour = (io.get_BGP() >> 2) & 0b11;
                    break;
                case BGW_ID_2: 
                    colour = (io.get_BGP() >> 4) & 0b11;
                    break;
                case BGW_ID_3: 
                    colour = (io.get_BGP() >> 6) & 0b11;
                    break;
                case OBP0_ID_1: 
                    colour = (io.get_OBP0() >> 2) & 0b11;
                    break;
                case OBP0_ID_2: 
                    colour = (io.get_OBP0() >> 4) & 0b11;
                    break;
                case OBP0_ID_3: 
                    colour = (io.get_OBP0() >> 6) & 0b11;
                    break;
                case OBP1_ID_1: 
                    colour = (io.get_OBP1() >> 2) & 0b11;
                    break;
                case OBP1_ID_2: 
                    colour = (io.get_OBP1() >> 4) & 0b11;
                    break;
                case OBP1_ID_3: 
                    colour = (io.get_OBP1() >> 6) & 0b11;
                    break;
            }
            switch (colour) {
                case 0: // "White"
                    SDL_SetRenderDrawColor(renderer, 154, 158, 63, 255); 
                    break;
                case 1: // "Light gray"
                    SDL_SetRenderDrawColor(renderer, 73, 107, 34, 255); 
                    break;
                case 2: // "Dark gray"
                    SDL_SetRenderDrawColor(renderer, 14, 69, 11, 255); 
                    break;
                case 3: // "Black"
                    SDL_SetRenderDrawColor(renderer, 27, 42, 9, 255); 
                    break;

            }
            SDL_RenderFillRect(renderer, &pxl); 
        }
    }

    SDL_RenderPresent(renderer);

    // Handling shutdown requests every frame speeds up emulator
    event_handler.handle_events();   
}

u8 PPU::vram_read(u16 addr) {
    // ppu_mode curr_mode = (ppu_mode)(io.get_STAT() & 0b11);
    // if (curr_mode == Mode_Drawing) {
    //     // std::cout << "PPU: Locked out of reading VRAM -> returning garbage\n";
    //     return 0xFF;
    // }

    u16 offset = 0x8000;
    addr -= offset;
    return vram[addr];
}

void PPU::vram_write(u16 addr, u8 val) {
    // ppu_mode curr_mode = (ppu_mode)(io.get_STAT() & 0b11);
    // if (curr_mode == Mode_Drawing) {
    //     // std::cout << "PPU: Locked out of writing VRAM\n";
    //     return; 
    // }
    
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
    // std::cout << "Reading OAM" << std::endl;
    // ppu_mode curr_mode = (ppu_mode)(io.get_STAT() & 0b11);
    // if (curr_mode == Mode_Drawing || curr_mode == Mode_OAM_Scan) {
    //     // std::cout << "PPU: Locked out of reading OAM -> returning garbage\n";
    //     return 0xFF;
    // }

    u16 offset = 0xFE00;
    addr -= offset;
    return oam[addr];
}

void PPU::oam_write(u16 addr, u8 val) {
    // std::cout << "Writing to OAM" << std::endl;
    // ppu_mode curr_mode = (ppu_mode)(io.get_STAT() & 0b11);
    // if (curr_mode == Mode_Drawing || curr_mode == Mode_OAM_Scan) {
    //     // std::cout << "PPU: Locked out of writing OAM\n";
    //     return; 
    // }
    
    u16 offset = 0xFE00;
    addr -= offset;
    oam[addr] = val;
}

void PPU::oam_scan() {
    // std::cout << "Doing OAM scan" << std::endl;

    // Mostly a sanity check: scanline rendering should always empty it out
    if (!sprite_buffer.empty()) {
        return;
    }

    sprite_size = BIT(io.get_LCDC(), 2);
    u8 height = sprite_size ? 16 : 8;

    // There are 40 sprites in OAM
    for (int sprite_i = 0; sprite_i < 40; sprite_i++) {
        
        // Grabbing sprite 
        u8 sprite_addr = 4 * sprite_i;
        u8 y_pos = oam[sprite_addr];
        // u8 x_pos = oam[sprite_addr+1];
        
        // std::cout << "Scanning sprite at addr: 0x" << std::hex << +(0xFE00+sprite_addr) << std::endl;
        
        // Push sprites that are hit by the current scanline
        bool add_sprite = ((io.get_LY() + 16) >= y_pos) && ((io.get_LY() + 16) < (y_pos + height));
        if (add_sprite) {
            // std::cout << "OAM Scan: Adding sprite at addr: 0x" << std::hex << +(0xFE00+sprite_addr)
            //     << " LY: " << std::dec << +io.get_LY()
            //     << " sprite x: " << +x_pos << " sprite y: " << +y_pos << " sprite height: " << +height << std::endl;
            
            sprite_buffer.push_back(sprite_addr);
        }

        // Stop once sprite buffer hits its limit
        if (sprite_buffer.size() >= sprite_limit) break;
    }
    
}

void PPU::print_oam() {
    u16 start = 0xFE00;
    std::cout << "0x" << std::hex << +start << ": ";
    for (int i = 0; i < 0xA0; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << +oam[i] << " ";
        if (i % 16 == 15 and i != 0) {
            std::cout << std::endl;
            start += 0x10;
            std::cout << "0x" << std::hex << +start << ": ";
        }
    }
}