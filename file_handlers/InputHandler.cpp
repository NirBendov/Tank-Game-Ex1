#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include "../constants/BoardConstants.h"

using namespace std;
using namespace BoardConstants;

// Function to log errors to board_errors.txt
void logError(const string& errorMessage) {
    ofstream errorFile("board_errors.txt", ios::app);
    if (errorFile.is_open()) {
        errorFile << errorMessage << endl;
        errorFile.close();
    }
}

// Parse dimensions from the first line of the file
std::array<int, 2> dims(const std::string& s) {
    cout << "Parsing dimensions from: " << s << endl;
    size_t space = s.find(" ");
    if (space == std::string::npos) {
        logError("Error: Invalid dimensions format: " + s);
        throw std::runtime_error("Invalid dimensions format: " + s);
    }
    
    int height = std::stoi(s.substr(0, space));
    int width = std::stoi(s.substr(space + 1));
    
    if (height <= 0 || width <= 0) {
        logError("Error: Invalid board dimensions: " + s);
        throw std::runtime_error("Invalid board dimensions: " + s);
    }
    
    return {height, width};
}

// Process the board file and return a 2D vector of characters
std::vector<std::vector<char>> process(const std::string& fileName) {
    cout << "Opening file: " << fileName << endl;
    std::ifstream f(fileName);
    if (!f.is_open()) {
        logError("Error: Could not open file: " + fileName);
        throw std::runtime_error("Could not open file: " + fileName);
    }

    std::string s;
    if (!std::getline(f, s)) {
        logError("Error: File is empty: " + fileName);
        throw std::runtime_error("File is empty: " + fileName);
    }

    cout << "Parsing dimensions..." << endl;
    std::array<int, 2> d;
    try {
        d = dims(s);
    } catch (const std::runtime_error& e) {
        logError("Error: No valid dimensions provided in file");
        throw std::runtime_error("No valid dimensions provided in file");
    }
    
    BoardConstants::BOARD_HEIGHT = d[0];
    BoardConstants::BOARD_WIDTH = d[1];
    std::cout << "Board dimensions: " << BoardConstants::BOARD_HEIGHT << "x" << BoardConstants::BOARD_WIDTH << std::endl;
    
    std::vector<std::vector<char>> board;
    int player1_count = 0;
    int player2_count = 0;
    int line_number = 1;
    
    cout << "Reading board contents..." << endl;
    while (std::getline(f, s)) {
        line_number++;
        cout << "Reading line " << line_number << ": " << s << endl;
        
        if (board.size() >= static_cast<size_t>(BOARD_HEIGHT)) {
            cout << "Reached maximum board height, stopping." << endl;
            logError("Warning: File has more rows than specified height. Extra rows will be ignored.");
            break;
        }
        
        std::vector<char> row;
        for (size_t i = 0; i < static_cast<size_t>(BOARD_WIDTH); i++) {
            char c;
            if (i < s.length()) {
                c = s[i];
                // Validate characters
                if (c != WALL && c != DAMAGED_WALL && c != MINE && c != PLAYER1_TANK && c != PLAYER2_TANK && c != EMPTY_SPACE) {
                    logError("Warning: Invalid character '" + std::string(1, c) + "' at line " + std::to_string(line_number) + ", position " + std::to_string(i) + ". Replacing with empty space.");
                    c = EMPTY_SPACE;
                }
            } else {
                c = EMPTY_SPACE;
                logError("Warning: Line " + std::to_string(line_number) + " is shorter than specified width. Adding empty spaces.");
            }
            
            // Count tanks and handle multiple tanks per player
            if (c == PLAYER1_TANK) {
                if (player1_count == 0) {
                    player1_count++;
                } else {
                    logError("Warning: Multiple player 1 tanks found. Converting extra tank at line " + std::to_string(line_number) + ", position " + std::to_string(i) + " to empty space.");
                    c = EMPTY_SPACE;
                }
            }
            if (c == PLAYER2_TANK) {
                if (player2_count == 0) {
                    player2_count++;
                } else {
                    logError("Warning: Multiple player 2 tanks found. Converting extra tank at line " + std::to_string(line_number) + ", position " + std::to_string(i) + " to empty space.");
                    c = EMPTY_SPACE;
                }
            }
            
            row.push_back(c);
        }
        board.push_back(row);
    }
    
    // Add empty rows if needed
    while (board.size() < static_cast<size_t>(BOARD_HEIGHT)) {
        logError("Warning: File has fewer rows than specified height. Adding empty rows.");
        std::vector<char> emptyRow(BOARD_WIDTH, EMPTY_SPACE);
        board.push_back(emptyRow);
    }
    
    // Validate tank counts
    if (player1_count == 0) {
        logError("Error: No player 1 tank found in the board");
        throw std::runtime_error("No player 1 tank found in the board");
    }
    
    if (player2_count == 0) {
        logError("Error: No player 2 tank found in the board");
        throw std::runtime_error("No player 2 tank found in the board");
    }

    std::cout << "Finished constructing board" << std::endl;
    
    return board;
}