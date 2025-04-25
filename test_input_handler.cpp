#include <iostream>
#include <vector>
#include "file_handlers/InputHandler.cpp"

using namespace std;

int main() {
    try {
        vector<vector<char>> board = process("C:/repos/Tank-Game-Ex1/test_board.txt");
        
        // Print the board to verify
        for (const auto& row : board) {
            for (char c : row) {
                cout << c;
            }
            cout << endl;
        }
        
        cout << "Board successfully loaded!" << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
} 