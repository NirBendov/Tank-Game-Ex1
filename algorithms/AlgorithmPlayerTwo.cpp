#include "Action.h"
#include "Algorithm.h"
#include "AlgorithmPlayerTwo.h"
#include "../game_objects/Moveable.h"
#include "../game_objects/Direction.h"
#include "./constants/BoardConstants.h"
#include "PathFinder.h"
#include <vector>

using namespace std;

AlgorithmPlayerTwo::AlgorithmPlayerTwo(int playerId, GameBoard* gameBoard, Tank *tank)
    : Algorithm(playerId, gameBoard): mode(OperationMode::CHASE), tank(tank) {
    Array<int,2> tankLocation = tank->info.location;
    Point tankPoint = {tankLocation[1], tankLocation[0]};

    Tank &enemyTank = gameBoard.getEnemyTankPositions(Player::PlayerId::P1)[0]
    Array<int,2> enemyTankLocation = enemyTank->info.location;
    Point enemyTankPoint = {enemyTankLocation[1], enemyTankLocation[0]};

    pathToEnemy = bfsPathfinder(gameBoard.board, tankPoint, enemyTankPoint, false);
}

AlgorithmPlayerTwo::~AlgorithmPlayerTwo()
{
}

vector<Action> AlgorithmPlayerTwo::decideNextActions() {
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

    for (const Shell shell : shells) {
        Moveable::Info shellInfo = shell.getInfo();
        vector<Array<int, 2>> dangerousDirections;
        if (isInBulletPath(shellInfo.location, shellInfo.dir, tankInfo.location)) {
            dangerousDirections.push_back(shellInfo.dir);
            Action::Type move = possibleDodgeMove(myTanks[0]);
            actions.push_back({move, myTanks[0]});
            mode = OperationMode::Dodge;
            break;
        }
    }
    
    // Get tank positions
    vector<Tank> &myTanks = gameBoard.getPlayerTankPositions(Player::PlayerId::P2);
    vector<Tank> &enemyTanks = gameBoard.getEnemyTankPositions(Player::PlayerId::P1);
    vector<Shell> &shells = gameBoard.getBullets();
    
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

// decide on an action when evading a shell
Action::type AlgorithmPlayerTwo::possibleDodgeMove(Tank &tank, Shell &shell) {
    array<int,2> dir = tank.info.dir;
    array<int,2> shellDir = shell.info.dir;
    int res = vectorMultiply(dir, shellDir);
    const TurnToAction lightTurns[4] = {
        {Direction::Turn::RIGHT_45, Action::Type::TURN_R_45},
        {Direction::Turn::RIGHT_90, Action::Type::TURN_R_90},
        {Direction::Turn::LEFT_45, Action::Type::TURN_L_45}, 
        {Direction::Turn::LEFT_90, Action::Type::TURN_L_90}
        };
    const TurnToAction longTurns[3] = {
        {Direction::Turn::RIGHT_135, Action::Type::TURN_R_90}, 
        {Direction::Turn::LEFT_135, Action::Type::TURN_L_90},
        {Direction::Turn::COMPLETE_180, Action::Type::TURN_R_90}
    }
    // if tank direction isnt parallel to bullet direction
    if (res != 2 && res != -2 
    && gameBoard.board[info.location[0] + dir[0]][info.location[1] + dir[1]] == BoardConstants::EMPTY_SPACE) {
        // move away if possible
        return Action::Type::MOVE_FORWARD;
    }
    for (TurnToAction t: lightTurns) {
        array<int,2> newDir = getDirection(dir, t.turn);
        if (gameBoard.board[info.location[0] + newDir[0]][info.location[1] + newDir[1]] == BoardConstants::EMPTY_SPACE
        && vectorMultiply(newDir, shellDir) != -2) {
            // Turn towards an open tile to move to
            return t.actionType;
        }
    }
    if (if res == -2 && tank.getShootingCooldown() == 0) {
        // Shoot the bullet if it is headed in your direction
        return Action::Type::SHOOT;
    }
    if (res != 2 && res != -2
    && gameBoard.board[info.location[0] + dir[0]][info.location[1] + dir[1]] == BoardConstants::DAMAGED_WALL) {
        // destroy wall in order to move away
        return Action::Type::SHOOT;
    }
    for (TurnToAction t: longTurns) {
        // If needed, take a long turn, takes 2 game turns
        array<int,2> newDir = getDirection(dir, t.turn);
        if (gameBoard.board[info.location[0] + newDir[0]][info.location[1] + newDir[1]] == BoardConstants::EMPTY_SPACE) {
            return t.actionType;
        }
    }
    return Action::Type::NOP;
}

