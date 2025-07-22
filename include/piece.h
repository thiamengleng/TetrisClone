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
    inline bool CheckCollisions(Grid pieceMatrix, Grid& pieceSurroundings, BoardInfo boardiInfo);
    bool TestSRS(bool isCW, Board* board, Grid& pieceMatrix);
};