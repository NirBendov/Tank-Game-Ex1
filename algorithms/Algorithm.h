#pragma once
class GameBoard;  // Forward declaration
#include "Action.h"
#include <vector>
#include <utility>

class Algorithm
{
protected:
    int playerId;
    GameBoard* gameBoard;
    bool isMovingBackward;
    int backwardMoveTurnsLeft;
    std::pair<int, int> lastBackwardMoveTarget;

public:
    Algorithm(int playerId, GameBoard* gameBoard): playerId(playerId), gameBoard(gameBoard) {}
    virtual ~Algorithm() = default;
    virtual std::vector<Action> decideNextActions() = 0;
    // Action moveBackwards(); 
    virtual void defaultMode() = 0;
    virtual void update() = 0;
};

