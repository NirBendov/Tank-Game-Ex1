#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include "./constants/BoardConstants.h"
#include "./game_objects/Direction.h"
#include "PathFinder.h"

using namespace std;

bool isValid(int x, int y, const vector<vector<char>>& grid, const vector<vector<bool>>& visited, bool includeWalls) {
    if (visited[x][y]) {
        return false;
    }
    if (grid[x][y] == BoardConstants::EMPTY_SPACE) {
        return true;
    }
    if (includeWalls && 
        (grid[x][y] == BoardConstants::WALL || grid[x][y] == BoardConstants::DAMAGED_WALL)) {
            return true;
        } 
    return false;
}

Point wrapPoint(int x, int y, int rows, int cols) {
    x = (x + rows) % rows;
    y = (y + cols) % cols;
    return {x, y};
}

vector<Point> directPathFinder(Point start, Point end, int rows, int columns) {
    int yDir = start.y == end.y ? 0 : 1;
    int dy = abs(start.y - end.y);
    yDir = start.y > end.y ? -1 * yDir : yDir;
    int verticalDist = min(dy, rows - dy);
    yDir = verticalDist == rows - dy ? -1 * yDir : yDir;

    int xDir = start.x == end.x ? 0 : 1;
    int dx = abs(start.x - end.x);
    xDir = start.x > end.x ? -1 * xDir : xDir;
    int horizontalDist = min(dx, columns- dx);
    xDir = horizontalDist == columns - dx ? -1 * xDir : xDir;

    int dist = min(horizontalDist, verticalDist);
    int maxDist = max(horizontalDist, verticalDist);

    vector<Point> path;
    for (int i = 0; i <= dist; ++i) {
        Point p = wrapPoint(start.x + xDir*i, start.y + yDir*i, rows, columns);
        path.push_back(p);
    }
    for (int i = dist+1; i <= maxDist; ++i) {
        Point p;
        if (horizontalDist > verticalDist)
            p = wrapPoint(start.x + xDir*i, end.y, rows, columns);
        else 
            p = wrapPoint(end.x, start.y + yDir*i, rows, columns);
        path.push_back(p);
    }

    return path;
}

vector<Point> bfsPathfinder(const vector<vector<char>>& grid, Point start, Point end, bool includeWalls) {
    int rows = grid.size();
    int cols = grid[0].size();

    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    vector<vector<Point>> parent(rows, vector<Point>(cols, {-1, -1}));

    queue<Node> q;
    visited[start.x][start.y] = true;
    q.push({start, 0});

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        Point pt = current.pt;

        if (pt == end) {
            // Reconstruct path
            vector<Point> path;
            while (!(pt.x == -1 && pt.y == -1)) {
                path.push_back(pt);
                pt = parent[pt.x][pt.y];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& dir : directions) {
            Point neighbor = wrapPoint(pt.x + dir[1], pt.y + dir[0], rows, cols);
            if (isValid(neighbor.x, neighbor.y, grid, visited, includeWalls)) {
                visited[neighbor.x][neighbor.y] = true;
                parent[neighbor.x][neighbor.y] = pt;
                q.push({neighbor, current.dist + 1});
            }
        }
    }

    if (includeWalls) {
        return {};
    }

    return bfsPathfinder(grid, start, end, true);
}

int dist(Point p1, Point p2) {
    return max(abs(p1.x - p2.x), abs(p1.y - p2.y));
}

vector<Point> updatePathEnd(vector<Point> &path, Point &newEnd) {
    Point end = path.back();
    path.pop_back();
    if (newEnd == end) {
        path.push_back(newEnd);
    }
    else {
        Point nearEnd = path.back();
        path.pop_back();
        if (!(newEnd == nearEnd)) {
            if (dist(end, newEnd) >= dist(nearEnd, newEnd)) {
                if (!path.empty()) {
                    Point nearNearEnd = path.back();
                    if (dist(newEnd, nearNearEnd) > 1) {
                        path.push_back(nearEnd);
                    }
                }
            }
            else {
                path.push_back(nearEnd);
                path.push_back(end);
            }
        }
        path.push_back(newEnd);
    }
}

vector<Point> updatePathStart(vector<Point> &path, Point &newStart) {
    Point start = path.front();
    path.erase(path.begin());
    if (newStart == start) {
        path.insert(path.begin(), start);
    }
    else {
        Point nearStart = path.front();
        if (!(newStart == nearStart)) {
            if (dist(start, newStart) >= dist(nearStart, newStart)) {
                if (!path.empty()) {
                    Point nearNearStart = path.front();
                    if (dist(newStart, nearNearStart) > 1) {
                        path.insert(path.begin(), nearStart);
                    }
                }
            }
            else {
                path.insert(path.begin(), nearStart);
                path.insert(path.begin(), start);
            }
        }
        path.insert(path.begin(), newStart);
    }
}

array<int,2> calcDirection(vector<Point> &path, int rows, int columns) {
    Point *start = &path[0];
    Point *next = &path[1];
    int dx, dy;
    if (start->x == next->x)
        dx = 0;
    else if (start->x == 0 and next->x == columns - 1) 
        dx = -1;
    else if (start->x == columns - 1 and next->x == 0)
        dx = 1;
    else
        dx = next->x - start->x;

    if (start->y == next->y)
        dy = 0;
    else if (start->y == 0 and next->y == rows - 1) 
        dy = -1;
    else if (start->y == rows - 1 and next->y == 0)
        dy = 1;
    else
        dy = next->y - start->y;
    
    return {dy ,dx};
}

bool isPathStraight(vector<Point> &path, int rows, int columns) {
    if (path.size() == 1) {
        return true;
    }
    array<int,2> dir = calcDirection(path, rows, columns);

    for (int i = 1; i < path.size() - 1; ++i) {
        Point *start = &path[i];
        Point *next = &path[i+1];
        if (!((next->x - start->x)%columns == dir[0]%columns)) {
            return false;
        }
        if (!((next->y - start->y)%rows == dir[1]%rows)) {
            return false;
        }
    }
    return true;
}

bool isPathClear(vector<Point> &path, const vector<vector<char>>& grid) {
    for (Point &p: path) {
        char tile = grid[p.y][p.x];
        if (tile == BoardConstants::WALL ||
                tile == BoardConstants::DAMAGED_WALL)
                return false;
    }
    return true;
}

// int main() {
//     vector<vector<int>> grid = {
//         {0, 1, 0, 0, 0},
//         {0, 1, 0, 1, 0},
//         {0, 0, 0, 1, 0},
//         {1, 1, 0, 0, 0},
//         {0, 0, 0, 1, 0}
//     };

//     Point start = {0, 0};
//     Point end = {4, 4};

//     vector<Point> path = bfsPathfinder(grid, start, end, false);

//     if (!path.empty()) {
//         cout << "Path found:\n";
//         for (const auto& p : path) {
//             cout << "(" << p.x << ", " << p.y << ") ";
//         }
//         cout << endl;
//     } else {
//         cout << "No path found.\n";
//     }

//     return 0;
// }