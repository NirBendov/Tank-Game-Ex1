#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#include "file_handlers/InputHandler.h"
#include "board/GameBoard.h"
#include "algorithms/AlgorithmBasic.h"
#include "algorithms/AlgorithmPlayerTwo.h"
#include "algorithms/Action.h"
#include "player/Player.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <max_steps>\n";
        return 1;
    }

    // 1. Read the input filename from argv
    string inputFile = argv[1];
    int maxSteps = stoi(argv[2]);

    // 2. Parse out just the base name (no directory, no extension)
    fs::path inPath{inputFile};
    string baseName = inPath.stem().string();  // e.g. "input" from "input.txt"

    // 3. Construct output filename
    string outputFile = "output_" + baseName + ".txt";

    // 4. Process input and run game
    cout << "Processing input file..." << endl;
    vector<vector<char>> initialBoard = process(inputFile);
    cout << "Creating game board..." << endl;
    GameBoard gameBoard(initialBoard, maxSteps);
    cout << "Game board created" << endl;

    cout << "Initializing algorithms..." << endl;
    // Example: both players using AlgorithmPlayerTwo
    AlgorithmPlayerTwo algo1(1, &gameBoard);
    AlgorithmPlayerTwo algo2(2, &gameBoard);
    cout << "Algorithms initialized" << endl;

    int step = 0;
    while (!gameBoard.isGameOver()) {
        cout << "Step " << ++step << endl;
        // Player 1's turn
        cout << "Player 1's turn..." << endl;
        for (const auto& action : algo1.decideNextActions())
            gameBoard.addToStepMoves(action, 1);

        // Player 2's turn
        cout << "Player 2's turn..." << endl;
        for (const auto& action : algo2.decideNextActions())
            gameBoard.addToStepMoves(action, 2);

        cout << "Executing step..." << endl;
        gameBoard.executeStep();
        cout << "Step executed" << endl;
    }

    // 5. Save moves to the dynamically constructed file
    cout << "Saving game moves..." << endl;
    gameBoard.saveGameMoves(outputFile);
    cout << "Saved game moves to: " << outputFile << "\n";

    return 0;
}
