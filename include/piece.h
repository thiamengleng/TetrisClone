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

//For Setting RotationState enums
//inline void setRotationState(Rotation* r, bool CW);
//For rotating a 3x3 matrix
inline void rotateMatrix(Grid& grid, bool isCCW);

struct TetriminoData {
    SDL_Color color;
    std::vector<Offset> offsets;
};

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

    inline void setRotationState(Rotation* r, bool CW);
    bool testSRS(bool isCW, Board* board, Grid& pieceMatrix);
};