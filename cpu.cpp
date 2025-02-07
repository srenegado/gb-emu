#include "cpu.h"

CPU::CPU(MemoryBus &bus_) : bus(bus_), instrs(regs) {}
CPU::~CPU() {}

bool CPU::step() {
    
    if (!halted) {

        // Fetch opcode
        u8 opcode = bus.read(regs.PC++); 
        std::cout << "Fetching opcode: 0x" << std::hex << +opcode << std::endl;

        std::cout << "Current PC = 0x" << std::hex << regs.PC << std::endl;
        
        // Decode and execute opcode
        std::cout << "Decoding opcode...\n";
        switch (opcode) {
            
            case 0x00: break; // NOP

            default: 
                std::cout << "Unknown opcode: unable to decode into an instruction\n";
                return false;
        }
    
    }
    
    return true;
}