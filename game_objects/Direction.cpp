#include "Direction.h"
#include "../constants/BoardConstants.h"
#include "../board/GameBoard.h"
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
    int dy = start[0] - end[0];
    int dx = start[1] - end[1];
    dy = dy == 0 ? dy : dy/abs(dy);
    dx = dx == 0 ? dx : dx/abs(dx);
    if (!(trajectory[0] == dy && trajectory[1] == dx))
        return false;
    int m = trajectory[0] != 0 ? min(start[0], end[0]) : min(start[1], end[1]);
    int n = trajectory[0] != 0 ? max(start[0], end[0]) : max(start[1], end[1]);
    int x = trajectory[0] != 0 ? start[0] : end[0];
    int y = trajectory[0] != 0 ? start[1] : end[1];
    for (int i = m; i <= n; ++i) {
        // TODO: Implement bullet path checking
    }
    return true;
}