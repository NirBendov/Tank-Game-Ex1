#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "file_handlers/InputHandler.h"
#include "board/GameBoard.h"
#include "player/Player.h"
#include "algorithms/Algorithm.h"
#include "algorithms/AlgorithmPlayerOne.h"
#include "algorithms/AlgorithmPlayerTwo.h"

using namespace std;

int main() {
    // Create game board from input
    string inputFile = "game_board.txt"; // You should specify the actual input file
    vector<vector<char>> initialBoard = process(inputFile);
    GameBoard gameBoard(initialBoard);

    // Initialize players and their algorithms
    Player player1;
    Player player2;
    player1.id = Player::PlayerId::P1;
    player2.id = Player::PlayerId::P2;

    // Initialize algorithms with player IDs and game board
    AlgorithmPlayerOne algo1(1, &gameBoard);
    AlgorithmPlayerTwo algo2(2, &gameBoard);

    // Main game loop
    while (!gameBoard.isGameOver()) {
        // Player 1's turn
        vector<Action> player1Actions = algo1.decideNextActions();
        for (const auto& action : player1Actions) {
            gameBoard.addToStepMoves(action, 1);
        }

        // Player 2's turn
        vector<Action> player2Actions = algo2.decideNextActions();
        for (const auto& action : player2Actions) {
            gameBoard.addToStepMoves(action, 2);
        }
        
        // Execute all moves for this step
        gameBoard.executeStep();
    }
    
    // Save game moves to file
    gameBoard.saveGameMoves("game_moves.txt");

    return 0;
}