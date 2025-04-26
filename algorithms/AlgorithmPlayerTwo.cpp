#include "Action.h"
#include "Algorithm.h"
#include "AlgorithmPlayerTwo.h"
#include "../game_objects/Moveable.h"
#include "../game_objects/Direction.h"
#include "./constants/BoardConstants.h"
#include "./board/GameBoard.h"
#include "PathFinder.h"
#include <array>
#include <vector>

using namespace std;

AlgorithmPlayerTwo::AlgorithmPlayerTwo(int playerId, GameBoard *gameBoard, Tank *tank)
    : Algorithm(playerId, gameBoard), mode(OperationMode::CHASE), tank(tank) {
    array<int,2> tankLocation = tank->getInfo().location;
    Point tankPoint = {tankLocation[0], tankLocation[1]};
    pair<int, int> enemyTankLocation = (*gameBoard).getEnemyTankPositions(Player::PlayerId::P1)[0];
    Point enemyTankPoint = {enemyTankLocation.second, enemyTankLocation.first};

    pathToEnemy = bfsPathfinder(gameBoard->getBoard(), tankPoint, enemyTankPoint, false);
}

AlgorithmPlayerTwo::~AlgorithmPlayerTwo()
{
}

void AlgorithmPlayerTwo::defaultMode() {
    if (mode != OperationMode::PANIC)
        mode = OperationMode::CHASE;
}

vector<Action> AlgorithmPlayerTwo::decideNextActions() {
    vector<Action> actions;
    
    // If we're in the middle of a backward move
    if (isMovingBackward) {
        if (backwardMoveTurnsLeft > 0) {
            backwardMoveTurnsLeft--;
            actions.push_back(Action(Action::Type::MOVE_BACKWARD, *tank));
            return actions;
        } else {
            isMovingBackward = false;
        }
    }

    // Get tank positions
    // vector<Tank> myTanks = (*gameBoard).getPlayerTankPositions(Player::PlayerId::P2);
    // vector<Tank> enemyTanks = (*gameBoard).getEnemyTankPositions(Player::PlayerId::P1);
    vector<Shell> shells = (*gameBoard).getBullets();

    array<int,2> tankLocation = tank->getInfo().location;
    for (Shell shell : shells) {
        Moveable::Info shellInfo = shell.getInfo();
        vector<array<int, 2>> dangerousDirections;
        if (isInBulletPath(shellInfo.location, shellInfo.dir, tankLocation, gameBoard)) {
            dangerousDirections.push_back(shellInfo.dir);
            Action::Type move = possibleDodgeMove(*tank, shell);
            actions.push_back({move, *tank});
            mode = OperationMode::DODGE;
            break;
        }
    }

    // For now, just return a simple action for the first tank
    // if (!myTanks.empty()) {
    //     actions.push_back(Action(Action::Type::NOP, myTanks[0]));
    // }
    return actions;
} 

/* In the rare case where there is not path from one tank
 * to another, not even by breaking walls, the algorithm
 * will enter panic mode and finish all the tank's ammo as fast as possible
*/
Action::Type AlgorithmPlayerTwo::panicRoutine() {
    if (tank->getShootingCooldown() == 0 && tank->getAmmoCount() > 0) {
        return Action::Type::SHOOT;
    }
    return Action::Type::NOP;
}

void AlgorithmPlayerTwo::addTileToAvoid() {
    array<int,2> tankPosition = tank->getInfo().location;
    tilesToAvoid.push_back({tankPosition[1], tankPosition[0]});
}

void AlgorithmPlayerTwo::removeTileToAvoid(Point &p) {
    if (gameBoard->getBoard()[p.x][p.y] == BoardConstants::SHELL)
        tilesToAvoid.erase(find(tilesToAvoid.begin(), tilesToAvoid.end(), p));
}

/***decide on an action when evading a shell
 * basic dodging, only escapes the first shell in its path
***/
Action::Type AlgorithmPlayerTwo::possibleDodgeMove(Tank &tank, Shell &shell) {
    int rows = gameBoard->getBoard().size();
    int columns = gameBoard->getBoard()[0].size();

    array<int,2> dir = tank.getInfo().dir;
    array<int,2> location = tank.getInfo().location;
    array<int,2> shellDir = shell.getInfo().dir;
    vector<vector<char>> board = gameBoard->getBoard();
    int res = vectorMultiply(dir, shellDir);

    const TurnToAction lightTurns[4] = {
        {Turn::RIGHT_45, Action::Type::TURN_R_45},
        {Turn::RIGHT_90, Action::Type::TURN_R_90},
        {Turn::LEFT_45, Action::Type::TURN_L_45}, 
        {Turn::LEFT_90, Action::Type::TURN_L_90}
        };
    const TurnToAction longTurns[3] = {
        {Turn::RIGHT_135, Action::Type::TURN_R_90}, 
        {Turn::LEFT_135, Action::Type::TURN_L_90},
        {Turn::COMPLETE_180, Action::Type::TURN_R_90}
    };
    // if tank direction isnt parallel to bullet direction
    Point p = {(location[0] + dir[0])%rows, (location[1] + dir[1])%columns};
    if ((res != 2) && (res != -2) 
    && (board[(location[0] + dir[0])%rows][(location[1] + dir[1])%columns] == BoardConstants::EMPTY_SPACE)
    && std::find(tilesToAvoid.begin(), tilesToAvoid.end(), p) == tilesToAvoid.end()) {
        // Move out of the shell's path if possible
        return Action::Type::MOVE_FORWARD;
    }
    for (TurnToAction t: lightTurns) {
        array<int,2> newDir = getDirection(dir, t.turn);
        if (board[location[0] + newDir[0]][location[1] + newDir[1]] == BoardConstants::EMPTY_SPACE
        && vectorMultiply(newDir, shellDir) != -2) {
            // Turn towards an open tile to move to
            return t.actionType;
        }
    }
    if (res == -2 && tank.getShootingCooldown() == 0) {
        // Shoot the bullet if it is headed in your direction
        return Action::Type::SHOOT;
    }
    if (res != 2 && res != -2
    && board[location[0] + dir[0]][location[1] + dir[1]] == BoardConstants::DAMAGED_WALL
    && tank.getShootingCooldown() == 0) {
        // destroy wall in order to move away
        return Action::Type::SHOOT;
    }
    for (TurnToAction t: longTurns) {
        // If needed, take a long turn, takes 2 game turns
        array<int,2> newDir = getDirection(dir, t.turn);
        if (board[location[0] + newDir[0]][location[1] + newDir[1]] == BoardConstants::EMPTY_SPACE) {
            return t.actionType;
        }
    }
    return Action::Type::NOP;
}

