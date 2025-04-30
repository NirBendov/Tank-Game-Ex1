#include "Player.h"
#include "../game_objects/Tank.h"
#include <vector>
using namespace std;

Player::Player()
{
}

Player::~Player()
{
}

void Player::AssignPlayerIdToTanks() {
    for (size_t i = 0; i < tanks.size(); ++i) {
        tanks[i]->assignPlayerId(id);
    }
}
