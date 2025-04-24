#include "Moveable.h"

Moveable::Moveable(int lc[2], int d[2])
{
    *info->location = *lc;
    *info->dir = *d;
}

void Moveable::move() {
    if (info->dir[0] == 1) {
        info->location[0] += 1;
    }
    else if (info->dir[0] == -1) {
        info->location[0] -= 1;
    }
    if (info->dir[1] == -1) {
        info->location[1] -= 1;
    }
    else if (info->dir[1] == 1) {
        info->location[1] += 1;
    }
}

Moveable::~Moveable()
{
}
