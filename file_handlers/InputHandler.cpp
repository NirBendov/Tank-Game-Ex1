#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include "../constants/BoardConstants.h"

using namespace std;
using namespace BoardConstants;

// Parse dimensions from the first line of the file
std::array<int, 2> dims(const std::string& s) {
    cout << "Parsing dimensions from: " << s << endl;
    size_t space = s.find(" ");
    if (space == std::string::npos) {
        throw std::runtime_error("Invalid dimensions format: " + s);
    }
    
    int height = std::stoi(s.substr(0, space));
    int width = std::stoi(s.substr(space + 1));
    
    if (height <= 0 || width <= 0) {
        throw std::runtime_error("Invalid board dimensions: " + s);
    }
    
    return {height, width};
}

// Process the board file and return a 2D vector of characters
std::vector<std::vector<char>> process(const std::string& fileName) {
    cout << "Opening file: " << fileName << endl;
    std::ifstream f(fileName);
    if (!f.is_open()) {
        throw std::runtime_error("Could not open file: " + fileName);
    }

    std::string s;
    if (!std::getline(f, s)) {
        throw std::runtime_error("File is empty: " + fileName);
    }

    cout << "Parsing dimensions..." << endl;
    std::array<int, 2> d = dims(s);
    const int BOARD_HEIGHT = d[0];
    const int BOARD_WIDTH = d[1];
    std::cout << "Board dimensions: " << BOARD_HEIGHT << "x" << BOARD_WIDTH << std::endl;
    
    std::vector<std::vector<char>> board;
    int player1_count = 0;
    int player2_count = 0;
    int line_number = 1; // Track line number for better error messages
    
    cout << "Reading board contents..." << endl;
    while (std::getline(f, s)) {
        line_number++;
        cout << "Reading line " << line_number << ": " << s << endl;
        
        if (board.size() >= static_cast<size_t>(BOARD_HEIGHT)) {
            cout << "Reached maximum board height, stopping." << endl;
            break; // Stop reading if we already have the required number of lines
        }
        
        if (s.length() < static_cast<size_t>(BOARD_WIDTH)) {
            throw std::runtime_error("Invalid line length at line " + std::to_string(line_number) + ": expected " + std::to_string(BOARD_WIDTH) + ", got " + std::to_string(s.length()));
        }
        
        std::vector<char> row;
        for (size_t i = 0; i < std::min(s.length(), static_cast<size_t>(BOARD_WIDTH)); i++) {
            char c = s[i];
            cout << "Processing character at position " << i << ": '" << c << "'" << endl;
            
            // Validate characters
            if (c != WALL && c != DAMAGED_WALL && c != MINE && c != PLAYER1_TANK && c != PLAYER2_TANK && c != EMPTY_SPACE) {
                throw std::runtime_error("Invalid character '" + std::string(1, c) + "' at line " + std::to_string(line_number) + ", position " + std::to_string(i));
            }
            
            // Count tanks and enforce the limit
            if (c == PLAYER1_TANK) {
                player1_count++;
                cout << "Found player 1 tank (count: " << player1_count << ")" << endl;
                if (player1_count > NUMBER_OF_TANKS_PER_PLAYER) {
                    cout << "Too many player 1 tanks, converting to empty space" << endl;
                    c = EMPTY_SPACE; // Replace extra tanks with empty space
                }
            }
            if (c == PLAYER2_TANK) {
                player2_count++;
                cout << "Found player 2 tank (count: " << player2_count << ")" << endl;
                if (player2_count > NUMBER_OF_TANKS_PER_PLAYER) {
                    cout << "Too many player 2 tanks, converting to empty space" << endl;
                    c = EMPTY_SPACE; // Replace extra tanks with empty space
                }
            }
            
            row.push_back(c);
        }
        board.push_back(row);
        cout << "Added row to board (current height: " << board.size() << ")" << endl;
    }
    
    // Validate board dimensions
    if (board.size() != static_cast<size_t>(BOARD_HEIGHT)) {
        throw std::runtime_error("Invalid board height: expected " + std::to_string(BOARD_HEIGHT) + ", got " + std::to_string(board.size()));
    }
    
    // Validate tank counts
    if (player1_count != 1) {
        std::cout << "Warning: Invalid number of player 1 tanks: " << player1_count << std::endl;
    }
    
    if (player2_count != 1) {
        std::cout << "Warning: Invalid number of player 2 tanks: " << player2_count << std::endl;
    }

    std::cout << "Finished constructing board" << std::endl;
    
    return board;
}