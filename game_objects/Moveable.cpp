#include "Moveable.h"
#include "../constants/BoardConstants.h"
#include <iostream>

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
    // Store the previous position
    prevPosition[0] = info.location[0];
    prevPosition[1] = info.location[1];
    
    // Print movement information
    std::cout << "Moving from (" << prevPosition[0] << "," << prevPosition[1] << ") to (";
    
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
    
    // Complete the movement print
    std::cout << info.location[0] << "," << info.location[1] << ")" << std::endl;
}

void Moveable::moveBack() {
    // Simply swap current position with previous position
    std::swap(info.location, prevPosition);
}