#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "board.h"

enum PieceType { //SINGLE means single tetrimino piece for fun
    I,
    Z,
    S,
    L,
    J,
    T,
    O,
    SINGLE
};
enum Rotation { //For SRS rotations cuz they matter 
    SPAWN = 0,
    CLOCKWISE = 1,
    UPSIDEDOWN = 2,
    ANTICLOCKWISE = 3
};

using Grid = std::vector<std::vector<Block>>;
using Tetrimino = std::vector<Block>;
using Offset = std::pair<int, int>;

struct TetriminoData {
    SDL_Color color;
    std::vector<Offset> offsets;
};

namespace Colors {
    extern const SDL_Color blue;
    extern const SDL_Color red;
    extern const SDL_Color green;
    extern const SDL_Color orange;
    extern const SDL_Color cyan;
    extern const SDL_Color pink;
    extern const SDL_Color yellow;
    extern const SDL_Color gray;
}

namespace Offsets {

    extern const std::unordered_map<PieceType, TetriminoData>& getPieceMap();
    
    extern const Offset clockwiseTests[4][4];

    extern const Offset anticlockwiseTests[4][4];
}

/*
TODO: 
rename class variable members as m_variable
add board as a member
*/
// For supporting SRS rotations and such 

class Piece {
public:
    Piece();
    Piece(PieceType block);
    void Move(bool isLeft, Board* board);
    void Rotate(bool isCW, Board* board);
    void Reset();
    void Reset(int centerX, int centerY);
    virtual void Render(SDL_Renderer* renderer, Board* board);
    void Place(Board* board);
    void SoftDrop(Board* board);
    void HardDrop(Board* board);
    void DropGhost(Board* board);
    void Render(Board* board);
    Tetrimino GetPiece();
    Tetrimino GetGhost();

protected:
    bool m_placed = false;
    int m_centerBlockIndex = 1;
    float m_cx, m_cy; //SPECIAL FOR I PIECE

    enum PieceType m_type;
    Tetrimino m_piece;
    Tetrimino m_ghostPiece;
    Rotation m_rotationState = SPAWN;
protected:
//helper funcs
    inline Rotation GetNextRotationState(Rotation r, bool CW);
    inline bool CheckBlockCollision(const Block& pieceBlock, const Block& boardBlock, const BoardInfo boardInfo);
    inline void RotatePiece(Tetrimino& piece, const Block& center, bool isCCW);
    bool TestSRS(bool isCW, Board* board, Tetrimino& piece);
    
};

class HoldPiece : public Piece {
public: 
    void Render(SDL_Renderer* renderer, Board* board) override;
};