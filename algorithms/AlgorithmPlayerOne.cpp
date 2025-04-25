#include "AlgorithmPlayerOne.h"

AlgorithmPlayerOne::AlgorithmPlayerOne(int playerId, GameBoard* gameBoard) 
    : Algorithm(playerId, gameBoard) {
}

AlgorithmPlayerOne::~AlgorithmPlayerOne()
{
}

vector<Action> AlgorithmPlayerOne::decideNextActions() {
    vector<Action> actions;
    
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
    auto myTanks = gameBoard->getPlayerTankPositions(playerId);
    auto enemyTanks = gameBoard->getEnemyTankPositions(playerId);
    
    // For now, just return a simple action for the first tank
    if (!myTanks.empty()) {
        actions.push_back(Action(Action::ActionType::NOP, myTanks[0]));
    }
    return actions;
} 