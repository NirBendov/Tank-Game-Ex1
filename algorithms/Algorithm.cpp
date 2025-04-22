#include "Algorithm.h"
#include "./game_objects/Tank.h"

const Tank* Algorithm::getTank() {
    return tank;
}

void Algorithm::assignTank(Tank *t) {
    this->tank = t;
}

Algorithm::Algorithm(/* args */)
{
}

Algorithm::~Algorithm()
{
}
