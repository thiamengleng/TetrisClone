#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <utility>

#define BLOCK_WIDTH 44
#define BLOCK_HEIGHT 44

struct Block {
    int x, y;
    SDL_Color color;
    Block(); // Default constructor
    Block(int x, int y, SDL_Color color);
};

//2D matrix for pieces
using Grid = std::vector<std::vector<Block>>;
using BoardInfo = std::pair<int, int>;

inline void DrawRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color);
inline void DrawRectFilled(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color);

class Board {
    public:
        Board(int w, int h);
        bool SetBlockColor(int row, int column, SDL_Color color);
        void DrawBoard(SDL_Renderer* renderer);
        Grid GetSurrounding(Block center);
        Block GetCorrespondingBlock(Block center);
        BoardInfo GetBoardInfo();
    private:
        int boardWidth, boardHeight;
        Grid grid;
};