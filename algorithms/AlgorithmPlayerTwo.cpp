#include "Algorithm.h"
#include "AlgorithmPlayerTwo.h"
#include "./game_objects/Moveable.h"
#include "./game_objects/Direction.h"
#include <vector>

using namespace std;

AlgorithmPlayerTwo::AlgorithmPlayerTwo(int playerId, GameBoard* gameBoard)
    : Algorithm(playerId, gameBoard) {
}

AlgorithmPlayerTwo::~AlgorithmPlayerTwo()
{
}

vector<Action> AlgorithmPlayerTwo::decideNextActions() {
    vector<Action> actions;
    GameBoard::BoardData *boardData = gameBoard->boardData;
    
    // If we're in the middle of a backward move
    if (isMovingBackward) {
        if (backwardMoveTurnsLeft > 0) {
            backwardMoveTurnsLeft--;
            actions.push_back(Action(Action::ActionType::MOVE_BACKWARD, lastBackwardMoveTarget));
            return actions;
        } else {
            isMovingBackward = false;
        }
    }
    
    // Get tank positions
    vector<Tank> myTanks = boardData->tank2Position;
    vector<Tank> enemyTanks = boardData->tank1Position;
    
    // For now, just return a simple action for the first tank
    if (!myTanks.empty()) {
        actions.push_back(Action(Action::ActionType::NOP, myTanks[0]));
    }
    return actions;
} 

Action dodge(Tank &tank, vector<Shell> shells) {
    Moveable::Info tankInfo = tank.getInfo();
    for (const Shell shell : shells) {
        Moveable::Info shellInfo = shell.getInfo();
        if (isInBulletPath(shellInfo.location, shellInfo.dir, tankInfo.location)) {
            int res = vectorMultiply(shellInfo.dir, tankInfo.dir)
            if (res == 2 || res == -2) {

            }
        }
    }
}

