#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cmath>

#include "piece.h"

void RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius, int segments, SDL_FColor color);

int main() { //Main thread
    const int windowHeight = 1080, windowWidth = 1920;
    const int bWidth=10, bHeight=20;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Event event;

    Board board(10, 20);

    Piece piec(PieceType::T);
    piec.SoftDrop();
    piec.SoftDrop();


    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Tetris Clone", windowWidth, windowHeight, 0, &window, &renderer);
    while(true) {
        //Render
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        SDL_RenderClear(renderer);
        board.DrawBoard(renderer);
        SDL_RenderPresent(renderer);
        
        SDL_WaitEvent(&event);
        
        switch (event.type) {
            case SDL_EVENT_QUIT:
            {
                SDL_Quit();
                break;
            }
            case SDL_EVENT_KEY_DOWN:
            {
                if (event.key.key == SDLK_S) {
                    piec.Rotate(false, &board);
                    piec.Rotate(false, &board);
                }
                

                piec.Place(&board);
            }
            case SDL_EVENT_KEY_UP:
            {

            }
        }
    }
}

void RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius, int segments, SDL_FColor color) { //rewrite this crappy gpt code
    if (segments < 3) return; // Need at least 3 segments to form a circle

    int vertex_count = segments + 1; // Center + perimeter points
    int index_count = segments * 3;  // 3 indices per triangle

    // Allocate memory for positions and colors
    float positions[vertex_count*2];
    int indices[index_count];
    SDL_FColor colors[vertex_count];
    //float *positions = (float*)malloc(sizeof(float) * vertex_count * 2); // x and y for each vertex
    //SDL_FColor *colors = (SDL_FColor*)malloc(sizeof(SDL_FColor) * vertex_count);
    //int *indices = (int*)malloc(sizeof(int) * index_count);


    // Center vertex
    positions[0] = cx;
    positions[1] = cy;
    colors[0] = color;

    // Perimeter vertices
    for (int i = 0; i < segments; ++i) {
        float angle = (float)i / segments * 2.0f * M_PI;
        float x = cx + radius * cosf(angle);
        float y = cy + radius * sinf(angle);
        positions[(i + 1) * 2] = x;
        positions[(i + 1) * 2 + 1] = y;
        colors[i + 1] = color;
    }

    // Indices for triangle fan
    for (int i = 0; i < segments; ++i) {
        indices[i * 3] = 0;               // Center vertex
        indices[i * 3 + 1] = i + 1;       // Current perimeter vertex
        indices[i * 3 + 2] = (i + 2 > segments) ? 1 : i + 2; // Next perimeter vertex, wrap around
    }

    // Render the geometry
    SDL_RenderGeometryRaw(renderer,
                          NULL, // No texture
                          positions, sizeof(float) * 2,
                          colors, sizeof(SDL_FColor),
                          NULL, 0, // No texture coordinates
                          vertex_count,
                          indices, index_count,
                          sizeof(int));
}