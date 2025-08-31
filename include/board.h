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

class Board {
    public:
        Board(int w, int h);
        bool SetBlockColor(int row, int column, SDL_Color color);
        void ClearLines(void);
        Block GetCorrespondingBlock(Block center);
        BoardInfo GetBoardInfo();
        Grid GetGrid();
    private:
        int boardWidth, boardHeight;
        Grid grid;

        bool ClearLine(int row);
};