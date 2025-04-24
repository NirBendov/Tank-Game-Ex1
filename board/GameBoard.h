#include <vector>
using namespace std;

class GameBoard
{
private:
    int height, width;
    vector<vector<char>> board;
public:
    class BoardData {
        private:
        public:
            int tank1Position[2];
            int tank2Position[2];
            vector<int[]> bulletsPositions;
            BoardData();
            ~BoardData();
    };
    BoardData data;
    GameBoard(/* args */);
    ~GameBoard();
};