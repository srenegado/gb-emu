#include "ppu.h"
#include "memory.h"
#include "cpu.h"

int main(int argc, char** argv) {
    
    // std::freopen("log.txt","w",stdout);

    // Setup Game Boy components
    Cartridge cart;
    IO io;
    PPU ppu(io);
    MemoryBus bus(cart, io, ppu);
    CPU cpu(bus);

    // Load game ROM
    char *ROM = argv[1];
    if (!cart.load_rom(ROM)) {
        std::cout << "ROM could not be loaded\n";
        return -1;
    } 

    // Main emulation loop
    bool running = true;
    while (running) {
        
        // Handle input events (so far, quit emulator if user pressed esc)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case (SDL_KEYDOWN):
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        running = false;
                    }
                    break;
            }
        }

        // Fetch, decode, and execute an instruction
        if (!cpu.step()) {
            std::cout << "CPU could not step\n";
            return -2;
        }
        
    }
    
    return 0;
}