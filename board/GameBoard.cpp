#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "GameBoard.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Shell.h"
#include "../algorithms/Action.h"
#include "../constants/BoardConstants.h"
#include "../game_objects/Direction.h"

using namespace std;
using namespace BoardConstants;

// Constants for board elements

bool GameBoard::isWall(int x, int y) const {
    // Check if position is out of bounds
    if (x < 0 || x >= height || y < 0 || y >= width) {
        return true;
    }
    // Check if position contains a wall
    return board[x][y] == WALL || board[x][y] == DAMAGED_WALL;
}

void GameBoard::updateCooldowns() {
    // Decrease cooldown for all tanks
    for (auto it = tankShootCooldowns.begin(); it != tankShootCooldowns.end();) {
        if (--(it->second) <= 0) {
            it = tankShootCooldowns.erase(it);
        } else {
            ++it;
        }
    }
}

GameBoard::GameBoard(const vector<vector<char>>& initialBoard) 
    : board(initialBoard), gameOver(false), winner(0) {
    height = board.size();
    width = board[0].size();

    // Initialize tanks based on the board
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (board[i][j] == PLAYER1_TANK) {
                int location[2] = {i, j};
                int direction[2] = {0, 1}; // Default direction: right
                Tank tank(location, direction);
                tank.assignPlayerId(Player::PlayerId::P1);
                player1Tanks.push_back(tank);
            } else if (board[i][j] == PLAYER2_TANK) {
                int location[2] = {i, j};
                int direction[2] = {0, -1}; // Default direction: left
                Tank tank(location, direction);
                tank.assignPlayerId(Player::PlayerId::P2);
                player2Tanks.push_back(tank);
            }
        }
    }
}

GameBoard::~GameBoard() {
}

bool GameBoard::validateMove(const Action& action, int playerId) const {
    // Get the tank that's trying to move
    const Tank& targetTank = action.target();
    const auto& tankLocation = targetTank.getInfo().location;
    
    // Check if the tank belongs to the player
    const auto& playerTanks = getPlayerTanks(playerId);
    bool isPlayerTank = false;
    
    for (const auto& tank : playerTanks) {
        if (tank.getInfo().location == tankLocation) {
            isPlayerTank = true;
            break;
        }
    }
    
    if (!isPlayerTank) return false;

    return true;
}

void GameBoard::addToStepMoves(const Action& action, int playerId) {
    if (!validateMove(action, playerId)) {
        return;
    }
    
    const auto& tankLocation = action.target().getInfo().location;
    std::string moveStr = "Player " + std::to_string(playerId) + ": " + 
                         std::to_string(static_cast<int>(action.type())) +
                         " on tank at (" + 
                         std::to_string(tankLocation[0]) + "," +
                         std::to_string(tankLocation[1]) + ")";
    
    if (playerId == 1) {
        player1Moves.push_back(moveStr);
    } else {
        player2Moves.push_back(moveStr);
    }

    // Add the action to the step moves
    stepMoves.push_back(action);
}

char GameBoard::calculateNewPositionCharForTank(int x, int y, Player::PlayerId playerId) {
    char prevChar = board[x][y];
    
    // Check for collisions with different board elements
    switch (prevChar) {
        case BoardConstants::EMPTY_SPACE:
            if(playerId == Player::PlayerId::P1){
                return BoardConstants::PLAYER1_TANK; // Default to player 1 tank if empty
            } else {
                return BoardConstants::PLAYER2_TANK; // Default to player 2 tank if empty
            }
            
        case BoardConstants::PLAYER1_TANK:
        case BoardConstants::PLAYER2_TANK:
            return BoardConstants::MULTIPLE_TANKS; // Multiple tanks collision
            
        case BoardConstants::WALL:
            return BoardConstants::WALL_AND_TANK; // Wall + Tank collision
            
        case BoardConstants::DAMAGED_WALL:
            return BoardConstants::DAMAGED_WALL_AND_TANK; // Damaged Wall + Tank collision
            
        case BoardConstants::MINE:
            return BoardConstants::MINE_AND_TANK; // Mine + Tank collision
            
        case BoardConstants::SHELL:
            return BoardConstants::SHELL_AND_TANK; // Shell + Tank collision
        
        case BoardConstants::SHELL_AND_TANK:
            return BoardConstants::MULTIPLE_TANKS_AND_SHELL; // Shell + multiple tanks

        case BoardConstants::WALL_AND_SHELL:
            return BoardConstants::WALL_SHELL_AND_TANK; // Wall + Shell + Tank collision
            
        case BoardConstants::DAMAGED_WALL_AND_SHELL:
            return BoardConstants::DAMAGED_WALL_SHELL_AND_TANK; // Damaged Wall + Shell + Tank collision
            
        case BoardConstants::MINE_AND_SHELL:
            return BoardConstants::MINE_SHELL_AND_TANK; // Mine + Shell + Tank collision
            
        case BoardConstants::MULTIPLE_TANKS:
            return BoardConstants::MULTIPLE_TANKS; // Multiple tanks collision (already multiple tanks)
            
        case BoardConstants::MULTIPLE_TANKS_AND_SHELL:
            return BoardConstants::MULTIPLE_TANKS_AND_SHELL; // Multiple tanks + Shell collision
            
        case BoardConstants::SHELL_AND_SHELL:
            return BoardConstants::SHELL_AND_TANK; // Multiple shells + Tank collision
            
        case BoardConstants::WALL_SHELL_AND_TANK:
            return BoardConstants::WALL_SHELL_AND_TANK; // Wall + Shell + Tank collision (already has tank)
            
        case BoardConstants::DAMAGED_WALL_SHELL_AND_TANK:
            return BoardConstants::DAMAGED_WALL_SHELL_AND_TANK; // Damaged Wall + Shell + Tank collision (already has tank)
            
        case BoardConstants::MINE_SHELL_AND_TANK:
            return BoardConstants::MINE_SHELL_AND_TANK; // Mine + Shell + Tank collision (already has tank)
            
        default:
            std::cout << "Warning: Unexpected board character encountered: " << prevChar << std::endl;
            return prevChar; // Return the original character if no specific collision case
    }
}

