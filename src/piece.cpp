#include <SDL3/SDL.h>
#include <vector>
#include <algorithm>

#include "piece.h"

namespace Colors {
    const SDL_Color blue = {20, 20, 180, SDL_ALPHA_OPAQUE};
    const SDL_Color red = {255, 0, 0, SDL_ALPHA_OPAQUE};
    const SDL_Color green = {0, 255, 0, SDL_ALPHA_OPAQUE};
    const SDL_Color orange = {255, 165, 0, SDL_ALPHA_OPAQUE};
    const SDL_Color cyan = {20, 20, 255, SDL_ALPHA_OPAQUE};
    const SDL_Color pink = {255, 16, 240, SDL_ALPHA_OPAQUE};
    const SDL_Color yellow = {255, 255, 0, SDL_ALPHA_OPAQUE};
    const SDL_Color gray = {150, 150, 150, SDL_ALPHA_OPAQUE};
}

namespace Offsets {
    const std::unordered_map<PieceType, TetriminoData>& getPieceMap() {
        static const std::unordered_map<PieceType, TetriminoData> pieceMap = {
            { PieceType::I, {Colors::cyan,   {{-1, 0}, {0, 0}, {1, 0}, {2, 0}}} },
            { PieceType::Z, {Colors::red,    {{-1, 1}, {0, 0}, {0, 1}, {1, 0}}} },
            { PieceType::S, {Colors::green,  {{-1, 0}, {0, 0}, {0, 1}, {1, 1}}} },
            { PieceType::L, {Colors::orange, {{-1, 0}, {0, 0}, {1, 0}, {1, 1}}} },
            { PieceType::J, {Colors::blue,   {{-1, 1}, {0, 0}, {-1, 0}, {1, 0}}} },
            { PieceType::T, {Colors::pink,   {{-1, 0}, {0, 0}, {1, 0}, {0, 1}}} },
            { PieceType::O, {Colors::yellow, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}} },
            { PieceType::SINGLE, {Colors::gray, {{0, 0}}} }
        };
        return pieceMap;
    }

    const Offset clockwiseTests[4][4] = {
        {{-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},   //Anticlockwise->Spawn      / 3->0
        {{-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},  //Spawn->Clockwise          / 0->1
        {{1, 0}, {1, -1}, {0, 2}, {1, 2}},      //Clockwise->Upsidedown     / 1->2
        {{1, 0}, {1, 1}, {0, -2}, {1, -2}}      //Upsidedown->Anticlockwise / 2->3
    };

    const Offset anticlockwiseTests[4][4] = {
        
        {{1, 0}, {1, -1}, {0, 2}, {1, 2}},      //Clockwise->Spawn          / 1->0
        {{-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},  //Upsidedown->Clockwise     / 2->1
        {{-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},   //Anticlockwise->Upsidedown / 3->2
        {{1, 0}, {1, 1}, {0, -2}, {1, -2}}      //Spawn->Anticlockwise      / 0->3
    };
}

Piece::Piece(PieceType block) : type(block) { 
    piece.reserve(4); //What in the shit is ts hardcoded shitty code
        
    const int centerX = 5;
    const int centerY = 21;

    const std::unordered_map<PieceType, TetriminoData> pieceMap = Offsets::getPieceMap();

    auto it = pieceMap.find(type);
    if (it == pieceMap.end()) {
        SDL_Log("Invalid piece type!\n");
        return;
    }

    const auto& data = it->second;
    for (const auto& [dx, dy] : data.offsets) {
        piece.emplace_back(centerX + dx, centerY + dy, data.color);
    }
    piece.shrink_to_fit();
} 

void Piece::Render(SDL_Renderer* renderer, Board *board) {
    if (placed) return;
    BoardInfo info = board->GetBoardInfo();
    int boardWidth = info.first;
    int boardHeight = info.second;
    const int boardStartX = 1920/2 - (BLOCK_WIDTH * boardWidth)/2, boardStartY = 1080 - 90 - BLOCK_HEIGHT * boardHeight;
    for (Block block: piece) {
        SDL_FRect tempRect;

        SDL_SetRenderDrawColor(renderer, block.color.r, block.color.g, block.color.b, block.color.a);
        tempRect.x = boardStartX + block.x*BLOCK_WIDTH;
        tempRect.y = boardStartY + (boardHeight - 1 - block.y) * BLOCK_HEIGHT;;
        tempRect.w = BLOCK_WIDTH;
        tempRect.h = BLOCK_HEIGHT;
        SDL_RenderFillRect(renderer, &tempRect);
    }
    
    return;
}

void Piece::Place(Board* board) {
    if (placed) return;
    for (Block& block: piece) {
        board->SetBlockColor(block.y, block.x, block.color);
    }
    placed = true;
    return;
}

void Piece::SoftDrop(Board* board) {
    for (Block block: piece) {
        if (block.y <= 0) return; //0 is the lowest point lmao
    }
    for (Block& block: piece) {
        block.y -=1;
    }
    return;
}
void Piece::Move(bool isLeft, Board* board) {
    //Check if collide with anything here 
    BoardInfo info = board->GetBoardInfo();
    int width = info.first - 1;
    for (Block block: piece) {
        if ((block.x == 0 && isLeft) || (block.x >= width && !isLeft)) return; 
    }
    //Move
    for (Block& block: piece) {
        block.x += isLeft ? -1 : 1;
    }
    return;
}
 /**
 * @brief Rotates current piece that is not placed on board
 * 
 * @param isCW If direction piece is rotated in clockwise
 * @param board Pointer to board that will be placed on
 * @return
 */
void Piece::Rotate(bool isCW, Board* board) { //delete this and recode in the future or recode
    
    if (type == PieceType::SINGLE || type == PieceType::O) {
        return; // No rotation for single or O piece
    } else if (type != PieceType::I) { //Check if piece collides with any other piece first in other functions
        const int center = 1; //Middle of 3x3 matrix row and column
        Grid temp;
        Grid surroundings = board->GetSurrounding(piece[centerBlockIndex]);
        BoardInfo boardInfo = board->GetBoardInfo();
        bool doSRS = false;

        temp.resize(3);
        for (std::vector<Block> &row : temp) {
            row.resize(3);
        }
        //setting block cords in 3x3 temp matrixx for some reason
        for (Block block : piece) {
            int offsetX = block.x - piece[centerBlockIndex].x, offsetY = block.y - piece[centerBlockIndex].y;
            temp[center+offsetY][center+offsetX] = block;
        }
        rotateMatrix(temp, !isCW); //Rotate Matrix cuz i suck at math
        //Write checking if it collide code here
        doSRS = CheckCollisions(temp, surroundings, board->GetBoardInfo());
        if (doSRS) {
            if(!TestSRS(isCW, board, temp)) return;
        }
        rotationState = GetNextRotationState(rotationState, isCW);
        int w_index = 0;
        for (size_t row = 0; row < temp.size(); row++) {
            for (size_t col = 0; col < temp[row].size(); col++) {
                int x = col-center;
                int y = row-center;
                if (temp[row][col].color.a == SDL_ALPHA_TRANSPARENT) continue;
                if (x == 0 && y == 0) centerBlockIndex = w_index;
                piece[w_index].x = temp[center][center].x + x;
                piece[w_index].y = temp[center][center].y + y;
                w_index++;
            }
        }
    } else {
        return;
    }
    return;
}
/**
 * @brief Returns next rotation state in enum depending on direction
 * Since tetris clockwise rotations actually rotate in the opposite direction,
 * the original isCW parameter is changed to isCCW
 * @param grid A reference to the grid to be rotated
 * @param isCCW If the direction to be rotated in is counter clockwise
 * @return
 */
inline void rotateMatrix(Grid& grid, bool isCCW) {
    constexpr int center = 1;
    for (size_t i = 0; i < grid.size(); ++i) {
        for (size_t j = i+1; j < grid[i].size(); ++j) {
            std::swap(grid[i][j], grid[j][i]);
        }
    }
    if (isCCW) {
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
    for (size_t row = 0; row < grid.size(); row++) {
        for (size_t col = 0; col < grid[row].size(); col++) {
            int x = col-center;
            int y = row-center;
            if (grid[row][col].color.a == SDL_ALPHA_TRANSPARENT) continue;
            grid[row][col].x = grid[center][center].x+x;
            grid[row][col].y = grid[center][center].y+y;
        }
    }
}
/**
 * @brief Returns next rotation state in enum depending on direction
 * 
 * @param r The current rotation state as a enum
 * @param isCW If the next direction is clockwise
 * @return Next rotation state defined in Rotation enum 
 */
inline Rotation Piece::GetNextRotationState(Rotation r, bool isCW) {
    constexpr int ROTATIONS = 4;
    Rotation clockwise = static_cast<Rotation>((static_cast<int>(r) + 1) % ROTATIONS);
    Rotation antiClockwise = static_cast<Rotation>((static_cast<int>(r) - 1 + ROTATIONS) % ROTATIONS);
    r = isCW ? clockwise : antiClockwise;
    return r;
}
/**
 * @brief To check if pieceMatrix and surroundings have collisions with their pieces/blocks
 * @note Make sure pieceMatrix and surroundings have the same size
 * 
 * @param pieceMatrix A const reference to the current grid containing the piece positions
 * @param surroundings A const reference to a grid that is at the same position as pieceMatrix on the board
 * @param boardInfo Information of board height and width in blocks
 * @return Returns true if collision else false
 */
inline bool Piece::CheckCollisions(const Grid &pieceMatrix, const Grid &surroundings, const BoardInfo boardInfo) {
    for (size_t row = 0; row < pieceMatrix.size(); row++) {
        for (size_t col = 0; col < pieceMatrix[row].size(); col++) {
            bool outsideBoardWidth = pieceMatrix[row][col].x < 0 || 
                                     pieceMatrix[row][col].x >= boardInfo.first || 
                                     pieceMatrix[row][col].y < 0;
            bool collision = pieceMatrix[row][col].color.a != SDL_ALPHA_TRANSPARENT && surroundings[row][col].color.a != SDL_ALPHA_TRANSPARENT;
            if (pieceMatrix[row][col].color.a == SDL_ALPHA_TRANSPARENT) continue;
            //Invalid position
            if (collision || outsideBoardWidth) {
                return true;
            }
        }
    }
    return false;
}
/**
 * @brief Offset each block by specified offset
 * 
 * @param offseted The grid that is to be offset
 * @param offset A pair of x and y offsets in number of blocks
 * @return
 */
inline void Piece::OffsetGridBlocks(Grid& offseted, Offset offset) {
    for (size_t row = 0; row < offseted.size(); row++) {
            for (size_t col = 0; col < offseted[row].size(); col++) {
                if (offseted[row][col].color.a == SDL_ALPHA_TRANSPARENT) continue;
                    offseted[row][col].x += offset.first;
                    offseted[row][col].y += offset.second;
            }
        }
}

bool Piece::TestSRS(bool isCW, Board* board, Grid& pieceMatrix) {
    constexpr int MAX_TESTS = 4;

    for (int test = 0; test < MAX_TESTS; test++) {
        constexpr int center = 1;
        Rotation nextRotationState = GetNextRotationState(rotationState, isCW);
        BoardInfo boardInfo = board->GetBoardInfo();
        Grid surroundings, offseted;
        Offset testOffset = isCW ?                                  //putting comments here cuz short hand if statement are confuse
            Offsets::clockwiseTests[nextRotationState][test] :      //True
            Offsets::anticlockwiseTests[nextRotationState][test];   //False
        offseted = pieceMatrix;
        OffsetGridBlocks(offseted, testOffset);
        surroundings = board->GetSurrounding(offseted[center][center]);
        if (CheckCollisions(offseted, surroundings, boardInfo)) continue;
        pieceMatrix = offseted;
        return true;
    }
    return false;
}