#include <SDL3/SDL.h>
#include <vector>
#include <algorithm>

#include "piece.h"

Piece::Piece(PieceType block) : type(block) { 
    piece.reserve(4);
    switch(block) { //What in the shit is ts hardcoded shitty code
        case (PieceType::I): { //?? No center cuz fuck I pieces
            const SDL_Color blue = {20, 20, 180, SDL_ALPHA_OPAQUE};
            piece.push_back(Block(4, 22, blue));
            piece.push_back(Block(5, 22, blue));
            piece.push_back(Block(6, 22, blue));
            piece.push_back(Block(7, 22, blue));
            break;
        }
        case (PieceType::Z): { // Index 2 is center
            //At the top of the board 
            const SDL_Color red = {255, 0, 0, SDL_ALPHA_OPAQUE};
            piece.push_back(Block(4, 23, red));
            piece.push_back(Block(5, 23, red));
            piece.push_back(Block(5, 22, red));
            piece.push_back(Block(6, 22, red));
            break;
        }
        case (PieceType::S): { // Index 2 is center
            const SDL_Color green = {0, 255, 0, SDL_ALPHA_OPAQUE};
            piece.push_back(Block(4, 22, green));
            piece.push_back(Block(5, 22, green));
            piece.push_back(Block(5, 23, green));
            piece.push_back(Block(6, 23, green));
            break;
        }
        case (PieceType::L): { // Index 2 is center
            const SDL_Color orange = {20, 20, 255, SDL_ALPHA_OPAQUE};
            piece.push_back(Block(4, 22, orange));
            piece.push_back(Block(5, 22, orange));
            piece.push_back(Block(6, 22, orange));
            piece.push_back(Block(6, 23, orange));
            break;
        }
        case (PieceType::J): { // Index 2 is center
            const SDL_Color blue = {20, 20, 255, SDL_ALPHA_OPAQUE};
            piece.push_back(Block(4, 22, blue));
            piece.push_back(Block(5, 22, blue));
            piece.push_back(Block(5, 21, blue));
            piece.push_back(Block(6, 21, blue));
            break;
        }
        case (PieceType::T): { // Index 2 is center
            const SDL_Color pink = {255, 16, 240, SDL_ALPHA_OPAQUE};
            piece.push_back(Block(4, 22, pink));
            piece.push_back(Block(5, 22, pink));
            piece.push_back(Block(5, 23, pink));
            piece.push_back(Block(6, 22, pink));
            break;
        }
        case (PieceType::O): { // Index 2 is center
            const SDL_Color yellow = {0, 155, 155, SDL_ALPHA_OPAQUE};
            piece.push_back(Block(5, 22, yellow));
            piece.push_back(Block(5, 23, yellow));
            piece.push_back(Block(6, 22, yellow));
            piece.push_back(Block(6, 23, yellow));
            break;
        }
        case (PieceType::SINGLE): {
            const SDL_Color gray = {150, 150, 150, SDL_ALPHA_OPAQUE};
            piece.push_back(Block(4, 22, gray));
            piece.shrink_to_fit();
            break;
        }
        default: {
            SDL_Log("end yourself\n");
        }
    }
} 

void Piece::Place(Board* board) {
    if (placed) return;
    for (Block& block: piece) {
        board->SetBlockColor(block.y, block.x, block.color);
    }
    placed = true;
    return;
}

void Piece::SoftDrop() {
    for (Block& block: piece) {
        block.y -=1;
    }
    return;
}

bool Piece::Rotate(bool isCW, Board* board) {
    rotationState = isCW ? clockwiseRotate(rotationState) : counterClockwiseRotate(rotationState);
    if (type == PieceType::SINGLE || type == PieceType::O) {
        return true; // No rotation for single or O piece
    } else if (type != PieceType::I) { //Check if piece collides with any other piece first in other functions
        int center = 1; //Middle of 3x3 matrix row and column
        const int centerX = piece[centerBlockIndex].x, centerY = piece[centerBlockIndex].y;
        Grid temp;
        Grid surroundings = board->GetSurrounding(piece[centerBlockIndex]);

        temp.resize(3);
        for (std::vector<Block> &row : temp) {
            row.resize(3);
        }

        for (Block block : piece) {
            int offsetX = block.x - piece[centerBlockIndex].x, offsetY = block.y - piece[centerBlockIndex].y;
            temp[center+offsetY][center+offsetX] = block;
        }
        rotateMatrix(temp, isCW);
        //Write checking if it collide code here

        int w_index = 0;
        //Doesnt check yet its just writing values to piece
        for (size_t row = 0; row < temp.size(); row++) {
            for (size_t col = 0; col < temp[row].size(); col++) {
                int x = col-center;
                int y = row-center;
                if (temp[row][col].color.a == SDL_ALPHA_TRANSPARENT) continue;
                if (x == 0 && y == 0) centerBlockIndex = w_index;
                piece[w_index].x = centerX + x;
                piece[w_index].y = centerY + y;
                w_index++;
            }
        }
    } else {
        return true;
    }
    return true;
}

inline void rotateMatrix(Grid& grid, bool isCW) {
    for (size_t i = 0; i < grid.size(); ++i) {
        for (size_t j = i+1; j < grid[i].size(); ++j) {
            std::swap(grid[i][j], grid[j][i]);
        }
    }
    if (isCW) {
        for (std::vector<Block> &row : grid) {
            std::reverse(row.begin(), row.end());
        }
    } else {
        for (size_t col = 0; col < grid.size(); ++col) {
            for (size_t row = 0, end = grid.size()-1; row < end; ++row, --end) {
                std::swap(grid[row][col], grid[end][col]);
            }
        }
    }
}

inline Rotation clockwiseRotate(Rotation r) {
    constexpr int ROTATIONS = 4;
    return static_cast<Rotation>((static_cast<int>(r) + 1) % ROTATIONS);
}
inline Rotation counterClockwiseRotate(Rotation r) {
    constexpr int ROTATIONS = 4;
    return static_cast<Rotation>((static_cast<int>(r) - 1 + ROTATIONS) % ROTATIONS);
}