char GameBoard::calculateNewPositionCharForShell(int x, int y) {
    char prevChar = board[x][y];
    
    // Check for collisions with different board elements
    switch (prevChar) {
        case BoardConstants::EMPTY_SPACE:
            return BoardConstants::SHELL; // Shell in empty space
            
        case BoardConstants::WALL:
            return BoardConstants::WALL_AND_SHELL; // Wall + Shell collision
            
        case BoardConstants::DAMAGED_WALL:
            return BoardConstants::DAMAGED_WALL_AND_SHELL; // Damaged Wall + Shell collision
            
        case BoardConstants::MINE:
            return BoardConstants::MINE_AND_SHELL; // Mine + Shell collision
        
        case BoardConstants::PLAYER1_TANK:
        case BoardConstants::PLAYER2_TANK:
            return BoardConstants::SHELL_AND_TANK; // Shell + Tank collision
            
        case BoardConstants::WALL_AND_TANK:
            return BoardConstants::WALL_SHELL_AND_TANK; // Wall + Shell + Tank collision
            
        case BoardConstants::DAMAGED_WALL_AND_TANK:
            return BoardConstants::DAMAGED_WALL_SHELL_AND_TANK; // Damaged Wall + Shell + Tank collision
            
        case BoardConstants::MINE_AND_TANK:
            return BoardConstants::MINE_SHELL_AND_TANK; // Mine + Shell + Tank collision
            
        case BoardConstants::MULTIPLE_TANKS:
            return BoardConstants::MULTIPLE_TANKS_AND_SHELL; // Multiple Tanks + Shell collision
            
        case BoardConstants::SHELL:
            return BoardConstants::SHELL_AND_SHELL; // Shell + Shell collision
            
        case BoardConstants::WALL_AND_SHELL:
            return BoardConstants::WALL_AND_SHELL; // Wall + Shell (already has shell)
            
        case BoardConstants::DAMAGED_WALL_AND_SHELL:
            return BoardConstants::DAMAGED_WALL_AND_SHELL; // Damaged Wall + Shell (already has shell)
            
        case BoardConstants::MINE_AND_SHELL:
            return BoardConstants::MINE_AND_SHELL; // Mine + Shell (already has shell)
            
        case BoardConstants::WALL_SHELL_AND_TANK:
            return BoardConstants::WALL_SHELL_AND_TANK; // Wall + Shell + Tank (already has shell)
            
        case BoardConstants::DAMAGED_WALL_SHELL_AND_TANK:
            return BoardConstants::DAMAGED_WALL_SHELL_AND_TANK; // Damaged Wall + Shell + Tank (already has shell)
            
        case BoardConstants::MINE_SHELL_AND_TANK:
            return BoardConstants::MINE_SHELL_AND_TANK; // Mine + Shell + Tank (already has shell)
            
        case BoardConstants::SHELL_AND_SHELL:
            return BoardConstants::SHELL_AND_SHELL; // Multiple Shells (already has shell)
            
        case BoardConstants::MULTIPLE_TANKS_AND_SHELL:
            return BoardConstants::MULTIPLE_TANKS_AND_SHELL; // Multiple Tanks + Shell (already has shell)
            
        default:
            std::cout << "Warning: Unexpected board character encountered for shell: " << prevChar << std::endl;
            return prevChar; // Return the original character if no specific collision case
    }
}

