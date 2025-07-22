#include <SDL3/SDL.h>
#include <vector>
#include <algorithm>

#include "piece.h"

Piece::Piece(PieceType block) : type(block) { 
    piece.reserve(4); //What in the shit is ts hardcoded shitty code

    const std::unordered_map<PieceType, TetriminoData> pieceMap = {
        { PieceType::I, {Colors::cyan, {{-1, 0}, {0, 0}, {1, 0}, {2, 0}}} },
        { PieceType::Z, {Colors::red,   {{-1, 1}, {0, 0}, {0, 1}, {1, 0}}} },
        { PieceType::S, {Colors::green,   {{-1, 0}, {0, 0}, {0, 1}, {1, 1}}} },
        { PieceType::L, {Colors::orange, {{-1, 0}, {0, 0}, {1, 0}, {1, 1}}} },
        { PieceType::J, {Colors::blue,   {{-1, 1}, {0, 0}, {-1, 0}, {1, 0}}} },
        { PieceType::T, {Colors::pink,{{-1, 0}, {0, 0}, {1, 0}, {0, 1}}} },
        { PieceType::O, {Colors::yellow, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}} },
        { PieceType::SINGLE, {Colors::gray, {{0, 0}}} }
    };
        
    const int centerX = 5;
    const int centerY = 21;

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

void Piece::Rotate(bool isCW, Board* board) { //delete this and recode in the future or recode
    
    if (type == PieceType::SINGLE || type == PieceType::O) {
        return; // No rotation for single or O piece
    } else if (type != PieceType::I) { //Check if piece collides with any other piece first in other functions
        int center = 1; //Middle of 3x3 matrix row and column
        const int centerX = piece[centerBlockIndex].x, centerY = piece[centerBlockIndex].y;
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
        for (size_t row = 0; row < temp.size(); row++) {
            
            for (size_t col = 0; col < temp[row].size(); col++) {
                bool outsideBoardWidth = temp[row][col].x < 0 || 
                                         temp[row][col].x >= boardInfo.first || 
                                         temp[row][col].y < 0;
                bool collision = temp[row][col].color.a != SDL_ALPHA_TRANSPARENT && surroundings[row][col].color.a != SDL_ALPHA_TRANSPARENT;
                if (temp[row][col].color.a == SDL_ALPHA_TRANSPARENT) continue;
                //Invalid position detected
                if (collision || outsideBoardWidth) {
                    doSRS = true;
                    break;
                }
            }
        }
        if (doSRS) {
            bool res = TestSRS(isCW, board, temp);
            if (!res) return;
        }
        setRotationState(&rotationState, isCW);
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

inline void Piece::setRotationState(Rotation *r, bool isCW) {
    constexpr int ROTATIONS = 4;
    Rotation clockwise = static_cast<Rotation>((static_cast<int>(*r) + 1) % ROTATIONS);
    Rotation antiClockwise = static_cast<Rotation>((static_cast<int>(*r) - 1 + ROTATIONS) % ROTATIONS);
    *r = isCW ? clockwise : antiClockwise;
    return;
}

bool Piece::TestSRS(bool isCW, Board* board, Grid& pieceMatrix) {
    constexpr int MAX_TESTS = 4;
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

    constexpr int ROTATIONS = 4;
    Rotation clockwise = static_cast<Rotation>((static_cast<int>(rotationState) + 1) % ROTATIONS);
    Rotation antiClockwise = static_cast<Rotation>((static_cast<int>(rotationState) - 1 + ROTATIONS) % ROTATIONS);

    for (int test = 0; test < MAX_TESTS; test++) {
        bool invalid = false;
        constexpr int center = 1;
        BoardInfo boardInfo = board->GetBoardInfo();
        Grid surroundings, temp;
        Offset testOffset = isCW ? clockwiseTests[clockwise][test] : anticlockwiseTests[antiClockwise][test];
        temp = pieceMatrix;
        for (size_t row = 0; row < temp.size(); row++) {
            for (size_t col = 0; col < temp[row].size(); col++) {
                if (temp[row][col].color.a == SDL_ALPHA_TRANSPARENT) continue;
                    temp[row][col].x += testOffset.first;
                    temp[row][col].y += testOffset.second;
            }
        }
        surroundings = board->GetSurrounding(temp[center][center]);
        for (size_t row = 0; row < temp.size(); row++) {
            if (invalid) break;
            for (size_t col = 0; col < temp[row].size(); col++) {
                bool outsideBoardWidth = temp[row][col].x < 0 || 
                                         temp[row][col].x >= boardInfo.first || 
                                         temp[row][col].y < 0;
                bool collision = temp[row][col].color.a != SDL_ALPHA_TRANSPARENT && surroundings[row][col].color.a != SDL_ALPHA_TRANSPARENT;
                if (temp[row][col].color.a == SDL_ALPHA_TRANSPARENT) continue;
                //Invalid position detected
                if (collision || outsideBoardWidth) {
                    invalid = true;
                    break;
                }
            }
        }
        if (invalid) continue;
        pieceMatrix = temp;
        return true;
    }
    return false;
}