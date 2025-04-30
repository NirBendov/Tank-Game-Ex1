#include "Direction.h"
#include "../constants/BoardConstants.h"
#include "../board/GameBoard.h"
#include "../algorithms/Pathfinder.h"
#include <cmath>
#include <array>
#include <iostream>

using namespace std;

int vectorMultiply(array<int, 2> d1, array<int, 2> d2) {
    return d1[0] * d2[0] + d1[1] * d2[1];
}

int inverseMap(array<int, 2> d) {
    for(int i = 0; i < 8; ++i) {
        if (directions[i][0] == d[0] && directions[i][1] == d[1]) {
            return i;
        }
    }
    return -1; // Return -1 if direction not found
}

array<int, 2> getDirection(array<int, 2> d, Turn t) {
    int index = inverseMap(d);
    if (index == -1) {
        // If direction not found, return original direction
        return d;
    }
    
    // Calculate new index with wrap-around
    int newIndex = (index + t + 8) % 8;
    
    // Create a new array to hold the result
    static array<int, 2> result;
    result[0] = directions[newIndex][0];
    result[1] = directions[newIndex][1];
    
    return result;
}

bool isInBulletPath(array<int, 2> start, array<int, 2> trajectory, array<int, 2> end, GameBoard *gameBoard) {
    // Check if end point is in the direction of trajectory
    int dx = end[1] - start[1];
    int dy = end[0] - start[0];
    
    // Handle wrap-around
    if (abs(dx) > gameBoard->getBoard().size() / 2) {
        dx = dx > 0 ? dx - gameBoard->getBoard().size() : dx + gameBoard->getBoard().size();
    }
    if (abs(dy) > gameBoard->getBoard()[0].size() / 2) {
        dy = dy > 0 ? dy - gameBoard->getBoard()[0].size() : dy + gameBoard->getBoard()[0].size();
    }
    
    // Check if end point is in the direction of trajectory
    if ((dx != 0 && trajectory[1] == 0) || (dy != 0 && trajectory[0] == 0)) {
        return false;
    }
    
    // Check if the direction matches
    if (dx != 0 && dy != 0) {
        if (abs(dx) != abs(dy)) {
            return false;
        }
        if ((dx > 0 && trajectory[1] < 0) || (dx < 0 && trajectory[1] > 0)) {
            return false;
        }
        if ((dy > 0 && trajectory[0] < 0) || (dy < 0 && trajectory[0] > 0)) {
            return false;
        }
    } else if (dx != 0) {
        if ((dx > 0 && trajectory[1] < 0) || (dx < 0 && trajectory[1] > 0)) {
            return false;
        }
    } else if (dy != 0) {
        if ((dy > 0 && trajectory[0] < 0) || (dy < 0 && trajectory[0] > 0)) {
            return false;
        }
    }
    
    // Check if path is clear
    vector<vector<char>> board = gameBoard->getBoard();
    int rows = board.size();
    int columns = board[0].size();
    
    int steps = max(abs(dx), abs(dy));
    for (int i = 1; i < steps; ++i) {
        int y = (start[0] + i * trajectory[0] + rows) % rows;
        int x = (start[1] + i * trajectory[1] + columns) % columns;
        
        if (board[y][x] != BoardConstants::EMPTY_SPACE && 
            board[y][x] != BoardConstants::MINE && 
            board[y][x] != BoardConstants::SHELL) {
            return false;
        }
    }
    
    return true;
}

bool isInBulletPathSimple(array<int, 2> start, array<int, 2> trajectory, array<int, 2> end) {
    array<int, 2> current = start;

    // Keep moving along the trajectory until we either find the end point or wrap around
    for (int i = 0; i < max(BoardConstants::BOARD_WIDTH, BoardConstants::BOARD_HEIGHT) + 1; i++) {
        // Move one step in the trajectory direction
        current[0] = (current[0] + trajectory[0] + BoardConstants::BOARD_WIDTH) % BoardConstants::BOARD_WIDTH;
        current[1] = (current[1] + trajectory[1] + BoardConstants::BOARD_HEIGHT) % BoardConstants::BOARD_HEIGHT;
        // If we've found the end point, return true
        if (current[0] == end[0] && current[1] == end[1]) {
            return true;
        }
        
        // If we've wrapped around to the start point, return false
        if (current[0] == start[0] && current[1] == start[1]) {
            return false;
        }
    }
    return false;
}

array<int,2> directionBetweenPoints(Point &start, Point &end) {
    int dy = end.x - start.x; //this is not a mistake
    int dx = end.y - start.y;
    if (dy > 1) 
        dy = -1;
    else if (dy < -1)
        dy = 1;
    if (dx > 1) 
        dx = -1;
    else if (dx < -1)
        dx = 1; 
    return {dy, dx};
}

Turn rotation(array<int,2> currDir, array<int,2> newDir) {
    int rotation = (inverseMap(newDir) - inverseMap(currDir))%8;
    if (rotation <= -4)
        rotation += 8;
    else if (rotation > 4)
        rotation -= 8;
    return Turn(rotation);
}

Action::Type turnToAction(Turn t) {
    switch (t)
    {
    case Turn::RIGHT_90 :
    case Turn::RIGHT_135 :
    case Turn::COMPLETE_180 :
        return Action::Type::TURN_R_90;
    case Turn::RIGHT_45 :
        return Action::Type::TURN_R_45;
    case Turn::LEFT_90:
    case Turn::LEFT_135:
        return Action::Type::TURN_L_90;
    case Turn::LEFT_45:
        return Action::Type::TURN_L_45;
    default:
        return Action::Type::NOP;
        break;
    }
}