void GameBoard::performActions() {
    std::cout << "Processing " << stepMoves.size() << " actions..." << std::endl;
    printBoard();

    // Track which actions have been processed
    std::vector<bool> processedActions(stepMoves.size(), false);

    // Process all moves in stepMoves
    for (size_t i = 0; i < stepMoves.size(); i++) {
        // Skip if this action was already processed
        if (processedActions[i]) {
            continue;
        }

        const auto& action = stepMoves[i];
        const auto& targetTank = action.target();
        const auto& tankInfo = targetTank.getInfo();
        const array<int,2> location = tankInfo.location;

        // Get reference to the actual tank in the board
        auto& tanks = (targetTank.getPlayerId() == Player::PlayerId::P1) ? 
                     player1Tanks : player2Tanks;
        Tank* actualTank = nullptr;
        for (auto& tank : tanks) {
            if (tank.getInfo().location == location) {
                actualTank = &tank;
                break;
            }
        }
        if (!actualTank) {
            continue;
        }

        // Mark this action as processed
        processedActions[i] = true;

        if(actualTank->getIsMovingBackward() && (action.type() != Action::Type::MOVE_FORWARD || action.type() != Action::Type::MOVE_BACKWARD)){
            const_cast<Action&>(action).setType(Action::Type::MOVE_BACKWARD);
        }
        switch (action.type()) {
            case Action::Type::SHOOT: {
                std::cout << "Executing SHOOT action" << std::endl;
                // Create a new shell
                Shell* shell = actualTank->shoot();
                if (shell) {
                    // Add shell to bullets
                    bulletsPositions.push_back(*shell);
                    // Set cooldown for the tank
                    tankShootCooldowns[std::make_pair(location[0], location[1])] = BoardConstants::SHOOT_COOLDOWN;
                    delete shell;
                    std::cout << "Shell created and added to board" << std::endl;
                }
                break;
            }
            case Action::Type::MOVE_FORWARD: {
                if(actualTank->getIsMovingBackward()){
                    actualTank->moveForward();
                    break;
                }
                // Calculate new position
                array<int,2> newLocation = location;
                newLocation[0] += tankInfo.dir[0];
                newLocation[1] += tankInfo.dir[1];
                
                // Handle wrap-around
                if (newLocation[0] < 0) newLocation[0] = BOARD_WIDTH - 1;
                if (newLocation[0] >= BOARD_WIDTH) newLocation[0] = 0;
                if (newLocation[1] < 0) newLocation[1] = BOARD_HEIGHT - 1;
                if (newLocation[1] >= BOARD_HEIGHT) newLocation[1] = 0;

                // Check what's in the new position
                char newPosChar = board[newLocation[0]][newLocation[1]];
                
                if (newPosChar == BoardConstants::PLAYER1_TANK || 
                          newPosChar == BoardConstants::PLAYER2_TANK || newPosChar == BoardConstants::MULTIPLE_TANKS) {
                    // Check if the tank in that position has a pending move
                    bool shouldSkip = false;
                    for (size_t j = 0; j < stepMoves.size(); j++) {
                        if (processedActions[j]) continue; // Skip already processed actions
                        
                        const auto& pendingAction = stepMoves[j];
                        if (pendingAction.target().getInfo().location == newLocation) {
                            // Check if tanks are moving towards each other
                            const auto& otherTankDir = pendingAction.target().getInfo().dir;
                            if (otherTankDir[0] == -tankInfo.dir[0] && otherTankDir[1] == -tankInfo.dir[1]) {
                                // Tanks are moving towards each other, skip this move
                                shouldSkip = true;
                                break;
                            }
                        }
                    }
                    if (!shouldSkip) {
                        // Other tank is moving away or not moving, proceed with move
                        actualTank->move();
                        board[location[0]][location[1]] = handleMultipleTanksPosition(location[0], location[1]);
                        board[newLocation[0]][newLocation[1]] = calculateNewPositionCharForTank(newLocation[0], newLocation[1], actualTank->getPlayerId());
                    }
                } else {
                    // Empty space, proceed with move
                    actualTank->move();
                    board[location[0]][location[1]] = handleMultipleTanksPosition(location[0], location[1]);
                    board[newLocation[0]][newLocation[1]] = calculateNewPositionCharForTank(newLocation[0], newLocation[1], actualTank->getPlayerId());
                }
                break;
            }
            case Action::Type::MOVE_BACKWARD: {
                if(!actualTank->getIsMovingBackward()){
                    actualTank->moveBackward();
                    break;
                }
                else if (actualTank->getIsMovingBackward() && actualTank->getMoveBackwardCooldown() > 0) {
                    actualTank->decreaseMoveBackwardCooldown();
                    break;
                }
                actualTank->setFinishedMoveBackward();
                // Calculate new position
                array<int,2> newLocation = location;
                newLocation[0] -= tankInfo.dir[0];
                newLocation[1] -= tankInfo.dir[1];
                
                // Handle wrap-around
                if (newLocation[0] < 0) newLocation[0] = BOARD_WIDTH - 1;
                if (newLocation[0] >= BOARD_WIDTH) newLocation[0] = 0;
                if (newLocation[1] < 0) newLocation[1] = BOARD_HEIGHT - 1;
                if (newLocation[1] >= BOARD_HEIGHT) newLocation[1] = 0;

                // Check what's in the new position
                char newPosChar = board[newLocation[0]][newLocation[1]];
                
                if (newPosChar == BoardConstants::PLAYER1_TANK || newPosChar == BoardConstants::PLAYER2_TANK) {
                    // Check if the tank in that position has a pending move
                    bool shouldSkip = false;
                    for (size_t j = 0; j < stepMoves.size(); j++) {
                        if (processedActions[j]) continue; // Skip already processed actions
                        
                        const auto& pendingAction = stepMoves[j];
                        if (pendingAction.target().getInfo().location == newLocation) {
                            // Check if tanks are moving towards each other
                            const auto& otherTankDir = pendingAction.target().getInfo().dir;
                            if (otherTankDir[0] == tankInfo.dir[0] && otherTankDir[1] == tankInfo.dir[1]) {
                                // Tanks are moving towards each other, skip this move
                                shouldSkip = true;
                                break;
                            }
                        }
                    }
                    if (!shouldSkip) {
                        // Other tank is moving away or not moving, proceed with move
                        actualTank->move();
                        board[location[0]][location[1]] = handleMultipleTanksPosition(location[0], location[1]);
                        board[newLocation[0]][newLocation[1]] = calculateNewPositionCharForTank(newLocation[0], newLocation[1], actualTank->getPlayerId());
                    }
                } else {
                    // Empty space, proceed with move
                    actualTank->move();
                    board[location[0]][location[1]] = handleMultipleTanksPosition(location[0], location[1]);
                    board[newLocation[0]][newLocation[1]] = calculateNewPositionCharForTank(newLocation[0], newLocation[1], actualTank->getPlayerId());
                }
                break;
            }
            case Action::Type::TURN_R_45: {
                actualTank->turn(Turn::RIGHT_45);
                break;
            }
            case Action::Type::TURN_R_90: {
                actualTank->turn(Turn::RIGHT_90);
                break;
            }
            case Action::Type::TURN_L_45: {
                actualTank->turn(Turn::LEFT_45);
                break;
            }
            case Action::Type::TURN_L_90: {
                actualTank->turn(Turn::LEFT_90);
                break;
            }
            case Action::Type::NOP:
                break;
            default:
                cerr << "Unknown action type" << endl;
                break;
        }
        if(action.type() != Action::Type::MOVE_BACKWARD){
            actualTank->cancelMoveBackward();
        }
    }
    
    // Clear the step moves after processing
    stepMoves.clear();
    std::cout << "All actions processed. This is the board after processing actions:" << std::endl;
    printBoard();
}

