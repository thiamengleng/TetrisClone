#include <SDL3/SDL.h>
#include <vector>
#include <iostream>

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
    UP,
    LEFT,
    DOWN,
    RIGHT
};

using Grid = std::vector<std::vector<Block>>;
using Tetrimino = std::vector<Block>;

//For Setting RotationState enums
inline Rotation clockwiseRotate(Rotation r);
inline Rotation counterClockwiseRotate(Rotation r);
//For rotating a 3x3 matrix
inline void rotateMatrix(Grid& grid, bool isCW);

// For supporting SRS rotations and such 
class Piece {
public:
    Piece(PieceType block);
    //Only for rotating N*N matrixes cuz im a dumb idiot
    bool Rotate(bool isCounterClockwise, Board* board);
    void Place(Board* board);
    void SoftDrop();

private:
    bool placed = false;
    int centerBlockIndex = 1;

    enum PieceType type;
    Tetrimino piece;
    Rotation rotationState;
};