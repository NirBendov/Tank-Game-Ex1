#pragma once
#include "Action.h"
#include "Algorithm.h"
#include "../game_objects/Direction.h"
#include "./constants/BoardConstants.h"
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
    vector<Point> pathToEnemy;
    vector<Point> tilesToAvoid;
    Tank *tank;
public:
    AlgorithmPlayerTwo(int playerId, GameBoard* gameBoard);
    ~AlgorithmPlayerTwo();

    vector<Action> decideNextActions() override;
    Action::Type possibleDodgeMove(Tank &tank, Shell &shell);
    Action::Type followPath();
    Action panicRoutine();
    Action regularRoutine();
    void addTileToAvoid();
    void removeTilesToAvoid();
    void defaultMode() override;
    void update() override;
};