#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <memory>
#include "GameBoard.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Shell.h"
#include "../algorithms/Action.h"
#include "../constants/BoardConstants.h"
#include "../game_objects/Direction.h"
#include "../algorithms/Algorithm.h"

using namespace std;
using namespace BoardConstants;

// Direction constants
constexpr int RIGHT_DIRECTION[2] = {0, 1};
constexpr int LEFT_DIRECTION[2] = {0, -1};

// Constants for board elements

bool GameBoard::isWall(int x, int y) const {
    // Check if position is out of bounds
    if (x < 0 || x >= height || y < 0 || y >= width) {
        return true;
    }
    // Check if position contains a wall
    return board[x][y] == WALL || board[x][y] == DAMAGED_WALL;
}

GameBoard::GameBoard(const vector<vector<char>>& initialBoard, int maxSteps) 
    : board(initialBoard), gameOver(false), winner(0), maxStepCount(maxSteps) {
    height = board.size();
    width = board[0].size();

    cout << "Initializing game board with dimensions " << height << "x" << width << endl;
    cout << "Initial board state:" << endl;
    for (const auto& row : board) {
        for (char c : row) {
            cout << c;
        }
        cout << endl;
    }

    // Initialize tanks based on the board
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (board[i][j] == PLAYER1_TANK) {
                cout << "Found player 1 tank at (" << i << "," << j << ")" << endl;
                int location[2] = {i, j};
                Tank tank(location, const_cast<int*>(RIGHT_DIRECTION));
                tank.assignPlayerId(1);
                player1Tanks.push_back(tank);
            } else if (board[i][j] == PLAYER2_TANK) {
                cout << "Found player 2 tank at (" << i << "," << j << ")" << endl;
                int location[2] = {i, j};
                Tank tank(location, const_cast<int*>(LEFT_DIRECTION));
                tank.assignPlayerId(2);
                player2Tanks.push_back(tank);
            }
        }
    }

    cout << "Initialized " << player1Tanks.size() << " player 1 tanks and " << player2Tanks.size() << " player 2 tanks" << endl;
}

