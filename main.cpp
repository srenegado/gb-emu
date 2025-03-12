#include "ppu.h"
#include "memory.h"
#include "cpu.h"

int main(int argc, char** argv) {
    
    // std::freopen("log.txt","w",stdout);

    // Setup Game Boy components
    Cartridge cart;
    IO io;
    EventHandler event_handler;
    PPU ppu(io, event_handler);
    MemoryBus bus(cart, io, ppu);
    CPU cpu(bus);

    // Load game ROM
    char *ROM = argv[1];
    if (!cart.load_rom(ROM)) {
        std::cout << "ROM could not be loaded\n";
        return -1;
    } 

    // Main emulation loop
    while (!event_handler.quit_requested()) {
    
        // Fetch, decode, and execute an instruction
        if (!cpu.step()) {
            std::cout << "CPU could not step\n";
            return -2;
        }
       
    }

    // ppu.print_vram();
    
    return 0;
}