bool GameBoard::isShellAtPosition(int x, int y) const {
    for (const auto& shell : bulletsPositions) {
        const auto& location = shell.getInfo().location;
        if (location[0] == x && location[1] == y) {
            return true;
        }
    }
    return false;
}

void GameBoard::moveShells() {
    // Create a copy of bulletsPositions to iterate over
    auto bullets = bulletsPositions;
    bulletsPositions.clear(); // Clear the original list to rebuild it
    
    // First pass: check for adjacent shells moving towards each other
    std::vector<bool> shellsToKeep(bullets.size(), true);
    for (size_t i = 0; i < bullets.size(); i++) {
        if (!shellsToKeep[i]) continue; // Skip if already marked for removal
        
        const auto& shell1 = bullets[i];
        const auto& loc1 = shell1.getInfo().location;
        const auto& dir1 = shell1.getInfo().dir;
        
        for (size_t j = i + 1; j < bullets.size(); j++) {
            if (!shellsToKeep[j]) continue; // Skip if already marked for removal
            
            const auto& shell2 = bullets[j];
            const auto& loc2 = shell2.getInfo().location;
            const auto& dir2 = shell2.getInfo().dir;
            
            // Calculate Manhattan distance between shells
            int dx = abs(loc1[0] - loc2[0]);
            int dy = abs(loc1[1] - loc2[1]);
            
            // Check if shells are adjacent (including diagonal) and moving towards each other
            if ((dx <= 1 && dy <= 1) && (dx + dy) > 0) { // Adjacent (including diagonal)
                if (dir1[0] == -dir2[0] && dir1[1] == -dir2[1]) { // Moving towards each other
                    std::cout << "Shells are adjacent and moving towards each other" << std::endl;
                    std::cout << "Shell 1: " << loc1[0] << ", " << loc1[1] << " with direction " << dir1[0] << ", " << dir1[1] << std::endl;
                    std::cout << "Shell 2: " << loc2[0] << ", " << loc2[1] << " with direction " << dir2[0] << ", " << dir2[1] << std::endl;
                    shellsToKeep[i] = false;
                    shellsToKeep[j] = false;
                    // Handle shell1
                    if (!shell1.getJustFired()) {
                        board[loc1[0]][loc1[1]] = BoardConstants::EMPTY_SPACE;
                    }
                    
                    // Handle shell2
                    if (!shell2.getJustFired()) {
                        board[loc2[0]][loc2[1]] = BoardConstants::EMPTY_SPACE;
                    }
                }
            }
        }
    }
    
    // Second pass: move remaining shells
    for (size_t i = 0; i < bullets.size(); i++) {
        if (!shellsToKeep[i]) continue; // Skip shells marked for removal
        
        auto& shell = bullets[i];
        // Get current position
        const auto& location = shell.getInfo().location;
        int prevX = location[0];
        int prevY = location[1];
        
        // Move the shell
        shell.move();
        
        // Get new position
        const auto& newLocation = shell.getInfo().location;
        int newX = newLocation[0];
        int newY = newLocation[1];
        
        // Handle the previous position - only if no other shells remain
        if (!isShellAtPosition(prevX, prevY)) {
            char prevChar = board[prevX][prevY];
            switch (prevChar) {
                case BoardConstants::MINE_AND_SHELL:
                    board[prevX][prevY] = BoardConstants::MINE;
                    break;
                    
                case BoardConstants::MINE_SHELL_AND_TANK:
                    board[prevX][prevY] = BoardConstants::MINE_AND_TANK;
                    break;
                    
                case BoardConstants::SHELL_AND_TANK:
                    // Shell and single tank case - change to just the tank
                    board[prevX][prevY] = handleMultipleTanksPosition(prevX, prevY);
                    break;
                    
                case BoardConstants::MULTIPLE_TANKS_AND_SHELL:
                    // Multiple tanks and shell case - change to multiple tanks
                    board[prevX][prevY] = BoardConstants::MULTIPLE_TANKS;
                    break;
                    
                case BoardConstants::SHELL_AND_SHELL:
                    // Shell collisions case - check if there are still shells
                    {
                        int shellCount = 0;
                        for (size_t j = 0; j < bullets.size(); j++) {
                            if (!shellsToKeep[j]) continue;
                            const auto& otherShell = bullets[j];
                            const auto& otherLocation = otherShell.getInfo().location;
                            if (otherLocation[0] == prevX && otherLocation[1] == prevY) {
                                shellCount++;
                            }
                        }
                        board[prevX][prevY] = (shellCount > 1) ? BoardConstants::SHELL_AND_SHELL : BoardConstants::SHELL;
                    }
                    break;
                case BoardConstants::PLAYER1_TANK:
                    if(shell.getJustFired()){
                        board[prevX][prevY] = BoardConstants::PLAYER1_TANK;
                        shell.setJustFired(false);
                    }
                    break;
                case BoardConstants::PLAYER2_TANK:
                    if(shell.getJustFired()){
                        board[prevX][prevY] = BoardConstants::PLAYER2_TANK;
                        shell.setJustFired(false);
                    }
                    break;
                default:
                    // For any other case, set to empty space
                    board[prevX][prevY] = BoardConstants::EMPTY_SPACE;
                    break;
            }
            shell.setJustFired(false);
        }
        
        // Handle the new position
        board[newX][newY] = calculateNewPositionCharForShell(newX, newY);
        
        // Add the moved shell to the updated list
        bulletsPositions.push_back(shell);
    }
}

