#include "event_handler.h"

EventHandler::EventHandler(Joypad &joypad_, IO &io_) 
: joypad(joypad_), io(io_) {}
EventHandler::~EventHandler() {}

void EventHandler::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case (SDL_QUIT): 
                quit = true; 
                break;
            case (SDL_KEYDOWN):
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        quit = true;
                        break;
                    case SDL_SCANCODE_UP:
                        joypad.update(Dpad_Up, true);
                        break;
                    case SDL_SCANCODE_DOWN:
                        joypad.update(Dpad_Down, true);
                        break;
                    case SDL_SCANCODE_LEFT:
                        joypad.update(Dpad_Left, true);
                        break;
                    case SDL_SCANCODE_RIGHT:
                        joypad.update(Dpad_Right, true);
                        break;
                    case SDL_SCANCODE_S:
                        joypad.update(Button_A, true);
                        break;
                    case SDL_SCANCODE_A:
                        joypad.update(Button_B, true);
                        break;
                    case SDL_SCANCODE_RETURN:
                        joypad.update(Button_Start, true);
                        break;
                    case SDL_SCANCODE_RSHIFT:
                        joypad.update(Button_Select, true);
                        break;
                } 
                break;    
            case (SDL_KEYUP):
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_UP:
                        joypad.update(Dpad_Up, false);
                        break;
                    case SDL_SCANCODE_DOWN:
                        joypad.update(Dpad_Down, false);
                        break;
                    case SDL_SCANCODE_LEFT:
                        joypad.update(Dpad_Left, false);
                        break;
                    case SDL_SCANCODE_RIGHT:
                        joypad.update(Dpad_Right, false);
                        break;
                    case SDL_SCANCODE_S:
                        joypad.update(Button_A, false);
                        break;
                    case SDL_SCANCODE_A:
                        joypad.update(Button_B, false);
                        break;
                    case SDL_SCANCODE_RETURN:
                        joypad.update(Button_Start, false);
                        break;
                    case SDL_SCANCODE_RSHIFT:
                        joypad.update(Button_Select, false);
                        break;
                } 
                break;       
        }
    }
}

bool EventHandler::quit_requested() {
    return quit;
}