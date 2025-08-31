#include "input.h"

Input::Input(Keybinds keys, Board *board) 
    :m_bindings(keys), m_attachedBoard(board)
{
    m_keyHold = {
        {keys.MoveLeft, false},
        {keys.MoveRight, false},
        {keys.ClockwiseRotate, false}, 
        {keys.CounterclockwiseRotate, false}, 
        {keys.SoftDrop, false}, 
        {keys.HardDrop, false}, 
        {keys.Flip, false}
    };
    m_dasRunning = {
        {keys.MoveLeft, false},
        {keys.MoveRight, false},
        {keys.SoftDrop, false}
    };
    m_pieceFunctions = {
        {keys.MoveLeft, [this](){ m_currentPiece.Move(true, m_attachedBoard); }},
        {keys.MoveRight, [this](){ m_currentPiece.Move(false, m_attachedBoard); }},
        {keys.SoftDrop, [this](){ m_currentPiece.SoftDrop(m_attachedBoard); }},
        {keys.ClockwiseRotate, [this](){ m_currentPiece.Rotate(false, m_attachedBoard); }},
        {keys.CounterclockwiseRotate, [this](){ m_currentPiece.Rotate(true, m_attachedBoard); }}
    };
    m_inputThread = SDL_CreateThread(InputThreadEntry, "InputThread", this);
    m_dasThread = SDL_CreateThread(DasThreadEntry, "DASThread", this);
    m_inputMutex = SDL_CreateMutex();
}

Input::~Input() {
    m_running = false;
    SDL_WaitThread(m_inputThread, NULL);
    SDL_WaitThread(m_dasThread, NULL);

}

 int Input::DasThread(void *data) {
    while(m_running) {
        SDL_LockMutex(m_inputMutex);

        SDL_Keycode key = m_currentEvent->key.key;
        bool goingBothDirections = m_keyHold.at(m_bindings.MoveLeft) && m_keyHold.at(m_bindings.MoveRight);

        for (auto key : m_keyHold) {
            if (!m_dasRunning.at(key.first)) continue;
            if ((key.first == m_bindings.MoveLeft ||  key.first == m_bindings.MoveRight) && goingBothDirections) continue;
            auto mappedFunction = m_pieceFunctions.find(key.first);

            if (!key.second || mappedFunction == m_pieceFunctions.end()) continue;

            mappedFunction->second();
        }
        SDL_UnlockMutex(m_inputMutex);
        SDL_Delay(m_das);
    }
    return 0;
}

int Input::InputThread(void *data) {
    while(m_running) {
        SDL_LockMutex(m_inputMutex);
        switch(m_currentEvent->type) {
            case SDL_EVENT_KEY_DOWN: {
                SDL_Keycode currentKey = m_currentEvent->key.key;
                auto keyFound = m_keyHold.find(currentKey);
                if (!(keyFound == m_keyHold.end() || keyFound->second == true)) {
                    keyFound->second = true;
                }
                break;
            }

            case SDL_EVENT_KEY_UP: {
                SDL_Keycode currentKey = m_currentEvent->key.key;
                auto keyFound = m_keyHold.find(currentKey);

                if (keyFound == m_keyHold.end()) 
                    break;
                if (currentKey == m_bindings.MoveLeft || currentKey == m_bindings.MoveRight
                    || currentKey == m_bindings.SoftDrop) 
                {//disable das
                    auto dasFound = m_dasRunning.find(currentKey);
                    if (dasFound != m_dasRunning.end())
                        dasFound->second = false;
                }

                break;
            }
        }
        SDL_UnlockMutex(m_inputMutex);
    }
    return 0;
}

int Input::InputThreadEntry(void *data) {
    return static_cast<Input*>(data)->InputThread(NULL);
}

int Input::DasThreadEntry(void *data) {
    return static_cast<Input*>(data)->DasThread(NULL);
}

void Input::PassEvent(SDL_Event *event) {
    SDL_LockMutex(m_inputMutex);
    m_currentEvent = event;
    SDL_UnlockMutex(m_inputMutex);
}