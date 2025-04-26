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
    AlgorithmPlayerTwo(int playerId, GameBoard* gameBoard, Tank *tank);
    ~AlgorithmPlayerTwo();

    vector<Action> decideNextActions() override;
    Action::Type possibleDodgeMove(Tank &tank, Shell &shell);
    vector<Point> findBestPath();
    Action::Type followPath();
    Action::Type panicRoutine();
    void addTileToAvoid();
    void removeTileToAvoid(Point &p);
    void defaultMode() override;
};