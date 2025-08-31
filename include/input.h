#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <unordered_map>
#include <functional>

#include "piece.h"
#include "randomiser.h"

using MS = int8_t;

typedef struct Keybinds {
    SDL_Keycode MoveLeft = SDLK_LEFT;
    SDL_Keycode MoveRight = SDLK_RIGHT;
    SDL_Keycode ClockwiseRotate = SDLK_UP;
    SDL_Keycode CounterclockwiseRotate = SDLK_Z;
    SDL_Keycode SoftDrop = SDLK_DOWN;
    SDL_Keycode HardDrop = SDLK_SPACE;
    SDL_Keycode Flip = SDLK_A;

    bool HoldRotate = false;
} Keybinds;

class Input {
public:

    Input(Keybinds bindings, Board *board);
    ~Input();
    
    void SetARR(MS ms);
    void SetDAS(MS ms);
    void ChangeKeybinds(Keybinds bindings);
    void PassEvent(SDL_Event* event);

private:
    MS m_arr = 0;               //Auto Repeat Rate in MS
    MS m_das = 6;              //Delayed Auto Shift in MS
    int m_blockPerDrop = 1000;  //Infinite Softdrop speed
    bool m_running = true;
    
    Keybinds m_bindings; 
    Randomiser m_randomiser;
    Piece m_currentPiece;
    Board *m_attachedBoard;

    std::unordered_map<SDL_Keycode, bool> m_keyHold;
    std::unordered_map<SDL_Keycode, std::function<void()>> m_pieceFunctions; //finish this
    std::unordered_map<SDL_Keycode, bool> m_dasRunning;

    SDL_Event *m_currentEvent;
    SDL_Thread *m_inputThread;
    SDL_Thread *m_dasThread;
    SDL_Mutex *m_inputMutex;
private:
    static int InputThreadEntry(void *data);
    static int DasThreadEntry(void *data);
    int InputThread(void *data);
    int DasThread(void *data);
};