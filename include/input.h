#pragma once
#include <SDL3/SDL.h>
#include <vector>

#include "piece.h"

using MS = int8_t;

typedef struct Keybinds {
    SDL_Keycode MoveLeft = SDLK_LEFT;
    SDL_Keycode Moveright = SDLK_RIGHT;
    SDL_Keycode ClockwiseRotate = SDLK_UP;
    SDL_Keycode CounterclockwiseRotate = SDLK_Z;
    SDL_Keycode SoftDrop = SDLK_DOWN;
    SDL_Keycode HardDrop = SDLK_SPACE;
    SDL_Keycode Flip = SDLK_A;

    bool HoldRotate = false;
} Keybinds;

class Input {
public:

    Input(Keybinds bindings);

    void SetARR(MS ms);
    void SetDAS(MS ms);

private:
    MS ARR = 0;             //Auto Repeat Rate in MS
    MS DAS = 60;            //Delayed Auto Shift in MS
    int pxPerDrop = 1000;   //Infinite Softdrop speed

    SDL_Thread* input_thread; 
    Keybinds bindings;
};