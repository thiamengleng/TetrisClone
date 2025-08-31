#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "input.h"
#include "piece.h"
#include "randomiser.h"
#include "render.h"

int main() { //Main thread

    
    const int windowHeight = 1080, windowWidth = 1920;
    const int bWidth=10, bHeight=20;
    bool holded = false, firsthold = true;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Tetris Clone", windowWidth, windowHeight, 0, &window, &renderer);

    Board board(bWidth, bHeight);
    Randomiser picker; //buggy
    Piece piec = picker.Get();
    Piece held;
    TetrisRender gameRender(windowWidth, windowHeight, renderer, window);
    SDL_Log(SDL_GetBasePath());
    piec.DropGhost(&board);
    while(true) {
        //Render
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        gameRender.RenderBoard(&board);
        if (!firsthold) gameRender.RenderHold(&held, holded);
        gameRender.RenderQueue(&picker);
        //board.DrawBoard(renderer);
        //gameRender.RenderPiece(&piec);
        piec.Render(renderer, &board);
        gameRender.RenderGhost(&piec);
        SDL_RenderPresent(renderer);
        
        SDL_PollEvent(&event);
        
        switch (event.type) {
            case SDL_EVENT_QUIT:
            {
                SDL_Quit();
                break;
            }
            case SDL_EVENT_KEY_DOWN: //make this a seperate function in input.cpp later cuz need to implement DAS and ARR lulz
            {
                if (event.key.key == SDLK_A) {
                    piec.Rotate(true, &board);
                    piec.Rotate(true, &board);
                }
                if (event.key.key == SDLK_Z) {
                    piec.Rotate(false, &board);
                }
                if (event.key.key == SDLK_UP) {
                    piec.Rotate(true, &board);
                }
                if (event.key.key == SDLK_LEFT) {
                    piec.Move(true, &board);
                }
                if (event.key.key == SDLK_DOWN) {
                    piec.SoftDrop(&board);
                }
                if (event.key.key == SDLK_RIGHT) {
                    piec.Move(false, &board);
                }
                if (event.key.key == SDLK_S) {
                    piec.Place(&board);
                    piec = picker.Get();
                }
                if (event.key.key == SDLK_SPACE) {
                    piec.HardDrop(&board);
                    piec = picker.Get();
                    piec.DropGhost(&board);
                    holded = false;
                }
                if (event.key.key == SDLK_LSHIFT && holded == false) {
                    if (firsthold){
                        held = std::move(piec);
                        piec = picker.Get();
                        holded = true;
                        firsthold = false;
                    } else {
                        std::swap(held, piec);
                        piec.Reset();
                        piec.DropGhost(&board);
                        holded = true;
                    }
                }
                break;

                
            }
            case SDL_EVENT_KEY_UP:
            {

            }
        }
    }
}