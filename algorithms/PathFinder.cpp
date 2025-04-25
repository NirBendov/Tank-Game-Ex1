#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

using namespace std;

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct Node {
    Point pt;
    int dist;
};

bool isValid(int x, int y, const vector<vector<int>>& grid, const vector<vector<bool>>& visited) {
    return (grid[x][y] == 0 && !visited[x][y]);
}

Point wrapPoint(int x, int y, int rows, int cols) {
    x = (x + rows) % rows;
    y = (y + cols) % cols;
    return {x, y};
}

vector<Point> bfsPathfinder(const vector<vector<int>>& grid, Point start, Point end) {
    int rows = grid.size();
    int cols = grid[0].size();

    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    vector<vector<Point>> parent(rows, vector<Point>(cols, {-1, -1}));

    // 8 directions: up, down, left, right, and the 4 diagonals
    vector<Point> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

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
            Point neighbor = wrapPoint(pt.x + dir.x, pt.y + dir.y, rows, cols);
            if (isValid(neighbor.x, neighbor.y, grid, visited)) {
                visited[neighbor.x][neighbor.y] = true;
                parent[neighbor.x][neighbor.y] = pt;
                q.push({neighbor, current.dist + 1});
            }
        }
    }

    return {}; // No path found
}

int main() {
    vector<vector<int>> grid = {
        {0, 1, 0, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {1, 1, 0, 0, 0},
        {0, 0, 0, 1, 0}
    };

    Point start = {0, 0};
    Point end = {4, 4};

    vector<Point> path = bfsPathfinder(grid, start, end);

    if (!path.empty()) {
        cout << "Path found:\n";
        for (const auto& p : path) {
            cout << "(" << p.x << ", " << p.y << ") ";
        }
        cout << endl;
    } else {
        cout << "No path found.\n";
    }

    return 0;
}