bool GameBoard::validateMove(const Action& action, int playerId) {
    // Get the tank that's trying to move
    const Tank& targetTank = action.target();
    const auto& tankLocation = targetTank.getInfo().location;
    
    // Check if the tank is dead
    if (!targetTank.getIsTankAlive()) {
        return false;
    }
    
    // Check if the tank belongs to the player
    auto& playerTanks = getPlayerTanks(playerId);
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
    
    // Convert action type to descriptive string
    std::string actionStr;
    switch (action.type()) {
        case Action::Type::SHOOT:
            actionStr = "SHOOT";
            break;
        case Action::Type::MOVE_FORWARD:
            actionStr = "MOVE_FORWARD";
            break;
        case Action::Type::MOVE_BACKWARD:
            actionStr = "MOVE_BACKWARD";
            break;
        case Action::Type::TURN_R_45:
            actionStr = "TURN_RIGHT_45";
            break;
        case Action::Type::TURN_R_90:
            actionStr = "TURN_RIGHT_90";
            break;
        case Action::Type::TURN_L_45:
            actionStr = "TURN_LEFT_45";
            break;
        case Action::Type::TURN_L_90:
            actionStr = "TURN_LEFT_90";
            break;
        case Action::Type::NOP:
            actionStr = "NO_OPERATION";
            break;
        default:
            actionStr = "UNKNOWN_ACTION";
            break;
    }
    
    std::string moveStr = "Player " + std::to_string(playerId) + ": " + 
                         actionStr +
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

char GameBoard::calculateNewPositionCharForTank(int x, int y, int playerId) {
    char prevChar = board[x][y];
    
    // Check for collisions with different board elements
    switch (prevChar) {
        case EMPTY_SPACE:
            if(playerId == 1){
                return PLAYER1_TANK; // Default to player 1 tank if empty
            } else {
                return PLAYER2_TANK; // Default to player 2 tank if empty
            }
            
        case PLAYER1_TANK:
        case PLAYER2_TANK:
            return MULTIPLE_TANKS; // Multiple tanks collision
            
        case WALL:
            return WALL_AND_TANK; // Wall + Tank collision
            
        case DAMAGED_WALL:
            return DAMAGED_WALL_AND_TANK; // Damaged Wall + Tank collision
            
        case MINE:
            return MINE_AND_TANK; // Mine + Tank collision
            
        case SHELL:
            return SHELL_AND_TANK; // Shell + Tank collision
        
        case SHELL_AND_TANK:
            return MULTIPLE_TANKS_AND_SHELL; // Shell + multiple tanks

        case WALL_AND_SHELL:
            return WALL_SHELL_AND_TANK; // Wall + Shell + Tank collision
            
        case DAMAGED_WALL_AND_SHELL:
            return DAMAGED_WALL_SHELL_AND_TANK; // Damaged Wall + Shell + Tank collision
            
        case MINE_AND_SHELL:
            return MINE_SHELL_AND_TANK; // Mine + Shell + Tank collision
            
        case MULTIPLE_TANKS:
            return MULTIPLE_TANKS; // Multiple tanks collision (already multiple tanks)
            
        case MULTIPLE_TANKS_AND_SHELL:
            return MULTIPLE_TANKS_AND_SHELL; // Multiple tanks + Shell collision
            
        case SHELL_AND_SHELL:
            return SHELL_AND_TANK; // Multiple shells + Tank collision
            
        case WALL_SHELL_AND_TANK:
            return WALL_SHELL_AND_TANK; // Wall + Shell + Tank collision (already has tank)
            
        case DAMAGED_WALL_SHELL_AND_TANK:
            return DAMAGED_WALL_SHELL_AND_TANK; // Damaged Wall + Shell + Tank collision (already has tank)
            
        case MINE_SHELL_AND_TANK:
            return MINE_SHELL_AND_TANK; // Mine + Shell + Tank collision (already has tank)
            
        default:
            std::cout << "Warning: Unexpected board character encountered: " << prevChar << std::endl;
            return prevChar; // Return the original character if no specific collision case
    }
}

char GameBoard::calculateNewPositionCharForShell(int x, int y) {
    char prevChar = board[x][y];
    
    // Check for collisions with different board elements
    switch (prevChar) {
        case EMPTY_SPACE:
            return SHELL; // Shell in empty space
            
        case WALL:
            return WALL_AND_SHELL; // Wall + Shell collision
            
        case DAMAGED_WALL:
            return DAMAGED_WALL_AND_SHELL; // Damaged Wall + Shell collision
            
        case MINE:
            return MINE_AND_SHELL; // Mine + Shell collision
        
        case PLAYER1_TANK:
        case PLAYER2_TANK:
            return SHELL_AND_TANK; // Shell + Tank collision
            
        case WALL_AND_TANK:
            return WALL_SHELL_AND_TANK; // Wall + Shell + Tank collision
            
        case DAMAGED_WALL_AND_TANK:
            return DAMAGED_WALL_SHELL_AND_TANK; // Damaged Wall + Shell + Tank collision
            
        case MINE_AND_TANK:
            return MINE_SHELL_AND_TANK; // Mine + Shell + Tank collision
            
        case MULTIPLE_TANKS:
            return MULTIPLE_TANKS_AND_SHELL; // Multiple Tanks + Shell collision
            
        case SHELL:
            return SHELL_AND_SHELL; // Shell + Shell collision
            
        case WALL_AND_SHELL:
            return WALL_AND_SHELL; // Wall + Shell (already has shell)
            
        case DAMAGED_WALL_AND_SHELL:
            return DAMAGED_WALL_AND_SHELL; // Damaged Wall + Shell (already has shell)
            
        case MINE_AND_SHELL:
            return MINE_AND_SHELL; // Mine + Shell (already has shell)
            
        case WALL_SHELL_AND_TANK:
            return WALL_SHELL_AND_TANK; // Wall + Shell + Tank (already has shell)
            
        case DAMAGED_WALL_SHELL_AND_TANK:
            return DAMAGED_WALL_SHELL_AND_TANK; // Damaged Wall + Shell + Tank (already has shell)
            
        case MINE_SHELL_AND_TANK:
            return MINE_SHELL_AND_TANK; // Mine + Shell + Tank (already has shell)
            
        case SHELL_AND_SHELL:
            return SHELL_AND_SHELL; // Multiple Shells (already has shell)
            
        case MULTIPLE_TANKS_AND_SHELL:
            return MULTIPLE_TANKS_AND_SHELL; // Multiple Tanks + Shell (already has shell)
            
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
        auto& tanks = (targetTank.getPlayerId() == 1) ? 
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
                auto shell = actualTank->shoot();
                if (shell) {
                    // Add shell to bullets
                    bulletsPositions.push_back(*shell);
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
                array<int,2> newLocation = actualTank->potentialMove();

                // Check what's in the new position
                char newPosChar = board[newLocation[0]][newLocation[1]];
                
                if (newPosChar == PLAYER1_TANK || 
                          newPosChar == PLAYER2_TANK || newPosChar == MULTIPLE_TANKS) {
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
                } else if (newPosChar == WALL || newPosChar == DAMAGED_WALL){
                    //Do not move tank
                } else {
                    // Empty space or mine, proceed with move
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
                array<int,2> newLocation = actualTank->potentialMove();

                // Check what's in the new position
                char newPosChar = board[newLocation[0]][newLocation[1]];
                
                if (newPosChar == PLAYER1_TANK || newPosChar == PLAYER2_TANK) {
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
                } else if (newPosChar == WALL || newPosChar == DAMAGED_WALL){
                    //Do not move tank
                } else {
                    // Empty space or mine, proceed with move
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
        actualTank->decreaseShootingCooldown();
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
                        board[loc1[0]][loc1[1]] = EMPTY_SPACE;
                    }
                    
                    // Handle shell2
                    if (!shell2.getJustFired()) {
                        board[loc2[0]][loc2[1]] = EMPTY_SPACE;
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
                case MINE_AND_SHELL:
                    board[prevX][prevY] = MINE;
                    break;
                    
                case MINE_SHELL_AND_TANK:
                    board[prevX][prevY] = MINE_AND_TANK;
                    break;
                    
                case SHELL_AND_TANK:
                    // Shell and single tank case - change to just the tank
                    board[prevX][prevY] = handleMultipleTanksPosition(prevX, prevY);
                    break;
                    
                case MULTIPLE_TANKS_AND_SHELL:
                    // Multiple tanks and shell case - change to multiple tanks
                    board[prevX][prevY] = MULTIPLE_TANKS;
                    break;
                    
                case SHELL_AND_SHELL:
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
                        board[prevX][prevY] = (shellCount > 1) ? SHELL_AND_SHELL : SHELL;
                    }
                    break;
                case PLAYER1_TANK:
                    if(shell.getJustFired()){
                        board[prevX][prevY] = PLAYER1_TANK;
                        shell.setJustFired(false);
                    }
                    break;
                case PLAYER2_TANK:
                    if(shell.getJustFired()){
                        board[prevX][prevY] = PLAYER2_TANK;
                        shell.setJustFired(false);
                    }
                    break;
                default:
                    // For any other case, set to empty space
                    board[prevX][prevY] = EMPTY_SPACE;
                    break;
            }
            shell.setJustFired(false);
        }
        
        // Handle the new position
        char newChar = calculateNewPositionCharForShell(newX, newY);
        board[newX][newY] = newChar;
        
        // Add the moved shell to the updated list
        bulletsPositions.push_back(shell);
    }
    
    // Print debug information
    std::cout << "After moving shells, bulletsPositions size: " << bulletsPositions.size() << std::endl;
    printBoard();
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
            std::cout << "Moving tank back to position (" << prevPos[0] << "," << prevPos[1] << ")" << std::endl;
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

void GameBoard::clearPosition(int x, int y, const string& cause) {
    killTanksAtPosition(x, y, cause);
    removeBulletsAtPosition(x, y);
    board[x][y] = EMPTY_SPACE;
}

void GameBoard::handleCollision(int i, int j, char currentChar) {
    switch (currentChar) {
        case MULTIPLE_TANKS: {
            // Case 1: Multiple tanks - everything dies/disappears
            clearPosition(i, j, "Multiple tanks collision");
            break;
        }
        
        case WALL_AND_TANK:
        case DAMAGED_WALL_AND_TANK: {
            // Case 2: Wall/Damaged Wall and Tank - move tank back
            moveTankBack(i, j);
            board[i][j] = (currentChar == WALL_AND_TANK) ? WALL : DAMAGED_WALL;
            break;
        }
        
        case SHELL_AND_SHELL: {
            // Case 3: Shell collision - both disappear
            clearPosition(i, j, "Shell collision");
            break;
        }
        
        case SHELL_AND_TANK:
        case MULTIPLE_TANKS_AND_SHELL: {
            // Case 4: Shell and Tank - everything disappears
            clearPosition(i, j, "Shell hit");
            break;
        }
        
        case MINE_AND_TANK: {
            // Case 5: Mine and Tank - tank dies, mine disappears
            clearPosition(i, j, "Mine explosion");
            break;
        }
        
        case WALL_AND_SHELL: {
            // Case 6: Wall and Shell
            int shellCount = countShellsAtPosition(i, j);
            
            if (shellCount > 1) {
                // More than one shell - everything disappears
                clearPosition(i, j, "Multiple shells hit wall");
            } else {
                // One shell - wall becomes damaged
                board[i][j] = DAMAGED_WALL;
            }
            removeBulletsAtPosition(i, j);
            break;
        }
        
        case DAMAGED_WALL_AND_SHELL: {
            // Case 7: Damaged Wall and Shell - both disappear
            clearPosition(i, j, "Shell hit damaged wall");
            break;
        }
        
        case DAMAGED_WALL_SHELL_AND_TANK: {
            // Case 8: Damaged Wall, Shell, and Tank
            int shellCount = countShellsAtPosition(i, j);
            
            if (shellCount > 1) {
                // More than one shell - everything dies/disappears
                clearPosition(i, j, "Multiple shells hit");
            } else {
                // One shell - calculate appropriate tank character
                board[i][j] = handleMultipleTanksPosition(i, j);
            }
            removeBulletsAtPosition(i, j);
            break;
        }
        
        case WALL_SHELL_AND_TANK: {
            // Case 9: Wall, Shell, and Tank
            int shellCount = countShellsAtPosition(i, j);
            
            if (shellCount == 1) {
                // One shell - move tank back to previous position and wall becomes damaged
                moveTankBack(i, j);
                board[i][j] = DAMAGED_WALL;
            } else if (shellCount == 2) {
                // Two shells - shells disappear, tank stays
                board[i][j] = handleMultipleTanksPosition(i, j);
                if(board[i][j] == MULTIPLE_TANKS){
                    clearPosition(i, j, "Multiple tanks collision");
                }
            } else {
                // More than two shells or multiple tanks - everything dies/disappears
                clearPosition(i, j, "Multiple shells hit");
            }
            removeBulletsAtPosition(i, j);
            break;
        }
        
        case MINE_AND_SHELL: {
            // Case 13: Mine and Shell - stays the same
            break;
        }
        
        case MINE_SHELL_AND_TANK: {
            // Case 12: Mine, Shell, and Tank - kill tanks, remove shells, set to empty
            clearPosition(i, j, "Mine explosion with shell");
            break;
        }
        
        default:
            // For any other collision character, set to empty space
            if (isCollision(currentChar)) {
                clearPosition(i, j, "Unknown collision");
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
            if (isCollision(currentChar)) {
                std::cout << "Handling collision at (" << i << "," << j << "): " << currentChar << std::endl;
                handleCollision(i, j, currentChar);
            }
        }
    }
    
    // Second pass: handle backward moves
    for (const auto& pos : backwardMoves) {
        char currentChar = board[pos.first][pos.second];
        if (isCollision(currentChar)) {
            std::cout << "Handling backward move collision at (" << pos.first << "," << pos.second << ")" << std::endl;
            handleCollision(pos.first, pos.second, currentChar);
        }
    }
    // Check if game is over after handling collisions
    checkGameOver();
}

void GameBoard::executeStep() {
    static int stepCount = 0;
    stepCount++;
    
    std::cout << "Executing game step " << stepCount << "..." << std::endl;
    
    // Check if we've exceeded max steps
    if (stepCount > maxStepCount) {
        std::cout << "Game ended due to exceeding " << maxStepCount << " steps" << std::endl;
        gameOver = true;
        winner = 0; // Tie game
        return;
    }

    std::cout << "Handling shells..." << std::endl;
    
    for (int i = 0; i < 2; ++i) {
        if (!bulletsPositions.empty()) {
            moveShells();
            std::cout << "Shells moved" << (i == 0 ? "first time" : " second time") << std::endl;
            handleCollisions();
            if (gameOver) {
                std::cout << "Game over" << std::endl;
                return;
            }
            std::cout << "Collisions handled for shells" << std::endl;
        }
    }
    updateAlgorithmsAfterShells();
    std::cout << "handling shells completed" << std::endl;

    performActions();
    std::cout << "Actions performed" << std::endl;
    handleCollisions();
    std::cout << "Collisions handled third time" << std::endl;
    checkNoAmoForTanks();
}

void GameBoard::updateAlgorithmsAfterShells() {
    for (Tank& tank : player1Tanks) {
        tank.getAlgorithm()->update();
    }
    for (Tank& tank : player2Tanks) {
        tank.getAlgorithm()->update();
    }
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
        if (tank.getIsTankAlive()) {  // Only include alive tanks
            const auto& location = tank.getInfo().location;
            positions.push_back({location[0], location[1]});
        }
    }
    return positions;
}

void GameBoard::saveGameMoves(const string& filename) const {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error opening output file" << endl;
        return;
    }

    // Find the maximum number of steps
    size_t maxSteps = std::max(player1Moves.size(), player2Moves.size());
    
    // Save moves step by step
    outputFile << "Game Moves (by step):" << endl;
    for (size_t i = 0; i < maxSteps; i++) {
        outputFile << "\nStep " << (i + 1) << ":" << endl;
        
        // Player 1's move for this step
        if (i < player1Moves.size()) {
            outputFile << player1Moves[i] << endl;
        } else {
            outputFile << "Player 1: No move" << endl;
        }
        
        // Player 2's move for this step
        if (i < player2Moves.size()) {
            outputFile << player2Moves[i] << endl;
        } else {
            outputFile << "Player 2: No move" << endl;
        }
    }

    // Save game result
    outputFile << "\nGame Result:" << endl;
    if (winner == 0) {
        bool anyTanksAlive = false;
        // Check if any tanks are still alive
        for (const auto& tank : player1Tanks) {
            if (tank.getIsTankAlive()) {
                anyTanksAlive = true;
                break;
            }
        }
        for (const auto& tank : player2Tanks) {
            if (tank.getIsTankAlive()) {
                anyTanksAlive = true;
                break;
            }
        }
        
        if (anyTanksAlive && numOfTurnsSinceNoAmmo >= NO_AMMO_TURNS) {
            outputFile << "Tie - All ammo exhausted" << endl;
        } else {
            outputFile << "Tie - All tanks lost" << endl;
        }
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
        if (location[0] == x && location[1] == y && tank.getIsTankAlive()) {
            p1TankCount++;
        }
    }
    
    // Check player 2 tanks
    for (const auto& tank : player2Tanks) {
        const auto& location = tank.getInfo().location;
        if (location[0] == x && location[1] == y && tank.getIsTankAlive()) {
            p2TankCount++;
        }
    }
    
    // Check if there are any shells at this position
    bool hasShell = isShellAtPosition(x, y);
    
    // If there's still more than one tank
    if ((p1TankCount + p2TankCount) > 1) {
        return hasShell ? MULTIPLE_TANKS_AND_SHELL : MULTIPLE_TANKS;
    }
    
    // If only one tank remains
    if (p1TankCount == 1) {
        return hasShell ? SHELL_AND_TANK : PLAYER1_TANK;
    } else if (p2TankCount == 1) {
        return hasShell ? SHELL_AND_TANK : PLAYER2_TANK;
    }
    
    // If no tanks remain but there's a shell
    if (hasShell) {
        return SHELL;
    }
    
    // If nothing remains
    return EMPTY_SPACE;
}

