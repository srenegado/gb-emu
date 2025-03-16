#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "common.h"
#include "joypad.h"
#include "io.h"
#include "SDL.h"

class EventHandler {
    private:
        bool quit = false;
        Joypad &joypad;
        IO &io;
    public:
        EventHandler(Joypad &joypad_, IO &io_);
        ~EventHandler();
        void handle_events();
        bool quit_requested();
};

#endif 