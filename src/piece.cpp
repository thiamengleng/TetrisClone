#include <SDL3/SDL.h>
#include <vector>
#include <algorithm>

#include "piece.h"

namespace Colors {
    const SDL_Color blue = {66, 66, 199, SDL_ALPHA_OPAQUE};
    const SDL_Color red = {255, 0, 0, SDL_ALPHA_OPAQUE};
    const SDL_Color green = {0, 255, 0, SDL_ALPHA_OPAQUE};
    const SDL_Color orange = {255, 165, 0, SDL_ALPHA_OPAQUE};
    const SDL_Color cyan = {54, 255, 255, SDL_ALPHA_OPAQUE};
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

    const Offset clockwiseTestsI[4][4] = {
        {{1, 0}, {-2, 0}, {1, -2}, {-2, 1}},    //Anticlockwise->Spawn      / 3->0
        {{-2, 0}, {1, 0}, {-2, -1}, {1, 2}},    //Spawn->Clockwise          / 0->1
        {{-1, 0}, {2, 0}, {-1, 2}, {2, -1}},    //Clockwise->Upsidedown     / 1->2
        {{2, 0}, {-1, 0}, {2, 1}, {-1, -2}}     //Upsidedown->Anticlockwise / 2->3
    };

    const Offset anticlockwiseTestsI[4][4] = {
        {{2, 0}, {-1, 0}, {2, 1}, {-1, -2}},      //Clockwise->Spawn          / 1->0
        {{1, 0}, {-2, 0}, {1, -2}, {-2, 1}},  //Upsidedown->Clockwise     / 2->1
        {{-2, 0}, {1, 0}, {-2, -1}, {1, 2}},   //Anticlockwise->Upsidedown / 3->2
        {{-1, 0}, {2, 0}, {-1, 2}, {2, -1}}      //Spawn->Anticlockwise      / 0->3
    };
}

Piece::Piece() {
    m_piece.reserve(4);
    const std::unordered_map<PieceType, TetriminoData> pieceMap = Offsets::getPieceMap();
    auto it = pieceMap.find(PieceType::T);
    if (it == pieceMap.end()) {
        SDL_Log("Invalid piece type!\n");
        return;
    }

    const auto& data = it->second;
    const int centerX = 5;
    const int centerY = 21;
    for (const auto& [dx, dy] : data.offsets) {
        m_piece.emplace_back(centerX + dx, centerY + dy, data.color);
    }
    m_piece.shrink_to_fit();
    m_rotationState = Rotation::SPAWN;
}

Piece::Piece(PieceType type) : m_type(type) { 
    m_piece.reserve(4);
        
    const int centerX = 5;
    const int centerY = 21;

    const std::unordered_map<PieceType, TetriminoData> pieceMap = Offsets::getPieceMap();

    auto it = pieceMap.find(m_type);
    if (it == pieceMap.end()) {
        SDL_Log("Invalid piece type!\n");
        return;
    }

    const auto& data = it->second;
    for (const auto& [dx, dy] : data.offsets) {
        m_piece.emplace_back(centerX + dx, centerY + dy, data.color);
    }
    m_piece.shrink_to_fit();
    if (it->first == PieceType::I) {
        m_cx = static_cast<float>(centerX) + 0.5;
        m_cy = static_cast<float>(centerY) - 0.5;
    }
    m_rotationState = Rotation::SPAWN;
} 
/**
 * Resets a piece 
 * 
 * @param hold If the piece is a hold piece
 * @return
 */
