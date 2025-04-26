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
}

AlgorithmBasic::~AlgorithmBasic() {
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
    
    // Check for mines in adjacent cells
    const auto& location = tankInfo.location;
    for (const auto& dir : directions) {
        int newX = (location[0] + dir[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
        int newY = (location[1] + dir[1] + BOARD_WIDTH) % BOARD_WIDTH;
        
        // Check for mines and other dangers
        if (board[newX][newY] == MINE) {
            return true;
        }
        
        // Check for enemy tanks
        const auto& enemyTanks = gameBoard->getEnemyTankPositions(playerId);
        for (const auto& enemyPos : enemyTanks) {
            if (enemyPos.first == newX && enemyPos.second == newY) {
                return true;
            }
        }
        
        // Check for shells
        for (const auto& shell : shells) {
            const auto& shellInfo = shell.getInfo();
            if (shellInfo.location[0] == newX && shellInfo.location[1] == newY) {
                return true;
            }
        }
    }
    
    return false;
}

bool AlgorithmBasic::canShootEnemy(const Tank& tank) const {
    if (shootingCooldown > 0) {
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

bool AlgorithmBasic::willShellHitPosition(int x, int y, int turnsAhead) const {
    const auto& shells = gameBoard->getBullets();
    
    for (const auto& shell : shells) {
        const auto& shellInfo = shell.getInfo();
        // Calculate where the shell will be in 'turnsAhead' turns
        // Shells move twice as fast as tanks
        int shellX = shellInfo.location[0] + shellInfo.dir[0] * turnsAhead * 2;
        int shellY = shellInfo.location[1] + shellInfo.dir[1] * turnsAhead * 2;
        
        // Handle wrap-around
        shellX = (shellX + BOARD_HEIGHT) % BOARD_HEIGHT;
        shellY = (shellY + BOARD_WIDTH) % BOARD_WIDTH;
        
        if (shellX == x && shellY == y) {
            return true;
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
    auto options = getPossibleMoves(tank);
    
    // Sort options by score (highest first)
    sort(options.begin(), options.end(), [](const MoveOption& a, const MoveOption& b) {
        return a.score > b.score;
    });
    
    // Return the action for the best option
    if (!options.empty()) {
        return options[0].action;
    }
    
    // If no options found, stay in place
    return Action::Type::NOP;
}

Action::Type AlgorithmBasic::findShootingPosition(const Tank& tank) const {
    auto options = getPossibleMoves(tank);
    
    // Sort options by score (highest first)
    sort(options.begin(), options.end(), [](const MoveOption& a, const MoveOption& b) {
        return a.score > b.score;
    });
    
    // Return the action for the best option
    if (!options.empty()) {
        return options[0].action;
    }
    
    // If no options found, stay in place
    return Action::Type::NOP;
}

vector<Action> AlgorithmBasic::decideNextActions() {
    vector<Action> actions;
    
    // Get tank positions
    const auto& myTanks = gameBoard->getPlayerTanks(playerId);
    
    // Process each tank
    for (const auto& tank : myTanks) {
        if (!tank.getIsTankAlive()) continue;
        
        // Check if tank is in danger
        if (isInDanger(tank)) {
            // Find a safe move to avoid danger
            Action::Type safeMove = findSafeMove(tank);
            actions.push_back(Action(safeMove, tank));
        }
        // Check if we can shoot an enemy
        else if (canShootEnemy(tank)) {
            actions.push_back(Action(Action::Type::SHOOT, tank));
            shootingCooldown = SHOOT_COOLDOWN + 1;
        }
        // Otherwise, try to position for shooting
        else {
            Action::Type move = findShootingPosition(tank);
            actions.push_back(Action(move, tank));
        }
    }
    
    // Decrease shooting cooldown
    if (shootingCooldown > 0) {
        shootingCooldown--;
    }
    
    return actions;
} 