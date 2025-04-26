#pragma once
#include "Action.h"
#include "Algorithm.h"
#include "../game_objects/Direction.h"
#include "./constants/BoardConstants.h"
#include "PathFinder.h"
using namespace std;

struct TurnToAction {
    Direction::Turn turn;
    Action::Type actionType;
};

class AlgorithmPlayerTwo: public Algorithm 
{
private:
    enum OperationMode {
        CHASE,
        HUNT,
        DODGE
    }
    OperationMode mode;
    vector<Point> pathToEnemy;
    Tank *tank;
public:
    AlgorithmPlayerTwo(int playerId, GameBoard* gameBoard, Tank *tank);
    ~AlgorithmPlayerTwo();

    vector<Action> decideNextActions() override;
    Action::Type possibleDodgeMove(Tank &tank);
    vector<Point> findBestPath();
    Action::Type followPath();
};