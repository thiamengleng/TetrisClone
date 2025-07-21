#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "input.h"
#include "piece.h"

int main() { //Main thread
    const int windowHeight = 1080, windowWidth = 1920;
    const int bWidth=10, bHeight=20;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Event event;

    Board board(bWidth, bHeight);

    Piece piec(PieceType::Z);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Tetris Clone", windowWidth, windowHeight, 0, &window, &renderer);
    while(true) {
        //Render
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        SDL_RenderClear(renderer);
        board.DrawBoard(renderer);
        piec.Render(renderer, &board);
        SDL_RenderPresent(renderer);
        
        SDL_WaitEvent(&event);
        
        switch (event.type) {
            case SDL_EVENT_QUIT:
            {
                SDL_Quit();
                break;
            }
            case SDL_EVENT_KEY_DOWN: //make this a seperate function in input.cpp later cuz need to implement DAS and ARR lulz
            {
                if (event.key.key == SDLK_A) {
                    piec.Rotate(false, &board);
                    piec.Rotate(false, &board);
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
                }
                break;

                
            }
            case SDL_EVENT_KEY_UP:
            {

            }
        }
    }
}