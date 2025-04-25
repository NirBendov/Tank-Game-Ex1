#include <vector>
#include <fstream>
#include <iostream>
#include "GameBoard.h"
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
    : board(initialBoard), gameOver(false) {
    height = board.size();
    width = board[0].size();
    boardData = new BoardData();

    // Initialize tanks based on the board
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (board[i][j] == PLAYER1_TANK) {
                int location[2] = {i, j};
                int direction[2] = {0, 1}; // Default direction: right
                Tank tank(location, direction);
                tank.assignPlayerId(Player::PlayerId::P1);
                player1Tanks.push_back(tank);
                boardData->player1Tanks.push_back(tank);
            } else if (board[i][j] == PLAYER2_TANK) {
                int location[2] = {i, j};
                int direction[2] = {0, -1}; // Default direction: left
                Tank tank(location, direction);
                tank.assignPlayerId(Player::PlayerId::P2);
                player2Tanks.push_back(tank);
                boardData->player2Tanks.push_back(tank);
            }
        }
    }
}

GameBoard::~GameBoard() {
    delete boardData;
}

bool GameBoard::validateMove(const Action& action, int playerId) const {
    // Get the tank that's trying to move
    Tank targetTank = action.getTargetTank();
    const auto& tankLocation = targetTank.getInfo().location;
    
    // Check if the tank belongs to the player
    const auto& playerTanks = (playerId == 1) ? boardData->player1Tanks : boardData->player2Tanks;
    bool isPlayerTank = false;
    
    for (const auto& tank : playerTanks) {
        if (tank.getInfo().location == tankLocation) {
            isPlayerTank = true;
            break;
        }
    }
    
    if (!isPlayerTank) return false;

    // Check for shoot cooldown
    if (action.getActionType() == Action::ActionType::SHOOT) {
        auto it = tankShootCooldowns.find({tankLocation[0], tankLocation[1]});
        if (it != tankShootCooldowns.end()) {
            return false; // Tank is on cooldown
        }
    }

    // Check for wall collision in movement actions
    if (action.getActionType() == Action::ActionType::MOVE_FORWARD || 
        action.getActionType() == Action::ActionType::MOVE_BACKWARD) {
        const auto& dir = targetTank.getInfo().dir;
        int newX = tankLocation[0];
        int newY = tankLocation[1];
        
        if (action.getActionType() == Action::ActionType::MOVE_FORWARD) {
            newX += dir[0];
            newY += dir[1];
        } else {
            newX -= dir[0];
            newY -= dir[1];
        }
        
        if (isWall(newX, newY)) {
            return false;
        }
    }
    
    return true;
}

void GameBoard::addToStepMoves(const Action& action, int playerId) {
    if (!validateMove(action, playerId)) {
        return;
    }
    
    const auto& tankLocation = action.getTargetTank().getInfo().location;
    std::string moveStr = "Player " + std::to_string(playerId) + ": " + 
                         std::to_string(static_cast<int>(action.getActionType())) +
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

void GameBoard::executeStep() {
    // Process all moves in stepMoves
    for (const auto& action : stepMoves) {
        const auto& targetTank = action.getTargetTank();
        const auto& tankInfo = targetTank.getInfo();
        const auto& location = tankInfo.location;

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
        if (!actualTank) continue; // Skip if tank not found

        switch (action.getActionType()) {
            case Action::ActionType::SHOOT: {
                // Create a new shell
                Shell* shell = actualTank->shoot();
                if (shell) {
                    // Add shell to boardData
                    boardData->bulletsPositions.push_back(*shell);
                    // Set cooldown for the tank
                    tankShootCooldowns[{location[0], location[1]}] = BoardConstants::SHOOT_COOLDOWN;
                    delete shell;
                }
                break;
            }
            case Action::ActionType::MOVE_FORWARD: {
                // Calculate new position
                actualTank->move();
                const auto& newLocation = actualTank->getInfo().location;
                int newX = newLocation[0];
                int newY = newLocation[1];
                
                board[location[0]][location[1]] = BoardConstants::EMPTY_SPACE;
                board[newX][newY] = (targetTank.getPlayerId() == Player::PlayerId::P1) ? 
                                   BoardConstants::PLAYER1_TANK : BoardConstants::PLAYER2_TANK;
                break;
            }
            case Action::ActionType::MOVE_BACKWARD: {
                // Calculate new position
                int newX = location[0];
                int newY = location[1];
                
                // Handle wrap-around
                if (newX < 0) newX = height - 1;
                else if (newX >= height) newX = 0;
                if (newY < 0) newY = width - 1;
                else if (newY >= width) newY = 0;
                
                board[location[0]][location[1]] = BoardConstants::EMPTY_SPACE;
                board[newX][newY] = (targetTank.getPlayerId() == Player::PlayerId::P1) ? 
                                   BoardConstants::PLAYER1_TANK : BoardConstants::PLAYER2_TANK;
                break;
            }
            case Action::ActionType::TURN_R_45: {
                actualTank->turn(Turn::RIGHT_45);
                break;
            }
            case Action::ActionType::TURN_R_90: {
                actualTank->turn(Turn::RIGHT_90);
                break;
            }
            case Action::ActionType::TURN_L_45: {
                actualTank->turn(Turn::LEFT_45);
                break;
            }
            case Action::ActionType::TURN_L_90: {
                actualTank->turn(Turn::LEFT_90);
                break;
            }
            case Action::ActionType::NOP:
                break;
        }
    }
    
    // Clear the step moves after processing
    stepMoves.clear();
    
    // Update cooldowns
    updateCooldowns();
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

    outputFile << "Player 1 moves:" << endl;
    for (const auto& move : player1Moves) {
        outputFile << move << endl;
    }

    outputFile << "\nPlayer 2 moves:" << endl;
    for (const auto& move : player2Moves) {
        outputFile << move << endl;
    }

    outputFile.close();
}
