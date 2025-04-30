#include "Action.h"
#include "Algorithm.h"
#include "AlgorithmPlayerTwo.h"
#include "../game_objects/Moveable.h"
#include "../game_objects/Direction.h"
#include "../constants/BoardConstants.h"
#include "../board/GameBoard.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Shell.h"
#include "PathFinder.h"
#include <array>
#include <vector>
#include <iostream>
using namespace std;

AlgorithmPlayerTwo::AlgorithmPlayerTwo(int playerId, GameBoard *gameBoard)
    : Algorithm(playerId, gameBoard), mode(OperationMode::CHASE) {
    cout << "Initializing AlgorithmPlayerTwo for player " << playerId << endl;
    tank = new Tank(gameBoard->getPlayerTanks(playerId)[0]);
    tank->assignAlgorithm(this);

    array<int,2> tankLocation = tank->getInfo().location;
    cout << "Tank location: (" << tankLocation[0] << "," << tankLocation[1] << ")" << endl;
    Point tankPoint = {tankLocation[0], tankLocation[1]};
    pair<int, int> enemyTankLocation = gameBoard->getEnemyTankPositions(playerId)[0];
    cout << "Enemy tank location: (" << enemyTankLocation.first << "," << enemyTankLocation.second << ")" << endl;
    Point enemyTankPoint = {enemyTankLocation.first, enemyTankLocation.second};

    pathToEnemy = bfsPathfinder(gameBoard->getBoard(), tankPoint, enemyTankPoint, false);
    cout << "Path to enemy found with " << pathToEnemy.size() << " points" << endl;
}

AlgorithmPlayerTwo::~AlgorithmPlayerTwo()
{
    delete tank;
}

void AlgorithmPlayerTwo::defaultMode() {
    if (mode != OperationMode::PANIC)
        mode = OperationMode::CHASE;
}

vector<Action> AlgorithmPlayerTwo::decideNextActions() {
    cout << "Deciding next actions for tank at (" << tank->getInfo().location[0] << "," << tank->getInfo().location[1] << ")" << endl;
    vector<Action> actions;
    if (mode == OperationMode::PANIC) {
        cout << "In PANIC mode" << endl;
        actions.push_back(panicRoutine());
    }
    else {
        cout << "In regular mode" << endl;
        actions.push_back(regularRoutine());
    }
    return actions;
} 

/* In the rare case where there is not path from one tank
 * to another, not even by breaking walls, the algorithm
 * will enter panic mode and finish all the tank's ammo as fast as possible
*/
Action AlgorithmPlayerTwo::panicRoutine() {
    if (tank->getShootingCooldown() == 0 && tank->getAmmoCount() > 0) {
        cout << "Panic: shooting" << endl;
        return {Action::Type::SHOOT, *tank};
    }
    cout << "Panic: no action" << endl;
    return {Action::Type::NOP, *tank};
}

Action AlgorithmPlayerTwo::regularRoutine() {
    cout << "Starting regular routine" << endl;
    pair<int, int> enemyPosition = gameBoard->getEnemyTankPositions(playerId)[0];
    Point enemy = {enemyPosition.first, enemyPosition.second};
    cout << "Enemy position: (" << enemy.x << "," << enemy.y << ")" << endl;

    vector<vector<char>> board = gameBoard->getBoard();
    int rows = board.size();
    int columns = board[0].size();

    updatePathEnd(pathToEnemy, enemy, rows, columns);
    cout << "Updated path to enemy, now has " << pathToEnemy.size() << " points" << endl;
    
    // If we're in the middle of a backward move
    if (isMovingBackward) {
        cout << "In backward move" << endl;
        if (backwardMoveTurnsLeft > 0) {
            backwardMoveTurnsLeft--;
            return Action(Action::Type::NOP, *tank);
        } else {
            isMovingBackward = false;
            return Action(Action::Type::MOVE_BACKWARD, *tank);
        }
    }

    vector<Shell> shells = gameBoard->getBullets();
    cout << "Found " << shells.size() << " shells on board" << endl;

    array<int,2> tankLocation = tank->getInfo().location;
    for (Shell shell : shells) {
        Moveable::Info shellInfo = shell.getInfo();
        vector<array<int, 2>> dangerousDirections;
        if (isInBulletPath(shellInfo.location, shellInfo.dir, tankLocation, gameBoard)) {
            cout << "Shell in path, dodging" << endl;
            dangerousDirections.push_back(shellInfo.dir);
            Action::Type move = possibleDodgeMove(*tank, shell);
            mode = OperationMode::DODGE;
            return {move, *tank};
        }
    }

    if (mode != OperationMode::DODGE) {
        cout << "Not in dodge mode" << endl;
        vector<vector<char>> board = gameBoard->getBoard();
        int rows = board.size();
        int columns = board[0].size();

        mode = OperationMode::CHASE;
        if (isPathStraight(pathToEnemy, rows, columns)) {
            cout << "Path is straight" << endl;
            array<int,2> pathDir = calcDirection(pathToEnemy, rows, columns);
            array<int,2> dir = tank->getInfo().dir;
            if (isPathClear(pathToEnemy, board)) {
                cout << "Path is clear" << endl;
                if (pathDir == dir) {
                    cout << "Tank facing enemy, shooting" << endl;
                    mode = OperationMode::HUNT;
                    return {Action::Type::SHOOT, *tank};
                }
                else {
                    cout << "Turning to face enemy" << endl;
                    Turn t = rotation(dir, pathDir);
                    return {turnToAction(t), *tank};
                }
            }
        }
        cout << "Following path" << endl;
        return {followPath(), *tank};
    }

    cout << "No action needed" << endl;
    return {Action::Type::NOP, *tank};
}