void GameBoard::removeBulletsAtPosition(int x, int y) {
    bulletsPositions.erase(
        std::remove_if(bulletsPositions.begin(), bulletsPositions.end(),
            [x, y](const Shell& shell) {
                const auto& location = shell.getInfo().location;
                return location[0] == x && location[1] == y;
            }),
        bulletsPositions.end()
    );
}

void GameBoard::killTanksAtPosition(int x, int y, const string& cause) {
    // Kill player 1 tanks at position
    for (auto& tank : player1Tanks) {
        const auto& location = tank.getInfo().location;
        if (location[0] == x && location[1] == y) {
            tankDeaths.push_back({{x, y}, 1, cause});
            tank.killTank();
        }
    }
    // Kill player 2 tanks at position
    for (auto& tank : player2Tanks) {
        const auto& location = tank.getInfo().location;
        if (location[0] == x && location[1] == y) {
            tankDeaths.push_back({{x, y}, 2, cause});
            tank.killTank();
        }
    }
}

void GameBoard::moveTankBack(int x, int y) {
    // Check player 1 tanks
    for (auto& tank : player1Tanks) {
        const auto& location = tank.getInfo().location;
        if (location[0] == x && location[1] == y) {
            tank.moveBack();
            const auto& prevPos = tank.getPrevPosition();
            board[prevPos[0]][prevPos[1]] = calculateNewPositionCharForTank(prevPos[0], prevPos[1], tank.getPlayerId());
            backwardMoves.push_back({prevPos[0], prevPos[1]});
        }
    }
    // Check player 2 tanks
    for (auto& tank : player2Tanks) {
        const auto& location = tank.getInfo().location;
        if (location[0] == x && location[1] == y) {
            tank.moveBack();
            const auto& prevPos = tank.getPrevPosition();
            board[prevPos[0]][prevPos[1]] = calculateNewPositionCharForTank(prevPos[0], prevPos[1], tank.getPlayerId());
            backwardMoves.push_back({prevPos[0], prevPos[1]});
        }
    }
}

