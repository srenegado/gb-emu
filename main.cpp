#include "memory.h"
#include "cpu.h"

int main(int argc, char** argv) {
    
    // std::freopen("log.txt","w",stdout);

    // Setup Game Boy components
    Cartridge cart;
    Joypad joypad;
    IO io(joypad);
    EventHandler event_handler(joypad, io);
    PPU ppu(io, event_handler);
    MemoryBus bus(cart, io, ppu);
    CPU cpu(bus);

    // Load game ROM
    char *ROM = argv[argc - 1];
    if (!cart.load_rom(ROM)) {
        std::cout << "ROM could not be loaded\n";
        return -1;
    } 

    // Grab SAV filename if supplied
    char *SAV = nullptr;
    int opt;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
            case 's': SAV = optarg; break;
        }
    }

    // Load game SAV file when supported
    switch (cart.get_type()) {
        case 0x03: // MBC1+RAM+BATTERY
        case 0x13: // MBC3+RAM+BATTERY
            if (!cart.load_state(SAV)) 
                std::cout << "SAV file could not be loaded\n";
    }
    
    // Main emulation loop
    while (!event_handler.quit_requested()) {
    
        // Fetch, decode, and execute an instruction
        if (!cpu.step()) {
            std::cout << "CPU could not step\n";
            return -2;
        }
       
    }

    // Create a SAV file when supported
    switch (cart.get_type()) {
        case 0x03: // MBC1+RAM+BATTERY
        case 0x13: // MBC3+RAM+BATTERY
            if (!cart.save_state(SAV))
                std::cout << "Game state could not be saved\n";
    }

    // ppu.print_vram();
    // ppu.print_oam();
    
    return 0;
}