// Don't return to the same tile you just escaped from
void AlgorithmPlayerTwo::addTileToAvoid() {
    array<int,2> tankPosition = tank->getInfo().location;
    tilesToAvoid.push_back({tankPosition[1], tankPosition[0]});
}

// Tile should be safe again after the shell already passed
void AlgorithmPlayerTwo::removeTilesToAvoid() {
    vector<Point>::iterator it = tilesToAvoid.begin();
    while(it != tilesToAvoid.end()) {
        Point p = *it;
        if (gameBoard->getBoard()[p.x][p.y] == BoardConstants::SHELL)
            tilesToAvoid.erase(find(tilesToAvoid.begin(), tilesToAvoid.end(), p));
        else
            it++;
    }
}

void AlgorithmPlayerTwo::update() {
    removeTilesToAvoid();
    if (pathToEnemy.empty())
        mode = OperationMode::PANIC;
}

/***decide on an action when evading a shell
 * basic dodging, only escapes the first shell in its path
***/
Action::Type AlgorithmPlayerTwo::possibleDodgeMove(Tank &tank, Shell &shell) {
    vector<vector<char>> board = gameBoard->getBoard();
    int rows = board.size();
    int columns = board[0].size();

    array<int,2> dir = tank.getInfo().dir;
    array<int,2> location = tank.getInfo().location;
    array<int,2> shellDir = shell.getInfo().dir;
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
        updatePathStart(pathToEnemy, p, rows, columns);
        return Action::Type::MOVE_FORWARD;
    }
    for (TurnToAction t: lightTurns) {
        array<int,2> newDir = getDirection(dir, t.turn);
        if (board[(location[0] + newDir[0])%rows][(location[1] + newDir[1])%columns] == BoardConstants::EMPTY_SPACE
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
    && board[(location[0] + dir[0])%rows][(location[1] + dir[1])%columns] == BoardConstants::DAMAGED_WALL
    && tank.getShootingCooldown() == 0) {
        // destroy wall in order to move away
        return Action::Type::SHOOT;
    }
    for (TurnToAction t: longTurns) {
        // If needed, take a long turn, takes 2 game turns
        array<int,2> newDir = getDirection(dir, t.turn);
        if (board[(location[0] + newDir[0])%rows][(location[1] + newDir[1])%columns] == BoardConstants::EMPTY_SPACE) {
            return t.actionType;
        }
    }
    return Action::Type::NOP;
}

Action::Type AlgorithmPlayerTwo::followPath() {
    vector<vector<char>> board = gameBoard->getBoard();
    int rows = board.size();
    int columns = board[0].size();
    Moveable::Info info = tank->getInfo();

    Point &start = pathToEnemy[0];
    Point &next = pathToEnemy[1];
    array<int,2> dir = directionBetweenPoints(start, next);
    if (dir == info.dir) {
        char tile = board[(info.location[0] + dir[0])%rows][(info.location[1] + dir[1])%columns];
        if (tile == BoardConstants::WALL || tile == BoardConstants::DAMAGED_WALL)
            return Action::Type::SHOOT;
        if (std::find(tilesToAvoid.begin(), tilesToAvoid.end(), next) != tilesToAvoid.end()) {
            pathToEnemy.erase(pathToEnemy.begin());
            return Action::Type::MOVE_FORWARD;
        }
        return Action::Type::NOP;
    } 
    else {
        Turn t = rotation(info.dir, dir);
        return turnToAction(t);
    }
}
