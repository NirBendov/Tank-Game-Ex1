#include "Moveable.h"
#include "../constants/BoardConstants.h"

Moveable::Moveable(int lc[2], int d[2])
{
    // copy initial location and direction
    info.location[0] = lc[0];
    info.location[1] = lc[1];
    info.dir[0]      = d[0];
    info.dir[1]      = d[1];
}

void Moveable::move()
{
    using namespace BoardConstants;
    
    // X-axis movement with wrap-around
    info.location[0] += info.dir[0];
    if (info.location[0] < 0) {
        info.location[0] = BOARD_WIDTH - 1;
    } else if (info.location[0] >= BOARD_WIDTH) {
        info.location[0] = 0;
    }

    // Y-axis movement with wrap-around
    info.location[1] += info.dir[1];
    if (info.location[1] < 0) {
        info.location[1] = BOARD_HEIGHT - 1;
    } else if (info.location[1] >= BOARD_HEIGHT) {
        info.location[1] = 0;
    }
}

Moveable::~Moveable()
{
    // nothing to clean up
}
