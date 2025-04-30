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
        
        // Check for mines
        if (board[newX][newY] == MINE) {
            return true;
        }
        
        // Check for shells that will hit this position in the next turn
        if (willShellHitPosition(newX, newY, 1)) {
            return true;
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
    const auto& tankInfo = tank.getInfo();
    const auto& board = gameBoard->getBoard();
    const auto& shells = gameBoard->getBullets();
    
    // First check if we need to turn to avoid danger
    for (const auto& shell : shells) {
        const auto& shellInfo = shell.getInfo();
        if (isInBulletPath(shellInfo.location, shellInfo.dir, tankInfo.location, gameBoard)) {
            // Try to turn perpendicular to the shell's direction
            array<int, 2> shellDir = shellInfo.dir;
            array<int, 2> currentDir = tankInfo.dir;
            
            // Calculate perpendicular direction
            array<int, 2> perpDir = {-shellDir[1], shellDir[0]};
            
            // Check if we can turn to face perpendicular direction
            if (currentDir[0] != perpDir[0] || currentDir[1] != perpDir[1]) {
                // Calculate turn needed
                int dotProduct = currentDir[0] * perpDir[0] + currentDir[1] * perpDir[1];
                int crossProduct = currentDir[0] * perpDir[1] - currentDir[1] * perpDir[0];
                
                if (crossProduct > 0) {
                    return Action::Type::TURN_R_45; // Try 45 degree turn first
                } else {
                    return Action::Type::TURN_L_45; // Try 45 degree turn first
                }
            }
        }
    }
    
    // If no immediate danger, try to move forward if safe
    int newX = (tankInfo.location[0] + tankInfo.dir[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
    int newY = (tankInfo.location[1] + tankInfo.dir[1] + BOARD_WIDTH) % BOARD_WIDTH;
    
    // Check if the forward position is safe
    if (board[newX][newY] != MINE && isPositionSafeFromShells(newX, newY)) {
        return Action::Type::MOVE_FORWARD;
    }
    
    // If forward is not safe, try turning
    for (const auto& turn : {Action::Type::TURN_R_45, Action::Type::TURN_L_45, 
                            Action::Type::TURN_R_90, Action::Type::TURN_L_90}) {
        array<int, 2> newDir = tankInfo.dir;
        if (turn == Action::Type::TURN_R_45) {
            newDir = {tankInfo.dir[0] - tankInfo.dir[1], tankInfo.dir[0] + tankInfo.dir[1]};
        } else if (turn == Action::Type::TURN_L_45) {
            newDir = {tankInfo.dir[0] + tankInfo.dir[1], -tankInfo.dir[0] + tankInfo.dir[1]};
        } else if (turn == Action::Type::TURN_R_90) {
            newDir = {tankInfo.dir[1], -tankInfo.dir[0]};
        } else {
            newDir = {-tankInfo.dir[1], tankInfo.dir[0]};
        }
        
        newX = (tankInfo.location[0] + newDir[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
        newY = (tankInfo.location[1] + newDir[1] + BOARD_WIDTH) % BOARD_WIDTH;
        
        if (board[newX][newY] != MINE && isPositionSafeFromShells(newX, newY)) {
            return turn;
        }
    }
    
    // If no safe moves found, stay in place
    return Action::Type::NOP;
}

Action::Type AlgorithmBasic::findShootingPosition(const Tank& tank) const {
    const auto& tankInfo = tank.getInfo();
    const auto& board = gameBoard->getBoard();
    const auto& enemyTanks = gameBoard->getEnemyTankPositions(playerId);
    
    // First check if we can shoot an enemy from current position
    if (canShootEnemy(tank)) {
        return Action::Type::SHOOT;
    }
    
    // Find closest enemy tank
    int closestDist = INT_MAX;
    array<int, 2> closestEnemy = {-1, -1};
    
    for (const auto& enemyPos : enemyTanks) {
        int dx = (enemyPos.first - tankInfo.location[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
        int dy = (enemyPos.second - tankInfo.location[1] + BOARD_WIDTH) % BOARD_WIDTH;
        int dist = dx * dx + dy * dy;
        
        if (dist < closestDist) {
            closestDist = dist;
            closestEnemy = {enemyPos.first, enemyPos.second};
        }
    }
    
    if (closestEnemy[0] == -1) {
        return Action::Type::NOP; // No enemies found
    }
    
    // Calculate direction to enemy
    int dx = (closestEnemy[0] - tankInfo.location[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
    int dy = (closestEnemy[1] - tankInfo.location[1] + BOARD_WIDTH) % BOARD_WIDTH;
    
    // Try to turn towards enemy
    if (dx != 0 || dy != 0) {
        // Calculate desired direction
        array<int, 2> desiredDir = {0, 0};
        if (abs(dx) > abs(dy)) {
            desiredDir = {dx > 0 ? 1 : -1, 0};
        } else {
            desiredDir = {0, dy > 0 ? 1 : -1};
        }
        
        // Check if we need to turn
        if (tankInfo.dir[0] != desiredDir[0] || tankInfo.dir[1] != desiredDir[1]) {
            // Calculate turn needed
            int dotProduct = tankInfo.dir[0] * desiredDir[0] + tankInfo.dir[1] * desiredDir[1];
            int crossProduct = tankInfo.dir[0] * desiredDir[1] - tankInfo.dir[1] * desiredDir[0];
            
            if (crossProduct > 0) {
                // Try 45 degree turn first
                array<int, 2> newDir = {tankInfo.dir[0] - tankInfo.dir[1], tankInfo.dir[0] + tankInfo.dir[1]};
                int newX = (tankInfo.location[0] + newDir[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
                int newY = (tankInfo.location[1] + newDir[1] + BOARD_WIDTH) % BOARD_WIDTH;
                
                if (board[newX][newY] != MINE && isPositionSafeFromShells(newX, newY)) {
                    return Action::Type::TURN_R_45;
                }
                return Action::Type::TURN_R_90;
            } else {
                // Try 45 degree turn first
                array<int, 2> newDir = {tankInfo.dir[0] + tankInfo.dir[1], -tankInfo.dir[0] + tankInfo.dir[1]};
                int newX = (tankInfo.location[0] + newDir[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
                int newY = (tankInfo.location[1] + newDir[1] + BOARD_WIDTH) % BOARD_WIDTH;
                
                if (board[newX][newY] != MINE && isPositionSafeFromShells(newX, newY)) {
                    return Action::Type::TURN_L_45;
                }
                return Action::Type::TURN_L_90;
            }
        }
        
        // If facing the right direction, try to move forward
        int newX = (tankInfo.location[0] + tankInfo.dir[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
        int newY = (tankInfo.location[1] + tankInfo.dir[1] + BOARD_WIDTH) % BOARD_WIDTH;
        
        if (board[newX][newY] != MINE && isPositionSafeFromShells(newX, newY)) {
            return Action::Type::MOVE_FORWARD;
        }
    }
    
    // If we can't move forward, try turning
    for (const auto& turn : {Action::Type::TURN_R_45, Action::Type::TURN_L_45,
                            Action::Type::TURN_R_90, Action::Type::TURN_L_90}) {
        array<int, 2> newDir = tankInfo.dir;
        if (turn == Action::Type::TURN_R_45) {
            newDir = {tankInfo.dir[0] - tankInfo.dir[1], tankInfo.dir[0] + tankInfo.dir[1]};
        } else if (turn == Action::Type::TURN_L_45) {
            newDir = {tankInfo.dir[0] + tankInfo.dir[1], -tankInfo.dir[0] + tankInfo.dir[1]};
        } else if (turn == Action::Type::TURN_R_90) {
            newDir = {tankInfo.dir[1], -tankInfo.dir[0]};
        } else {
            newDir = {-tankInfo.dir[1], tankInfo.dir[0]};
        }
        
        int newX = (tankInfo.location[0] + newDir[0] + BOARD_HEIGHT) % BOARD_HEIGHT;
        int newY = (tankInfo.location[1] + newDir[1] + BOARD_WIDTH) % BOARD_WIDTH;
        
        if (board[newX][newY] != MINE && isPositionSafeFromShells(newX, newY)) {
            return turn;
        }
    }
    
    // If no moves found, stay in place
    return Action::Type::NOP;
}

vector<Action> AlgorithmBasic::decideNextActions() {
    vector<Action> actions;
    
    // Get tank positions
    const auto& myTanks = gameBoard->getPlayerTanks(playerId);
    
    // Process each tank
    for (const auto& tank : myTanks) {
        if (!tank.getIsTankAlive()) continue;
        
        // First priority: Check if we can shoot an enemy
        if (canShootEnemy(tank)) {
            actions.push_back(Action(Action::Type::SHOOT, tank));
            shootingCooldown = SHOOT_COOLDOWN + 1;
            continue;
        }
        
        // Second priority: Check if tank is in danger
        if (isInDanger(tank)) {
            // Find a safe move to avoid danger
            Action::Type safeMove = findSafeMove(tank);
            actions.push_back(Action(safeMove, tank));
            continue;
        }
        
        // Third priority: Try to position for shooting
        Action::Type move = findShootingPosition(tank);
        actions.push_back(Action(move, tank));
    }
    
    // Decrease shooting cooldown
    if (shootingCooldown > 0) {
        shootingCooldown--;
    }
    
    return actions;
} 