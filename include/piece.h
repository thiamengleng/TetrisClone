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

//For rotating a 3x3 matrix
inline void rotateMatrix(Grid& grid, bool isCCW);

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

// For supporting SRS rotations and such 
class Piece {
public:
    Piece(PieceType block);
    //Only for rotating N*N matrixes cuz im a dumb idiot
    void Move(bool isLeft, Board* board);
    void Rotate(bool isCounterClockwise, Board* board);
    void Render(SDL_Renderer* renderer, Board* board);
    void Place(Board* board);
    void SoftDrop(Board* board);

private:
    bool placed = false;
    int centerBlockIndex = 1;

    enum PieceType type;
    Tetrimino piece;
    Rotation rotationState = SPAWN;
private:
    inline Rotation GetNextRotationState(Rotation r, bool CW);
    inline bool CheckCollisions(const Grid &pieceMatrix, const Grid &pieceSurroundings, const BoardInfo boardiInfo);
    inline void OffsetGridBlocks(Grid& pieceMatrix, Offset offset);
    bool TestSRS(bool isCW, Board* board, Grid& pieceMatrix);
};