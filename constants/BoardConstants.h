#pragma once

namespace BoardConstants {
    // Basic board elements
    const char WALL = '#';
    const char DAMAGED_WALL = '-';
    const char MINE = '@';
    const char EMPTY_SPACE = ' ';
    const char PLAYER1_TANK = '1';
    const char PLAYER2_TANK = '2';
    const char SHELL = '*';

    // Collision combinations
    // Wall/Damaged Wall + Shell combinations
    const char WALL_AND_SHELL = 'W';      // Wall + Shell
    const char DAMAGED_WALL_AND_SHELL = 'D';  // Damaged Wall + Shell

    // Mine combinations
    const char MINE_AND_SHELL = '&';      // Mine + Shell
    const char MINE_AND_TANK = 'M';       // Mine + Any Tank(s)
    const char MINE_SHELL_AND_TANK = 'S'; // Mine + Shell + Any Tank(s)

    // Tank combinations
    const char MULTIPLE_TANKS = 'T';      // Multiple Tanks (from any player)

    // Wall/Damaged Wall + Shell + Tank combinations
    const char WALL_SHELL_AND_TANK = 'U';  // Wall + Shell + Any Tank(s)
    const char DAMAGED_WALL_SHELL_AND_TANK = 'Y'; // Damaged Wall + Shell + Any Tank(s)

    // Game rules
    const int NUMBER_OF_TANKS_PER_PLAYER = 1;
    const int SHOOT_COOLDOWN = 5;
    const int MOVE_BACKWARD_COOLDOWN = 3;

    // Global board dimensions
    extern int BOARD_HEIGHT;
    extern int BOARD_WIDTH;

    // Helper function to check if a character represents a collision
    bool isCollision(char c) {
        return c != WALL && c != DAMAGED_WALL && c != MINE && 
               c != EMPTY_SPACE && c != PLAYER1_TANK && c != PLAYER2_TANK && 
               c != SHELL;
    }
} 