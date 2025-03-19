#include "joypad.h"

Joypad::Joypad() {}
Joypad::~Joypad() {}

u8 Joypad::read() {
    u8 output;
    if (!dpad_select) {
        output = 0xE0 | (down << 3) | (up << 2) | (left << 1) | right;
    } else if (!buttons_select) {
        output = 0xD0 | (start << 3) | (select << 2) | (b << 1) | a;
    } else {
        output = 0xFF;
    }
    // std::cout << "Reading joypad: 0x" << std::hex << +output << std::endl;
    return output;
}

void Joypad::write(u8 val) {
    // std::cout << "Writing joypad: 0x" << std::hex << +val << std::endl;
    buttons_select = BIT(val, 5);
    dpad_select = BIT(val, 4);
}

void Joypad::update(joypad_button button, bool pressed) {
    // A button being pressed is seen as the corresponding bit being 0, not 1
    switch (button) {
        case Dpad_Up: up = !pressed; break;
        case Dpad_Down: down = !pressed; break;
        case Dpad_Left: left = !pressed; break;
        case Dpad_Right: right = !pressed; break;
        case Button_A: a = !pressed; break;
        case Button_B: b = !pressed; break;
        case Button_Start: start = !pressed; break;
        case Button_Select: select = !pressed; break;
    }
}