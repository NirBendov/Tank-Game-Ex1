#pragma once
#include "./game_objects/Tank.h"
#include <vector>
using namespace std;

class Algorithm
{
private:
    Tank *tank;
public:
    enum Action {
        NOP,
        SHOOT,
        MOVE_FORWARD,
        MOVE_BACKWARD,
        TURN_R_45,
        TURN_R_90,
        TURN_L_45,
        TURN_L_90
    };
    Algorithm(/* args */);
    ~Algorithm();
    const Tank* getTank();
    void assignTank(Tank *t);
    vector<Action> decideNextActions();
};
