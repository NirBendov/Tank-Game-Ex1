#pragma once
#include "../game_objects/Tank.h"
class GameBoard;  // Forward declaration
#include "Action.h"
#include <vector>
using namespace std;

class Algorithm
{
protected:
    int playerId;
    GameBoard* gameBoard;
    bool isMovingBackward;
    int backwardMoveTurnsLeft;
    pair<int, int> lastBackwardMoveTarget;

public:
    Algorithm(int playerId, GameBoard* gameBoard);
    virtual ~Algorithm() = default;
    virtual vector<Action> decideNextActions() = 0;
    Action moveBackwards(); 
    virtual void defaultMode();
};
