#include "Direction.h"
#include "../constants/BoardConstants.h"
#include "../board/GameBoard.h"
#include "./algorithms/Pathfinder.h"
#include <cmath>
#include <array>

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
    int dy = end[0] - start[0];
    int dx = end[1] - start[1];
    if ((dx != 0 && trajectory[1] != 0 && dy/dx != trajectory[0]/trajectory[1]) 
    || (dy != 0 && trajectory[0] && dy/dx != trajectory[1]/trajectory[0])) 
        return false;
    dy = dy == 0 ? dy : dy/abs(dy);
    dx = dx == 0 ? dx : dx/abs(dx);
    if (!(trajectory[0] == dy && trajectory[1] == dx))
        return false;

    vector<vector<char>> board = gameBoard->getBoard();
    int rows = board.size();
    int columns = board[0].size();

    int m = trajectory[0] != 0 ? start[0] : start[1];
    int n = trajectory[0] != 0 ? end[0] : end[1];
    for (int i = 0; i <= abs(m-n); ++i) {
        int y = (start[0] + i * trajectory[0])%rows; 
        int x = (start[1] + i * trajectory[1])%columns;
        if (board[y][x] != BoardConstants::EMPTY_SPACE 
        && board[y][x] != BoardConstants::MINE
        && board[y][x] != BoardConstants::SHELL) {
            return false;
        }
    }
    return true;
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
    int rotation = inverseMap(newDir) - inverseMap(currDir);
    return rotation > 4 ? Turn(rotation - 8) : Turn(rotation);
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