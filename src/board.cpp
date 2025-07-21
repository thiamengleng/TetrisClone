#include <SDL3/SDL.h>
#include <vector>
#include "board.h"


Block::Block() : x(0), y(0) { // Default constructor
    color = {0,0,0, SDL_ALPHA_TRANSPARENT};
} 
Block::Block(int x, int y, SDL_Color color) : x(x), y(y), color(color) {}

inline void DrawRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color WHITE) {
    static SDL_FRect tempRect;
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    tempRect.x = x;
    tempRect.y = y;
    tempRect.w = w;
    tempRect.h = h;
    SDL_RenderRect(renderer, &tempRect);
}
inline void DrawRectFilled(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color WHITE) {
    static SDL_FRect tempRect;
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    tempRect.x = x;
    tempRect.y = y;
    tempRect.w = w;
    tempRect.h = h;
    SDL_RenderFillRect(renderer, &tempRect);
}

Board::Board(int w, int h) : boardWidth(w), boardHeight(h) {
    grid.resize(boardHeight*3);
    for (auto& row: grid) {
        row.resize(boardWidth);
    }
}

BoardInfo Board::GetBoardInfo() {
    BoardInfo temp = std::make_pair(boardWidth, boardHeight);
    return temp;
}

void Board::DrawBoard(SDL_Renderer* renderer) {
    SDL_FRect tempRect;
    const SDL_Color WHITE = {255, 255, 255, 255};
    const SDL_Color BLACK = {0, 0, 0, 255};
    
    //Bottom of the board is at index 0 of grid vector
    int boardStartX = 1920/2 - (BLOCK_WIDTH * boardWidth)/2, boardStartY = 1080 - 90 - BLOCK_HEIGHT * boardHeight;
    //Draw Board Outline
    DrawRect(renderer, boardStartX, boardStartY, BLOCK_WIDTH * boardWidth, BLOCK_HEIGHT * boardHeight, WHITE);
    for (size_t row = 0; row < boardHeight; ++row) { //Grid Squares
        for (size_t col = 0; col < boardWidth; ++col) {
            //SDL_Log()
            const SDL_Color color = {160, 160, 160, 130};
            int posX = boardStartX + col*BLOCK_WIDTH;
            int posY = row*BLOCK_HEIGHT + boardStartY;
            DrawRect(renderer, posX, posY, BLOCK_WIDTH, BLOCK_HEIGHT, color);
        }
    }
    //Draw Actual Pieces placed down
    for (size_t row = 0; row < grid.size(); ++row) { //Actual pieces
        for (size_t col = 0; col < grid[row].size(); ++col) {
            const Block& block = grid[row][col];
            if (block.color.a == SDL_ALPHA_TRANSPARENT) continue;
            int posX = boardStartX + block.x*BLOCK_WIDTH;
            int posY = boardStartY + (boardHeight - 1 - block.y) * BLOCK_HEIGHT;
            SDL_Log("StartPos: %d, %d", boardStartX, boardStartY);
            SDL_Log("Position: %d, %d", posX, posY);
            SDL_Log("Indice: %d, %d", block.x, block.y);
            //SDL_Log()
            DrawRectFilled(renderer, posX, posY, BLOCK_WIDTH, BLOCK_HEIGHT, block.color);
        }
    }
}

Grid Board::GetSurrounding(Block center) { // For everything except I piece, I piece will be in another func
    Grid surroundingBlocks;
    size_t rowMin = std::max(0, center.y - 1);
    size_t rowMax = std::min(static_cast<int>(grid.size()) - 1, center.y + 1);
    surroundingBlocks.resize(3);
    for (int row = rowMin; row <= rowMax; ++row) {
        int colMin = std::max(0, center.x - 1);
        int colMax = std::min(static_cast<int>(grid[row].size()) - 1, center.x + 1);
        for (int col = colMin; col <= colMax; ++col) {
            const Block& block = grid[row][col];
            surroundingBlocks[row - rowMin].push_back(block);
        }
    }
    return surroundingBlocks;
}

bool Board::SetBlockColor(int row, int column, SDL_Color color) {
    grid[row][column].x = column;
    grid[row][column].y = row;
    grid[row][column].color = color;
    return true;
}