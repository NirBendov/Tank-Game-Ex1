#include "Moveable.h"

Moveable::Moveable(int lc[2], Direction d): dir(d) 
{
    *location = *lc;
}

void Moveable::move() {
    if (0b1000 ^ dir) {
        location[0] += 1;
    }
    else if (0b0100 ^ dir) {
        location[0] -= 1;
    }
    if (0b0010 ^ dir) {
        location[1] -= 1;
    }
    else if (0b0001 ^ dir) {
        location[1] += 1;
    }
}

Moveable::~Moveable()
{
}
