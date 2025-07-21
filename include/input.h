#pragma once
#include <SDL3/SDL.h>
#include <vector>

#include "piece.h"

using MS = int;

class Input {
public:

    void SetARR(MS ms);
    void SetDAS(MS);

private:
    MS ARR = 0;
    MS DAS = 60;
};