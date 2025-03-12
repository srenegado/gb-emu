#include "event_handler.h"

EventHandler::EventHandler() {}
EventHandler::~EventHandler() {}

void EventHandler::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case (SDL_QUIT): quit = true; break;
            case (SDL_KEYDOWN):
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    quit = true;
                break;
        }
    }
}

bool EventHandler::quit_requested() {
    return quit;
}