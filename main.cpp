#include "cpu.h"
#include "cart.h"
#include "memory.h"
#include "cpu_util.h"

int main(int argc, char** argv) {
    
    // Setup Game Boy components
    Cartridge cart;
    MemoryBus bus(cart);
    CPU cpu(bus);

    // Load game ROM
    char *ROM = argv[1];
    if (!cart.load_rom(ROM)) {
        std::cout << "ROM could not be loaded\n";
        return -1;
    } 

    // Main emulation loop
    int running = 1;
    while (running) {
        if (!cpu.step()) {
            std::cout << "CPU could not step\n";
            return -2;
        }
    }
    
    return 0;
}