int GameBoard::countShellsAtPosition(int x, int y) const {
    int count = 0;
    for (const auto& shell : bulletsPositions) {
        const auto& location = shell.getInfo().location;
        if (location[0] == x && location[1] == y) {
            count++;
        }
    }
    return count;
}

void GameBoard::handleCollision(int i, int j, char currentChar) {
    switch (currentChar) {
        case BoardConstants::MULTIPLE_TANKS: {
            // Case 1: Multiple tanks - everything dies/disappears
            killTanksAtPosition(i, j, "Multiple tanks collision");
            board[i][j] = BoardConstants::EMPTY_SPACE;
            break;
        }
        
        case BoardConstants::WALL_AND_TANK:
        case BoardConstants::DAMAGED_WALL_AND_TANK: {
            // Case 2: Wall/Damaged Wall and Tank - move tank back
            moveTankBack(i, j);
            board[i][j] = (currentChar == BoardConstants::WALL_AND_TANK) ? BoardConstants::WALL : BoardConstants::DAMAGED_WALL;
            break;
        }
        
        case BoardConstants::SHELL_AND_SHELL: {
            // Case 3: Shell collision - both disappear
            removeBulletsAtPosition(i, j);
            board[i][j] = BoardConstants::EMPTY_SPACE;
            break;
        }
        
        case BoardConstants::SHELL_AND_TANK:
        case BoardConstants::MULTIPLE_TANKS_AND_SHELL: {
            // Case 4: Shell and Tank - everything disappears
            killTanksAtPosition(i, j, "Shell hit");
            removeBulletsAtPosition(i, j);
            board[i][j] = BoardConstants::EMPTY_SPACE;
            break;
        }
        
        case BoardConstants::MINE_AND_TANK: {
            // Case 5: Mine and Tank - tank dies, mine disappears
            killTanksAtPosition(i, j, "Mine explosion");
            board[i][j] = BoardConstants::EMPTY_SPACE;
            break;
        }
        
        case BoardConstants::WALL_AND_SHELL: {
            // Case 6: Wall and Shell
            int shellCount = countShellsAtPosition(i, j);
            
            if (shellCount > 1) {
                // More than one shell - everything disappears
                board[i][j] = BoardConstants::EMPTY_SPACE;
            } else {
                // One shell - wall becomes damaged
                board[i][j] = BoardConstants::DAMAGED_WALL;
            }
            removeBulletsAtPosition(i, j);
            break;
        }
        
        case BoardConstants::DAMAGED_WALL_AND_SHELL: {
            // Case 7: Damaged Wall and Shell - both disappear
            removeBulletsAtPosition(i, j);
            board[i][j] = BoardConstants::EMPTY_SPACE;
            break;
        }
        
        case BoardConstants::DAMAGED_WALL_SHELL_AND_TANK: {
            // Case 8: Damaged Wall, Shell, and Tank
            int shellCount = countShellsAtPosition(i, j);
            
            if (shellCount > 1) {
                // More than one shell - everything dies/disappears
                killTanksAtPosition(i, j, "Multiple shells hit");
                board[i][j] = BoardConstants::EMPTY_SPACE;
            } else {
                // One shell - calculate appropriate tank character
                board[i][j] = handleMultipleTanksPosition(i, j);
            }
            removeBulletsAtPosition(i, j);
            break;
        }
        
        case BoardConstants::WALL_SHELL_AND_TANK: {
            // Case 9: Wall, Shell, and Tank
            int shellCount = countShellsAtPosition(i, j);
            
            if (shellCount == 1) {
                // One shell - move tank back to previous position and wall becomes damaged
                moveTankBack(i, j);
                board[i][j] = BoardConstants::DAMAGED_WALL;
            } else if (shellCount == 2) {
                // Two shells - shells disappear, tank stays
                board[i][j] = handleMultipleTanksPosition(i, j);
                if(board[i][j] == BoardConstants::MULTIPLE_TANKS){
                    killTanksAtPosition(i, j, "Multiple tanks collision");
                    board[i][j] = BoardConstants::EMPTY_SPACE;
                }
            } else {
                // More than two shells or multiple tanks - everything dies/disappears
                killTanksAtPosition(i, j, "Multiple shells hit");
                board[i][j] = BoardConstants::EMPTY_SPACE;
            }
            removeBulletsAtPosition(i, j);
            break;
        }
        
        case BoardConstants::MINE_AND_SHELL: {
            // Case 13: Mine and Shell - stays the same
            break;
        }
        
        case BoardConstants::MINE_SHELL_AND_TANK: {
            // Case 12: Mine, Shell, and Tank - kill tanks, remove shells, set to empty
            killTanksAtPosition(i, j, "Mine explosion with shell");
            removeBulletsAtPosition(i, j);
            board[i][j] = BoardConstants::EMPTY_SPACE;
            break;
        }
        
        default:
            // For any other collision character, set to empty space
            if (BoardConstants::isCollision(currentChar)) {
                board[i][j] = BoardConstants::EMPTY_SPACE;
            }
            break;
    }
}

