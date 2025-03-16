#include "io.h"

IO::IO(Joypad &joypad_) : joypad(joypad_) {}
IO::~IO() {}

u8 IO::read(u16 addr) {
    if (addr == 0xFF00) {
        return joypad.read();

    } else if (addr == 0xFF01) {
        // std::cout << "Reading from SB!" << std::endl;
        return serial_data[0];

    } else if (addr == 0xFF02) {
        // std::cout << "Reading from SC!" << std::endl;
        return serial_data[1];

    } else if (0xFF04 <= addr && addr <= 0xFF07) {
        // Reading timers
        return timer.read(addr);

    } else if (addr == 0xFF0F) {
        // Reading Interrupt flags (IF)
        return IF;

    } 

    // LCD-related registers
    else if (addr == 0xFF40) {
        // Reading LCD control
        return LCDC;

    } else if (addr == 0xFF41) {
        // Reading LCD status
        return STAT;

    } else if (addr == 0xFF42) {
        // Reading Background viewport Y
        return SCY;

    } else if (addr == 0xFF43) {
        // Reading Background viewport X
        return SCX;

    } else if (addr == 0xFF44) {
        // Reading LCD Y coordinate
        return LY;

    } else if (addr == 0xFF45) {
        // Reading LY compare
        return LYC;

    } else if (addr == 0xFF47) {
        // Reading Background palette
        return BGP;
    
    } else if (addr == 0xFF48) {
        // Reading Sprite palette 0
        return OBP0;
    
    } else if (addr == 0xFF49) {
        // Reading Sprite palette 1
        return OBP1;

    } else if (addr == 0xFF4A) {
        // Reading Window pos Y
        return WY;

    } else if (addr == 0xFF4B) {
        // Reading Window pos X plus 7
        return WX;

    }

    else if (addr >= 0xFF10 && addr <= 0xFF26) {
        // Haven't support audio
    }

    else if (addr >= 0xFF30 && addr <= 0xFF3F) {
        // Haven't support audio
    }
    
    else {
        std::cout << "Unsupported bus read at: 0x" << std::hex << addr << std::endl;

    }

    return 0;
}

void IO::write(u16 addr, u8 val) {
    if (addr == 0xFF00) {
        joypad.write(val);

    } else if (addr == 0xFF01) {
        // std::cout << "Writing to SB!" << std::endl;
        serial_data[0] = val;

    } else if (addr == 0xFF02) {
        // std::cout << "Writing to SC!" << std::endl;
        serial_data[1] = val;

    } else if (0xFF04 <= addr && addr <= 0xFF07) {
        // Writing to timers
        timer.write(addr, val);

    } else if (addr == 0xFF0F) {
        // Writing to Interrupt flags (IF)
        IF = val;
        
    } 
    
    // LCD-related registers
    else if (addr == 0xFF40) {
        // Writing to LCD control
        LCDC = val;

    } else if (addr == 0xFF41) {
        // Writing to LCD status
        STAT = val;
        
    } else if (addr == 0xFF42) {
        // Writing to Background viewport Y
        SCY = val;
        
    } else if (addr == 0xFF43) {
        // Writing to Background viewport X
        SCX = val;
        
    } else if (addr == 0xFF45) {
        // Writing to LY compare
        LYC = val;

    } else if (addr == 0xFF47) {
        // Writing to Background palette
        BGP = val;

    } else if (addr == 0xFF48) {
        // Writing to Sprite palette 0
        OBP0 = val;

    } else if (addr == 0xFF49) {
        // Writing to Sprite palette 1
        OBP1 = val;

    } else if (addr == 0xFF4A) {
        // Writing to Window pos Y
        WY = val;

    } else if (addr == 0xFF4B) {
        // Writing to Window pos X plus 7
        WX = val;

    }

    else if (addr >= 0xFF10 && addr <= 0xFF26) {
        // Haven't support audio
        
    }

    else if (addr >= 0xFF30 && addr <= 0xFF3F) {
        // Haven't support audio
        
    }
    
    else {
        std::cout << "Unsupported bus write at: 0x" << std::hex << addr << std::endl;
    }
}

u8 IO::get_IF() {
    return IF;
}

void IO::set_IF(u8 val) {
    IF = val;
}

u8 IO::get_IE() {
    return IE;
}

void IO::set_IE(u8 val) {
    IE = val;
}

u8 IO::get_LCDC() {
    return LCDC;
}

void IO::set_LCDC(u8 val) {
    LCDC = val;
}

u8 IO::get_STAT() {
    return STAT;
}

void IO::set_STAT(u8 val) {
    STAT = val;
}

u8 IO::get_SCY() {
    return SCY;
}

void IO::set_SCY(u8 val) {
    SCY = val;
}

u8 IO::get_SCX() {
    return SCX;
}

void IO::set_SCX(u8 val) {
    SCX = val;
}


u8 IO::get_LY() {
    return LY;
}

void IO::set_LY(u8 val) {
    LY = val;
}

u8 IO::get_LYC() {
    return LYC;
}

void IO::set_LYC(u8 val) {
    LYC = val;
}

u8 IO::get_BGP() {
    return BGP;
}

void IO::set_BGP(u8 val) {
    BGP = val;
}

u8 IO::get_OBP0() {
    return OBP0;
}

void IO::set_OBP0(u8 val) {
    OBP0 = val;
}

u8 IO::get_OBP1() {
    return OBP1;
}

void IO::set_OBP1(u8 val) {
    OBP1 = val;
}

u8 IO::get_WY() {
    return WY;
}

void IO::set_WY(u8 val) {
    WY = val;
}

u8 IO::get_WX() {
    return WX;
}

void IO::set_WX(u8 val) {
    WX = val;
}

bool IO::timer_tick() {
    return timer.tick();
}