bool GameBoard::checkGameOver() {
    bool p1HasTanks = false;
    bool p2HasTanks = false;

    // Check if any player 1 tanks are alive
    for (const auto& tank : player1Tanks) {
        if (tank.getIsTankAlive()) {
            std::cout << "Player 1 has tanks" << std::endl;
            p1HasTanks = true;
            break;
        }
    }

    // Check if any player 2 tanks are alive
    for (const auto& tank : player2Tanks) {
        if (tank.getIsTankAlive()) {
            std::cout << "Player 2 has tanks" << std::endl;
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

    // Print ammo counts for each tank
    std::cout << "Ammo Counts:" << std::endl;
    for (const auto& tank : player1Tanks) {
        if (tank.getIsTankAlive()) {
            const auto& location = tank.getInfo().location;
            std::cout << "Player 1 tank at (" << location[0] << "," << location[1] << "): " 
                      << tank.getAmmoCount() << " ammo" << std::endl;
        }
    }
    for (const auto& tank : player2Tanks) {
        if (tank.getIsTankAlive()) {
            const auto& location = tank.getInfo().location;
            std::cout << "Player 2 tank at (" << location[0] << "," << location[1] << "): " 
                      << tank.getAmmoCount() << " ammo" << std::endl;
        }
    }
    std::cout << std::endl;
}

void GameBoard::checkNoAmoForTanks() {
    bool tankHasAmmo = false;
    for (auto& tank : player1Tanks) {
        if (tank.getAmmoCount() > 0) {
            tankHasAmmo = true;
            break;
        }
    }
    for (auto& tank : player2Tanks) {
        if (tank.getAmmoCount() > 0) {
            tankHasAmmo = true;
            break;
        }
    }
    if(!tankHasAmmo){
        numOfTurnsSinceNoAmmo++;
    }
    if(numOfTurnsSinceNoAmmo >= NO_AMMO_TURNS){
        gameOver = true;
        winner = 0; // 0 represents a tie
    }
}
