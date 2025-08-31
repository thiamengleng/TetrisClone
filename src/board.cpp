#include <SDL3/SDL.h>
#include <vector>
#include "board.h"


Block::Block() : x(0), y(0) { // Default constructor
    color = {0,0,0, SDL_ALPHA_TRANSPARENT};
} 
Block::Block(int x, int y, SDL_Color color) : x(x), y(y), color(color) {}

Board::Board(int w, int h) : boardWidth(w), boardHeight(h) {
    grid.resize(boardHeight*3);
    for (auto& row: grid) {
        row.resize(boardWidth);
    }
}
Grid Board::GetGrid() {
    return grid;
}

BoardInfo Board::GetBoardInfo() {
    BoardInfo temp = std::make_pair(boardWidth, boardHeight);
    return temp;
}
// returns block on board that corresponds with center
Block Board::GetCorrespondingBlock(Block center) {
    if (center.y < 0 || center.x >= boardWidth || center.x < 0) {
        return Block(-147, -147, {0, 0, 0, SDL_ALPHA_TRANSPARENT}); //-147 snooker respect
    }
    return grid[center.y][center.x];
}

bool Board::SetBlockColor(int row, int column, SDL_Color color) {
    if (row > grid.size() or row < 0) {
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Row %d is out of range!", row);
        return false;
    }
    if (column > grid[row].size() or column < 0) {
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Column %d is out of range!", row);
        return false;
    }
    grid[row][column].x = column;
    grid[row][column].y = row;
    grid[row][column].color = color;
    return true;
}

void Board::ClearLines() { 
    for (int row = 0; row <= 28; row++) {
        if (ClearLine(row)) row -= 1;
    }
}

bool Board::ClearLine(int row) {
    for (int block_idx = 0; block_idx < grid[row].size(); block_idx++) {
        if (grid[row][block_idx].color.a == SDL_ALPHA_TRANSPARENT) {
            return false;
        }
    }
    //Shift all rows down  by 1
    for (int row_idx = row; row_idx < 32; row_idx++) {
        grid[row_idx] = std::move(grid[row_idx+1]);
        for (int block_idx = 0; block_idx < grid[row_idx].size(); block_idx++) {
            if (grid[row_idx][block_idx].color.a == SDL_ALPHA_OPAQUE) {
                grid[row_idx][block_idx].y -= 1;
            }
        }
    }

    grid[32] = std::vector<Block>(boardWidth, Block());
    return true;
}