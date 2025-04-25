#include "../game_objects/Tank.h"
#include <vector>
using namespace std;

class Player
{
private:
    vector<Tank> tanks;
public:
    enum PlayerId {
        P1 , P2
    };
    PlayerId id;
    Player(/* args */);
    ~Player();
    void AssignPlayerIdToTanks();
};