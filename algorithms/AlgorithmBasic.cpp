#include "AlgorithmBasic.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Shell.h"
#include "../game_objects/Direction.h"
#include "../constants/BoardConstants.h"
#include "../board/GameBoard.h"
#include "Action.h"
#include <vector>
#include <algorithm>
#include <map>
#include <array>

using namespace std;
using namespace BoardConstants;

AlgorithmBasic::AlgorithmBasic(int playerId, GameBoard* gameBoard) 
    : Algorithm(playerId, gameBoard) {
    myTank = new Tank(gameBoard->getPlayerTanks(playerId)[0]);
    myTank->assignAlgorithm(this);
}

AlgorithmBasic::~AlgorithmBasic() {
    delete myTank;
}

bool AlgorithmBasic::isInDanger(const Tank& tank) const {
    const auto& tankInfo = tank.getInfo();
    const auto& shells = gameBoard->getBullets();
    const auto& board = gameBoard->getBoard();
    
    // Check if any shell is heading towards the tank
    for (const auto& shell : shells) {
        const auto& shellInfo = shell.getInfo();
        if (isInBulletPath(shellInfo.location, shellInfo.dir, tankInfo.location, gameBoard)) {
            return true;
        }
    }
    
    return false;
}

bool AlgorithmBasic::canShootEnemy(const Tank& tank) const {
    if (tank.getShootingCooldown() > 0) {
        return false;
    }
    
    const auto& tankInfo = tank.getInfo();
    const auto& enemyTanks = gameBoard->getEnemyTankPositions(playerId);
    
    // Check if any enemy tank is in the line of fire
    for (const auto& enemyPos : enemyTanks) {
        if (isInBulletPath(tankInfo.location, tankInfo.dir, {enemyPos.first, enemyPos.second}, gameBoard)) {
            return true;
        }
    }
    
    return false;
}

bool AlgorithmBasic::canShootFromPosition(const Tank& tank, int x, int y) const {
    const auto& enemyTanks = gameBoard->getEnemyTankPositions(playerId);
    
    // For each direction, check if we can shoot an enemy
    for (const auto& dir : directions) {
        // Check if any enemy is in this direction
        for (const auto& enemyPos : enemyTanks) {
            array<int, 2> startPos = {x, y};
            array<int, 2> endPos = {enemyPos.first, enemyPos.second};
            array<int, 2> dir = {dir[0], dir[1]};
            if (isInBulletPath(startPos, dir, endPos, gameBoard)) {
                return true;
            }
        }
    }
    return false;
}

bool AlgorithmBasic::isPositionSafeFromShells(int x, int y) const {
    const auto& bullets = gameBoard->getBullets();

    for (const auto& bullet : bullets) {
        const auto& bulletInfo = bullet.getInfo();

        int dx = (bulletInfo.location[0] - x + BOARD_WIDTH) % BOARD_WIDTH;
        int dy = (bulletInfo.location[1] - y + BOARD_HEIGHT) % BOARD_HEIGHT;

        // If shell is moving towards position
        if (dx == 0 || dy == 0 || abs(dx) == abs(dy)) {
            return false;
        }
    }

    return true;
}

vector<MoveOption> AlgorithmBasic::getPossibleMoves(const Tank& tank) const {
    vector<MoveOption> moves;
    const auto& board = gameBoard->getBoard();
    const auto& tankInfo = tank.getInfo();
    const auto& bullets = gameBoard->getBullets();

    for (const auto& dir : directions) {
        int newX = (tankInfo.location[0] + dir[0] + BOARD_WIDTH) % BOARD_WIDTH;
        int newY = (tankInfo.location[1] + dir[1] + BOARD_HEIGHT) % BOARD_HEIGHT;

        if (board[newX][newY] == EMPTY_SPACE) {
            bool isSafe = true;
            for (const auto& bullet : bullets) {
                const auto& bulletInfo = bullet.getInfo();
                if (isInBulletPath(bulletInfo.location, bulletInfo.dir, {newX, newY}, gameBoard)) {
                    isSafe = false;
                    break;
                }
            }
            
            if (isSafe) {
                MoveOption move;
                move.x = newX;
                move.y = newY;
                move.action = Action::Type::MOVE_FORWARD;
                move.score = 0;
                move.hasShellInFuture = false;
                move.canShootFromHere = false;
                moves.push_back(move);
            }
        }
    }

    return moves;
}

