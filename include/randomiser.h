#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <random>
#include <unordered_map>
#include <algorithm>
#include <deque>

#include "piece.h"

namespace Queue {
    const int DEFAULT_QUEUE_SIZE = 6;
}

namespace Bag {
    const int DEFAULT_BAG_SIZE = 7;

    enum BagType {
        Bag,
        BagAddOne,
        BagAddTwo,
        Classic
    };

    static const std::vector<Piece> bag = {
        Piece(PieceType::I),//I
        Piece(PieceType::J),
        Piece(PieceType::L),
        Piece(PieceType::O),
        Piece(PieceType::S),
        Piece(PieceType::Z),
        Piece(PieceType::T)
    };

    static const std::unordered_map<BagType, int> bagSizes = {
        {BagType::Bag, DEFAULT_BAG_SIZE},
        {BagType::BagAddOne, DEFAULT_BAG_SIZE+1},
        {BagType::BagAddTwo, DEFAULT_BAG_SIZE+2},
        {BagType::Classic, DEFAULT_BAG_SIZE}
    };
};

class Randomiser {
public:
    Randomiser();
    Randomiser(Bag::BagType bagType);

    Piece Get();
    std::deque<Piece> GetQueue();
private:
    int bag_rindex = 0;

    //std::vector<Piece> queue{};
    std::deque<Piece> queue{};
    std::vector<Piece> bag{};
    Bag::BagType type;

    std::mt19937 generator{std::random_device{}()}; 
    std::uniform_int_distribution<int> randomRange;
private:
    inline int GetNum();
    inline void RefreshBag();
};