void GameBoard::handleCollisions() {
    std::cout << "Handling collisions..." << std::endl;
    // Clear backward moves from previous step
    backwardMoves.clear();
    
    // First pass: handle all collisions
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            char currentChar = board[i][j];
            if (BoardConstants::isCollision(currentChar)) {
                std::cout << "Handling collision at (" << i << "," << j << "): " << currentChar << std::endl;
                handleCollision(i, j, currentChar);
            }
        }
    }
    
    // Second pass: handle backward moves
    for (const auto& pos : backwardMoves) {
        char currentChar = board[pos.first][pos.second];
        if (BoardConstants::isCollision(currentChar)) {
            std::cout << "Handling backward move collision at (" << pos.first << "," << pos.second << ")" << std::endl;
            handleCollision(pos.first, pos.second, currentChar);
        }
    }
    
    // Clean up dead tanks
    player1Tanks.erase(
        std::remove_if(player1Tanks.begin(), player1Tanks.end(),
            [](const Tank& tank) { return !tank.getIsTankAlive(); }),
        player1Tanks.end()
    );
    player2Tanks.erase(
        std::remove_if(player2Tanks.begin(), player2Tanks.end(),
            [](const Tank& tank) { return !tank.getIsTankAlive(); }),
        player2Tanks.end()
    );

    std::cout << "Tanks after cleanup: P1=" << player1Tanks.size() << " P2=" << player2Tanks.size() << std::endl;

    // Check if game is over after handling collisions
    checkGameOver();
    std::cout << "Collision handling complete" << std::endl;
}

void GameBoard::executeStep() {
    std::cout << "Executing game step..." << std::endl;
    moveShells();
    std::cout << "Shells moved" << std::endl;
    handleCollisions();
    if (gameOver) {
        std::cout << "Game over" << std::endl;
        return;
    }
    std::cout << "Collisions handled" << std::endl;
    moveShells();
    std::cout << "Shells moved again" << std::endl;
    handleCollisions();
    if (gameOver) {
        std::cout << "Game over" << std::endl;
        return;
    }
    std::cout << "Collisions handled again" << std::endl;
    performActions();
    std::cout << "Actions performed" << std::endl;
    handleCollisions();
    std::cout << "Collisions handled third time" << std::endl;
    // Update cooldowns
    updateCooldowns();
    std::cout << "Cooldowns updated" << std::endl;
}