void Piece::Reset() {
    const int centerX = 5;
    const int centerY = 21;

    const std::unordered_map<PieceType, TetriminoData> pieceMap = Offsets::getPieceMap();

    auto it = pieceMap.find(m_type);
    if (it == pieceMap.end()) {
        SDL_Log("Invalid piece type!\n");
        return;
    }

    const auto& data = it->second;
    for (int idx = 0; idx < m_piece.size(); idx++) {
        Offset offset = data.offsets[idx];
        m_piece[idx].x = centerX + offset.first;
        m_piece[idx].y = centerY + offset.second;
    }
    if (it->first == PieceType::I) {
        m_cx = static_cast<float>(centerX) + 0.5;
        m_cy = static_cast<float>(centerY) - 0.5;
    }
    
}
void Piece::Reset(int x, int y) {
    const int centerX = x;
    const int centerY = y;

    const std::unordered_map<PieceType, TetriminoData> pieceMap = Offsets::getPieceMap();

    auto it = pieceMap.find(m_type);
    if (it == pieceMap.end()) {
        SDL_Log("Invalid piece type!\n");
        return;
    }

    const auto& data = it->second;
    for (int idx = 0; idx < m_piece.size(); idx++) {
        Offset offset = data.offsets[idx];
        m_piece[idx].x = centerX + offset.first;
        m_piece[idx].y = centerY + offset.second;
    }
    
}
Tetrimino Piece::GetPiece() {
    return m_piece;
}
Tetrimino Piece::GetGhost() {
    return m_ghostPiece;
}
void Piece::Render(SDL_Renderer* renderer, Board *board) {
    if (m_placed) return;
    BoardInfo info = board->GetBoardInfo();
    int boardWidth = info.first;
    int boardHeight = info.second;
    const int boardStartX = 1920/2 - (BLOCK_WIDTH * boardWidth)/2, boardStartY = 1080 - 90 - BLOCK_HEIGHT * boardHeight;
    for (Block block: m_piece) {
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
/**
 * @brief Does not check for collisions and sets board color to piece colors
 * 
 * @param board Pointer to board that will be placed on
 * @return
 */
void Piece::Place(Board* board) {
    if (m_placed) return;
    for (Block& block: m_piece) {
        board->SetBlockColor(block.y, block.x, block.color);
    }

    board->ClearLines();
    m_placed = true;
    return;
}
/**
 * @brief Moves current piece down by 1 until it collides with a block or reaches bottom of the board
 * 
 * @param board Pointer to board that will be placed on
 * @return
 */
void Piece::SoftDrop(Board* board) {
    for (Block block: m_piece) {
        if (block.y <= 0) return; //0 is the lowest point lmao
    }
    Tetrimino tempTetrimino = m_piece;
    for (Block &block: tempTetrimino) {
        block.y -=1;
        Block boardBlock = board->GetCorrespondingBlock(block);

        if ( CheckBlockCollision(block, boardBlock, board->GetBoardInfo()) ) {
            return;
        }
    }
    for (Block &block: m_piece) {
        block.y -= 1;
    }
    if (m_type == PieceType::I) {
        m_cy -= 1.0;
    }
    
    return;
}
/**
 * @brief Moves current piece down until it collides with a block or reaches bottom of the board
 * 
 * @param board Pointer to board that will be placed on
 * @return
 */
void Piece::HardDrop(Board* board) {
    Tetrimino tempTetrimino = m_piece;
    for (int i = 0; i < 24; i++) {
        for (Block block: m_piece) {
            if (block.y <= 0) {
                Place(board);
                return;
            } //0 is the lowest point lmao
        }
        bool broke = false;
        for (Block &block: tempTetrimino) {
            block.y -=1;
            Block boardBlock = board->GetCorrespondingBlock(block);

            if ( CheckBlockCollision(block, boardBlock, board->GetBoardInfo()) ) {
                broke = true;
                break;
            }
        }
        if (broke) break;
        for (Block &block: m_piece) {
            block.y -= 1;
        }
        if (m_type == PieceType::I) {
            m_cy -= 1.0;
        }

    }
    Place(board);
    return;
}
void Piece::DropGhost(Board* board) {
    m_ghostPiece = m_piece;
    for (int i = 0; i < 24; i++) {
        for (Block block: m_ghostPiece) {
            if (block.y <= 0) {
                return;
            } //0 is the lowest point lmao
        }
        bool broke = false;
        for (Block &block: m_ghostPiece) {
            block.y -=1;
            Block boardBlock = board->GetCorrespondingBlock(block);

            if ( CheckBlockCollision(block, boardBlock, board->GetBoardInfo()) ) {
                broke = true;
            }
        }
        if (broke) {
            for (Block &block: m_ghostPiece) {
                block.y += 1;
            }
            break;
        }
    }
    return;
}
/**
 * @brief Moves current piece object horizontally
 * 
 * @param isLeft To see which direction
 * @param board Pointer to board that will be placed on
 * @return
 */
void Piece::Move(bool isLeft, Board* board) {
    //Check if collide with anything here 
    Tetrimino tempPiece = m_piece;
    BoardInfo info = board->GetBoardInfo();
    int width = info.first - 1;
    for (Block block: m_piece) {
        if ((block.x == 0 && isLeft) || (block.x >= width && !isLeft)) return; 
    }
    //Move
    for (Block &block: tempPiece) {
        block.x += isLeft ? -1 : 1;
        Block boardBlock = board->GetCorrespondingBlock(block);

        if (CheckBlockCollision(block, boardBlock, board->GetBoardInfo())) {
            return;
        }
    }
    for (Block &block: m_piece) {
        block.x += isLeft ? -1 : 1;
    }
    if (m_type == PieceType::I) {
        m_cx += isLeft ? -1.0 : 1.0;
    }
    DropGhost(board);
    return;
}
 /**
 * @brief Rotates current piece that is not placed on board
 * 
 * @param isCW If direction piece is rotated in clockwise
 * @param board Pointer to board that will be placed on
 * @return
 */
void Piece::Rotate(bool isCW, Board* board) { //recoded version
    
    if (m_type == PieceType::SINGLE || m_type == PieceType::O) {
        return; // No rotation for single or O piece
    } else {//if (m_type != PieceType::I) { //Check if piece collides with any other piece first in other functions
        Tetrimino tempTetri = m_piece;
        BoardInfo boardInfo = board->GetBoardInfo();
        bool doSRS = false;
        RotatePiece(tempTetri, tempTetri[m_centerBlockIndex], isCW);
        for (Block& block: tempTetri) {
            Block boardBlock = board->GetCorrespondingBlock(block);
            if (CheckBlockCollision(block, boardBlock, boardInfo)) {
                doSRS = true;
                break;
            }
        }
        if (doSRS) {
            if(!TestSRS(isCW, board, tempTetri)) return; //SRS test cases failed
        }
        m_rotationState = GetNextRotationState(m_rotationState, isCW);
        m_piece = std::move(tempTetri);
    }
    DropGhost(board);
    return;
}
/**
 * @brief Rotates a piece without matrix
 * @param piece Tetrimino to be rotated
 * @param center Center to be rotated about
 * @param isCCW If the direction to be rotated in is counter clockwise
 * @return
 */
inline void Piece::RotatePiece(Tetrimino& piece, const Block& center, bool isCW) {
    if (m_type == PieceType::I) {
        if (isCW) {
            for (Block& block: piece) {
                float dx = block.x - m_cx, dy = block.y - m_cy;
                block.x = static_cast<int>(dy + m_cx);
                block.y = static_cast<int>(-dx + m_cy);
            }
        } else {
            for (Block& block: piece) {
                float dx = block.x - m_cx, dy = block.y - m_cy;
                block.x = static_cast<int>(-dy + m_cx);
                block.y = static_cast<int>(dx + m_cy);
            }
        }
        return;
    }
    if (isCW) {
        for (Block& block: piece) {
            int dx = block.x - center.x, dy = block.y - center.y;
            block.x = dy + center.x;
            block.y = -dx + center.y;
        }
    } else {
        for (Block& block: piece) {
            int dx = block.x - center.x, dy = block.y - center.y;
            block.x = -dy + center.x;
            block.y = dx + center.y;
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

inline bool Piece::CheckBlockCollision(const Block& pieceBlock, const Block& gridBlock, const BoardInfo boardInfo) {
    bool outsideBoardWidth = pieceBlock.x < 0 || 
                                pieceBlock.x >= boardInfo.first || 
                                pieceBlock.y < 0;
    bool collision = pieceBlock.color.a != SDL_ALPHA_TRANSPARENT && gridBlock.color.a != SDL_ALPHA_TRANSPARENT;
    if (pieceBlock.color.a == SDL_ALPHA_TRANSPARENT) return false;
    //Invalid position
    if (collision || outsideBoardWidth) {
        return true;
    }
    return false;
}
bool Piece::TestSRS(bool isCW, Board* board, Tetrimino& piece) {
    const int MAX_TESTS = 4;

    for (int test = 0; test < MAX_TESTS; test++) {
        constexpr int center = 1;
        bool success = true;
        Offset testOffset;
        Rotation nextRotationState = GetNextRotationState(m_rotationState, isCW);
        BoardInfo boardInfo = board->GetBoardInfo();
        Tetrimino tempPiece = piece;
        if (m_type != PieceType::I) {
            testOffset = isCW ?                                  //putting comments here cuz short hand if statement are confuse
                Offsets::clockwiseTests[nextRotationState][test] :      //True
                Offsets::anticlockwiseTests[nextRotationState][test];   //False
        } else {
            testOffset = isCW ?
                Offsets::clockwiseTestsI[nextRotationState][test] :      //True
                Offsets::anticlockwiseTestsI[nextRotationState][test];   //False
        }
        for (Block& block: tempPiece) {
            block.x += testOffset.first;
            block.y += testOffset.second;

            Block boardBlock = board->GetCorrespondingBlock(block);
            if (CheckBlockCollision(block, boardBlock, boardInfo)) {
                success = false;
                break;
            }
        }

        if (!success) continue;

        piece = std::move(tempPiece);
        if (m_type == PieceType::I) {
            m_cx += static_cast<float>(testOffset.first);
            m_cy += static_cast<float>(testOffset.second);
        }
        return true;
    }
    return false;
}