#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "common.h"
#include "SDL.h"

class EventHandler {
    private:
        bool quit = false;
    public:
        EventHandler();
        ~EventHandler();
        void handle_events();
        bool quit_requested();
};

#endif 