vector<pair<int, int>> GameBoard::getPlayerTankPositions(int playerId) const {
    vector<pair<int, int>> positions;
    const auto& tanks = (playerId == 1) ? player1Tanks : player2Tanks;
    
    for (const auto& tank : tanks) {
        const auto& location = tank.getInfo().location;
        positions.push_back({location[0], location[1]});
    }
    return positions;
}

vector<pair<int, int>> GameBoard::getEnemyTankPositions(int playerId) const {
    vector<pair<int, int>> positions;
    const auto& enemyTanks = (playerId == 1) ? player2Tanks : player1Tanks;
    
    for (const auto& tank : enemyTanks) {
        const auto& location = tank.getInfo().location;
        positions.push_back({location[0], location[1]});
    }
    return positions;
}

void GameBoard::saveGameMoves(const string& filename) const {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error opening output file" << endl;
        return;
    }

    // Save player 1 moves
    outputFile << "Player 1 moves:" << endl;
    for (const auto& move : player1Moves) {
        outputFile << move << endl;
    }

    // Save player 2 moves
    outputFile << "\nPlayer 2 moves:" << endl;
    for (const auto& move : player2Moves) {
        outputFile << move << endl;
    }

    // Save game result
    outputFile << "\nGame Result:" << endl;
    if (winner == 0) {
        outputFile << "Tie - Both players lost all their tanks" << endl;
    } else {
        outputFile << "Player " << winner << " won!" << endl;
    }

    // Save tank deaths in chronological order
    outputFile << "\nTank Deaths (in order of occurrence):" << endl;
    for (const auto& death : tankDeaths) {
        outputFile << "Player " << death.playerId << "'s tank at position (" 
                  << death.position.first << "," << death.position.second 
                  << ") died from: " << death.cause << endl;
    }

    outputFile.close();
}

char GameBoard::handleMultipleTanksPosition(int x, int y) {
    // Count tanks at this position and track their player IDs
    int p1TankCount = 0;
    int p2TankCount = 0;
    
    // Check player 1 tanks
    for (const auto& tank : player1Tanks) {
        const auto& location = tank.getInfo().location;
        if (location[0] == x && location[1] == y) {
            p1TankCount++;
        }
    }
    
    // Check player 2 tanks
    for (const auto& tank : player2Tanks) {
        const auto& location = tank.getInfo().location;
        if (location[0] == x && location[1] == y) {
            p2TankCount++;
        }
    }
    
    // Check if there are any shells at this position
    bool hasShell = isShellAtPosition(x, y);
    
    // If there's still more than one tank
    if ((p1TankCount + p2TankCount) > 1) {
        return hasShell ? BoardConstants::MULTIPLE_TANKS_AND_SHELL : BoardConstants::MULTIPLE_TANKS;
    }
    
    // If only one tank remains
    if (p1TankCount == 1) {
        return hasShell ? BoardConstants::SHELL_AND_TANK : BoardConstants::PLAYER1_TANK;
    } else if (p2TankCount == 1) {
        return hasShell ? BoardConstants::SHELL_AND_TANK : BoardConstants::PLAYER2_TANK;
    }
    
    // If no tanks remain but there's a shell
    if (hasShell) {
        return BoardConstants::SHELL;
    }
    
    // If nothing remains
    return BoardConstants::EMPTY_SPACE;
}

bool GameBoard::checkGameOver() {
    bool p1HasTanks = false;
    bool p2HasTanks = false;

    // Check if any player 1 tanks are alive
    for (const auto& tank : player1Tanks) {
        if (tank.getIsTankAlive()) {
            p1HasTanks = true;
            break;
        }
    }

    // Check if any player 2 tanks are alive
    for (const auto& tank : player2Tanks) {
        if (tank.getIsTankAlive()) {
            p2HasTanks = true;
            break;
        }
    }

    // If both players have no tanks, it's a tie
    if (!p1HasTanks && !p2HasTanks) {
        gameOver = true;
        winner = 0; // 0 represents a tie
        return true;
    }
    // If only one player has tanks, they win
    else if (!p1HasTanks || !p2HasTanks) {
        gameOver = true;
        winner = p1HasTanks ? 1 : 2; // 1 for player 1, 2 for player 2
        return true;
    }

    return false;
}

void GameBoard::printBoard() const {
    std::cout << "\nCurrent Board State:" << std::endl;
    for (const auto& row : board) {
        for (char c : row) {
            std::cout << c;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