Action::Type AlgorithmBasic::findSafeMove(const Tank& tank) const {
    const auto& tankInfo = tank.getInfo();
    const auto& board = gameBoard->getBoard();
    const auto& shells = gameBoard->getBullets();
    
    // Check if we're in any bullet's path
    for (const auto& shell : shells) {
        const auto& shellInfo = shell.getInfo();
        if (isInBulletPath(shellInfo.location, shellInfo.dir, tankInfo.location, gameBoard)) {
            // First try to move forward
            array<int, 2> nextPos = tank.potentialMove();
            if (board[nextPos[0]][nextPos[1]] != WALL && 
                board[nextPos[0]][nextPos[1]] != DAMAGED_WALL && 
                board[nextPos[0]][nextPos[1]] != MINE) {
                return Action::Type::MOVE_FORWARD;
            }

            // If can't move forward, try the four possible turns
            const array<Action::Type, 4> possibleTurns = {
                Action::Type::TURN_R_90,
                Action::Type::TURN_R_45,
                Action::Type::TURN_L_90,
                Action::Type::TURN_L_45
            };

            for (const auto& turnAction : possibleTurns) {
                // Create a temporary tank to test the move
                Tank tempTank = tank;
                tempTank.assignAlgorithm(nullptr);
                
                // Apply the turn
                if (turnAction == Action::Type::TURN_R_90) {
                    tempTank.turn(Turn::RIGHT_90);
                } else if (turnAction == Action::Type::TURN_R_45) {
                    tempTank.turn(Turn::RIGHT_45);
                } else if (turnAction == Action::Type::TURN_L_90) {
                    tempTank.turn(Turn::LEFT_90);
                } else if (turnAction == Action::Type::TURN_L_45) {
                    tempTank.turn(Turn::LEFT_45);
                }

                // Check if we can move in this new direction
                nextPos = tempTank.potentialMove();
                if (board[nextPos[0]][nextPos[1]] != WALL && 
                    board[nextPos[0]][nextPos[1]] != DAMAGED_WALL && 
                    board[nextPos[0]][nextPos[1]] != MINE) {
                    return turnAction;
                }
            }
        }
    }
    
    return Action::Type::NOP;
}

Action::Type AlgorithmBasic::findShootingPosition(const Tank& tank) const {
    const auto& tankInfo = tank.getInfo();
    const auto& board = gameBoard->getBoard();
    const auto& enemyTanks = gameBoard->getEnemyTankPositions(playerId);
    auto& enemyTank = enemyTanks[0];
    
    if (isInBulletPath(tankInfo.location, tankInfo.dir, {enemyTank.first, enemyTank.second}, gameBoard)) {
        return Action::Type::NOP;
    }
    
    for (const auto& dir : directions) {
        array<int, 2> trajectory = {dir[0], dir[1]};
        array<int, 2> enemyPos = {enemyTanks[0].first, enemyTanks[0].second};
        if (isInBulletPath(tankInfo.location, trajectory, enemyPos, gameBoard)) {
            Turn turn = rotation(tankInfo.dir, trajectory);
            Action::Type action = turnToAction(turn);
            if (action != Action::Type::NOP) {
                return action;
            }
        }
    }
    
    // Check if enemy is to the left or right
    int dx = enemyTank.first - tankInfo.location[0];
    
    array<int, 2> targetDir = {0, dx > 0 ? 1 : -1}; // Right or left
    Turn turn = rotation(tankInfo.dir, targetDir);
    Action::Type action = turnToAction(turn);
    if (action != Action::Type::NOP) {
        return action;
    }

    // If we're already aligned, check if we can move forward
    array<int, 2> nextPos = tank.potentialMove();
    if (board[nextPos[0]][nextPos[1]] != WALL && 
        board[nextPos[0]][nextPos[1]] != DAMAGED_WALL && 
        board[nextPos[0]][nextPos[1]] != MINE) {
        return Action::Type::MOVE_FORWARD;
    }
    
    return Action::Type::NOP;
}

vector<Action> AlgorithmBasic::decideNextActions() {
    vector<Action> actions;
    
    // Get tank positions
    const auto& myTanks = gameBoard->getPlayerTanks(playerId);
    
    // Process each tank
    for (const auto& tank : myTanks) {
        if (!tank.getIsTankAlive()) continue;
        
        // First priority: Check if tank is in danger
        if (isInDanger(tank)) {
            // Find a safe move to avoid danger
            Action::Type safeMove = findSafeMove(tank);
            actions.push_back(Action(safeMove, tank));
            continue;
        }

        // Second priority: Check if we can shoot an enemy
        if (canShootEnemy(tank)) {
            actions.push_back(Action(Action::Type::SHOOT, tank));
            continue;
        }
        
        // Third priority: Try to position for shooting
        Action::Type move = findShootingPosition(tank);
        actions.push_back(Action(move, tank));
    }
    
    return actions;
}

void AlgorithmBasic::defaultMode() {
    // Basic algorithm doesn't have different modes
    // This is a no-op implementation
}

void AlgorithmBasic::update() {
    // Basic algorithm doesn't need to update any state
    // This is a no-op implementation
} 