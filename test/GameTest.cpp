#include "GameTest.h"
#include "../board/GameBoard.h"
#include <iostream>

bool GameTest::runTest(GameBoard& board) const {
    std::cout << "Starting test: " << testName << std::endl;
    
    // Get the tanks from the board
    const auto& p1Tanks = board.getPlayerTanks(1);
    const auto& p2Tanks = board.getPlayerTanks(2);
    
    if (p1Tanks.empty() || p2Tanks.empty()) {
        std::cout << "Test " << testName << " failed: Missing tanks" << std::endl;
        return false;
    }

    // Execute the game until it's over or we run out of moves
    int step = 0;
    size_t p1MoveIndex = 0;
    size_t p2MoveIndex = 0;
    
    while ((!board.isGameOver() && step < 10) && !(p1MoveIndex >= player1MoveTypes.size() && p2MoveIndex >= player2MoveTypes.size())) {
        std::cout << "\nExecuting step " << ++step << std::endl;
        
        // Add one move from each player if available
        if (p1MoveIndex < player1MoveTypes.size()) {
            std::cout << "Adding move for player 1" << std::endl;
            board.addToStepMoves(Action(player1MoveTypes[p1MoveIndex++], p1Tanks[0]), 1);
        }
        
        if (p2MoveIndex < player2MoveTypes.size()) {
            std::cout << "Adding move for player 2" << std::endl;
            board.addToStepMoves(Action(player2MoveTypes[p2MoveIndex++], p2Tanks[0]), 2);
        }
        
        // Execute the step
        board.executeStep();
    }
    
    std::cout << "Game ended after " << step << " steps" << std::endl;

    // Check winner
    if (board.getWinner() != expectedWinner) {
        std::cout << "Test " << testName << " failed: Wrong winner. Expected " 
                  << expectedWinner << ", got " << board.getWinner() << std::endl;
        return false;
    }
    std::cout << "Winner check passed" << std::endl;

    // Check deaths
    if (!compareDeaths(board.tankDeaths, expectedDeaths)) {
        std::cout << "Test " << testName << " failed: Deaths don't match expected" << std::endl;
        return false;
    }
    std::cout << "Deaths check passed" << std::endl;

    std::cout << "Test " << testName << " passed successfully" << std::endl;
    return true;
}

bool GameTest::compareDeaths(const std::vector<GameBoard::TankDeath>& actual, 
                           const std::vector<GameBoard::TankDeath>& expected) const {
    if (actual.size() != expected.size()) {
        std::cout << "Death count mismatch. Expected: " << expected.size() 
                  << ", Actual: " << actual.size() << std::endl;
        return false;
    }

    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i].position != expected[i].position ||
            actual[i].playerId != expected[i].playerId ||
            actual[i].cause != expected[i].cause) {
            std::cout << "Death " << i << " mismatch:" << std::endl;
            std::cout << "  Expected: Player " << expected[i].playerId 
                      << " at (" << expected[i].position.first << "," 
                      << expected[i].position.second << ") from " 
                      << expected[i].cause << std::endl;
            std::cout << "  Actual: Player " << actual[i].playerId 
                      << " at (" << actual[i].position.first << "," 
                      << actual[i].position.second << ") from " 
                      << actual[i].cause << std::endl;
            return false;
        }
    }

    return true;
}

bool GameTest::execute() const {
    std::cout << "\n=== Starting test: " << testName << " ===" << std::endl;
    
    // Create game board
    GameBoard gameBoard(initialBoard);
    std::cout << "Game board created with size: " 
              << gameBoard.getPlayerTankPositions(1).size() << " P1 tanks, "
              << gameBoard.getPlayerTankPositions(2).size() << " P2 tanks" << std::endl;

    return runTest(gameBoard);
} 