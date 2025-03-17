#ifndef JOYPAD_H
#define JOYPAD_H

#include "common.h"

typedef enum {
    Dpad_Up,
    Dpad_Down,
    Dpad_Left,
    Dpad_Right,
    Button_A,
    Button_B,
    Button_Start,
    Button_Select
} joypad_button;

class Joypad {
    private:
        bool buttons_select = 0;
        bool dpad_select = 0;
        u8 up = 1;
        u8 down = 1;
        u8 left = 1;
        u8 right = 1;
        u8 a = 1;
        u8 b = 1;
        u8 start = 1;
        u8 select = 1;
    public:
        Joypad();
        ~Joypad();
        u8 read();
        void write(u8 val);
        void update(joypad_button button, bool pressed);
        
};

#endif