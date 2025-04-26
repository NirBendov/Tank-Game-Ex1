#pragma once
class Tank;  // Forward declaration
#include <vector>
using namespace std;

class Player
{
private:
    vector<Tank*> tanks;  // Changed to pointer to avoid incomplete type issues
public:
    enum PlayerId {
        P1 , P2
    };
    PlayerId id;
    Player();
    ~Player();
    void AssignPlayerIdToTanks();
};