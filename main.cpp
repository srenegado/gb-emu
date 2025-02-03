#include "cpu.h"

int main(int argc, char** argv) {
    CPU cpu;

    int running = 1;
    while (running) {
        cpu.step();
    }
    
    return 0;
}