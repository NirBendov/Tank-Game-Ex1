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

std::array<int, 2> Moveable::calculateMove() const {
    using namespace BoardConstants;
    std::array<int, 2> newLocation = {info.location[0], info.location[1]};
    
    // X-axis movement with wrap-around
    newLocation[0] += info.dir[0];
    if (newLocation[0] < 0) {
        newLocation[0] = BOARD_WIDTH - 1;
    } else if (newLocation[0] >= BOARD_WIDTH) {
        newLocation[0] = 0;
    }

    // Y-axis movement with wrap-around 
    newLocation[1] += info.dir[1];
    if (newLocation[1] < 0) {
        newLocation[1] = BOARD_HEIGHT - 1;
    } else if (newLocation[1] >= BOARD_HEIGHT) {
        newLocation[1] = 0;
    }

    return newLocation;
}

void Moveable::move() {
    // Store the previous position
    prevPosition[0] = info.location[0];
    prevPosition[1] = info.location[1];
    
    // Print movement information
    std::cout << "Moving from (" << prevPosition[0] << "," << prevPosition[1] << ") to (";
    
    // Calculate and apply new position
    std::array<int, 2> newLocation = calculateMove();
    info.location[0] = newLocation[0];
    info.location[1] = newLocation[1];
    
    // Complete the movement print
    std::cout << info.location[0] << "," << info.location[1] << ")" << std::endl;
}

std::array<int, 2> Moveable::potentialMove() const {
    return calculateMove();
}

void Moveable::moveBack() {
    // Simply swap current position with previous position
    std::swap(info.location, prevPosition);
}