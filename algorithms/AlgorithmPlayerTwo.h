#pragma once
#include "Action.h"
#include "Algorithm.h"
#include "../game_objects/Moveable.h"
#include "../game_objects/Direction.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Shell.h"
#include "../constants/BoardConstants.h"
#include "../board/GameBoard.h"
#include "PathFinder.h"
using namespace std;

struct TurnToAction {
    Turn turn;
    Action::Type actionType;
};

class AlgorithmPlayerTwo: public Algorithm 
{
private:
    enum OperationMode {
        CHASE,
        HUNT,
        DODGE,
        PANIC
    };

    OperationMode mode;
    Tank* tank;
    vector<Point> pathToEnemy;
    bool isMovingBackward = false;
    int backwardMoveTurnsLeft = 0;
    Action::Type possibleDodgeMove(Tank &tank, Shell &shell);
    Action::Type followPath();
    Action panicRoutine();
    Action regularRoutine();
public:
    AlgorithmPlayerTwo(int playerId, GameBoard* gameBoard);
    ~AlgorithmPlayerTwo() override;

    vector<Action> decideNextActions() override;
    void defaultMode() override;
    void update() override;
};