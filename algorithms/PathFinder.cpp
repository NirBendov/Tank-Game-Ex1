#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include "../constants/BoardConstants.h"
#include "../game_objects/Direction.h"
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
    if (grid[x][y] == BoardConstants::PLAYER1_TANK || grid[x][y] == BoardConstants::PLAYER2_TANK) {
        return true;
    }
    return false;
}

Point wrapPoint(int x, int y, int rows, int cols) {
    x = (x + rows) % rows;
    y = (y + cols) % cols;
    return {x, y};
}

vector<Point> bfsPathfinder(const vector<vector<char>>& grid, Point start, Point end, bool includeWalls) {
    int rows = grid.size();
    int cols = grid[0].size();

    cout << "Starting BFS from (" << start.x << "," << start.y << ") to (" << end.x << "," << end.y << ")" << endl;
    cout << "Grid size: " << rows << "x" << cols << endl;
    cout << "Include walls: " << (includeWalls ? "true" : "false") << endl;

    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    vector<vector<Point>> parent(rows, vector<Point>(cols, {-1, -1}));

    queue<Node> q;
    visited[start.x][start.y] = true;
    q.push({start, 0});

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        Point pt = current.pt;
        cout << "Exploring point (" << pt.x << "," << pt.y << ")" << endl;

        if (pt.x == end.x && pt.y == end.y) {
            cout << "Found end point!" << endl;
            // Reconstruct path
            vector<Point> path;
            while (!(pt.x == -1 && pt.y == -1)) {
                path.push_back(pt);
                pt = parent[pt.x][pt.y];
            }
            reverse(path.begin(), path.end());
            cout << "Path length: " << path.size() << endl;
            return path;
        }

        for (const auto& dir : directions) {
            Point neighbor = wrapPoint(pt.x + dir[1], pt.y + dir[0], rows, cols);
            cout << "Checking neighbor (" << neighbor.x << "," << neighbor.y << "): ";
            if (isValid(neighbor.x, neighbor.y, grid, visited, includeWalls)) {
                cout << "valid" << endl;
                visited[neighbor.x][neighbor.y] = true;
                parent[neighbor.x][neighbor.y] = pt;
                q.push({neighbor, current.dist + 1});
            } else {
                // cout << "invalid" << endl;
            }
        }
    }

    cout << "No path found without walls" << endl;
    if (includeWalls) {
        return {};
    }

    return bfsPathfinder(grid, start, end, true);
}

int dist(Point p1, Point p2, int rows, int cols) {
    int dx = min(abs(p1.x - p2.x), rows - abs(p1.x - p2.x));
    int dy = min(abs(p1.y - p2.y), cols - abs(p1.y - p2.y));
    return max(dx ,dy);
}

void updatePathEnd(vector<Point> &path, Point &newEnd, int rows, int cols) {
    Point end = path.back();
    path.pop_back();
    if (newEnd == end) {
        path.push_back(newEnd);
    }
    else if (path.size() > 1){
        Point nearEnd = path.back();
        path.pop_back();
        if (!(newEnd == nearEnd)) {
            if (dist(end, newEnd, rows, cols) >= dist(nearEnd, newEnd, rows, cols)) {
                if (!path.empty()) {
                    Point nearNearEnd = path.back();
                    if (dist(newEnd, nearNearEnd, rows, cols) > 1) {
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
    } else {
        if(dist(newEnd, path.front(), rows, cols) > 1)
            path.push_back(end);
        path.push_back(newEnd);
    }
}

void updatePathStart(vector<Point> &path, Point &newStart, int rows, int cols) {
    Point start = path.front();
    path.erase(path.begin());
    if (newStart == start) {
        path.insert(path.begin(), start);
    }
    else if (path.size() > 1) {
        Point nearStart = path.front();
        path.erase(path.begin());
        if (!(newStart == nearStart)) {
            if (dist(start, newStart, rows, cols) >= dist(nearStart, newStart, rows, cols)) {
                if (!path.empty()) {
                    Point nearNearStart = path.front();
                    if (dist(newStart, nearNearStart, rows, cols) > 1) {
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
    } else {
        if(dist(newStart, path.back(), rows, cols) > 1)
            path.insert(path.begin(), start);
        path.insert(path.begin(), newStart);
    }
}

array<int,2> calcDirection(vector<Point> &path, int rows, int columns) {
    Point *start = &path[0];
    Point *next = &path[1];
    int dx, dy;
    if (start->x == next->x)
        dy = 0;
    else if (start->x == 0 and next->x == columns - 1) 
        dy = -1;
    else if (start->x == columns - 1 and next->x == 0)
        dy = 1;
    else
        dy = next->x - start->x;

    if (start->y == next->y)
        dx = 0;
    else if (start->y == 0 and next->y == rows - 1) 
        dx = -1;
    else if (start->y == rows - 1 and next->y == 0)
        dx = 1;
    else
        dx = next->y - start->y;
    
    return {dy ,dx};
}

bool isPathStraight(vector<Point> &path, int rows, int columns) {
    if (path.size() == 1) {
        return true;
    }
    array<int,2> dir = calcDirection(path, rows, columns);

    for (size_t i = 1; i < path.size() - 1; ++i) {
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

void printPath(const vector<Point>& path) {
    for (const auto& p : path) {
        cout << "(" << p.x << "," << p.y << ") ";
    }
    cout << endl;
}