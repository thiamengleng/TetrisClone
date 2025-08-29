#include "randomiser.h"

Randomiser::Randomiser() {
    type = Bag::BagType::Bag;
    randomRange = std::uniform_int_distribution<int>(0, Bag::bagSizes.find(type)->second - 1);
    RefreshBag();
}

Randomiser::Randomiser(Bag::BagType bagType) 
    : type(bagType), 
      randomRange(std::uniform_int_distribution<int>(0, Bag::bagSizes.find(type)->second - 1)) {
    RefreshBag();
}

Piece Randomiser::Get() {
    if (bag.empty()) RefreshBag();
    while(queue.size() < Queue::DEFAULT_QUEUE_SIZE) {
        if (bag.empty()) RefreshBag();
        queue.push_back(bag.back());
        bag.pop_back();
    }
    Piece next = queue.front();
    queue.pop_front();
    return next;
}

inline void Randomiser::RefreshBag() {
    bag.clear();
    switch(type) {
        case (Bag::BagType::Bag): {
            bag = Bag::bag;
            break;
        }
        case (Bag::BagType::BagAddOne): {
            bag = Bag::bag;
            bag.push_back(Bag::bag[GetNum()]);
            break;
        }
        case (Bag::BagType::BagAddTwo): {
            bag = Bag::bag;
            bag.push_back(Bag::bag[GetNum()]);
            bag.push_back(Bag::bag[GetNum()]);
            break;
        }
        case (Bag::BagType::Classic): {
            bag.reserve(Bag::bagSizes.at(type));
            for (int start = 0; start < Bag::bagSizes.at(type); start++) {
                bag.push_back(Bag::bag[GetNum()]);
            }
            break;
        }
        default: {
            SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Invalid Bag type!");
        }
    }
    bag_rindex = 0;
}

inline int Randomiser::GetNum() {
    return randomRange(generator);
}