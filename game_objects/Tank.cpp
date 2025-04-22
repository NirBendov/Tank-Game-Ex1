#include "Moveable.h"
#include "Shell.h"
#include "Tank.h"
#include "./algorithms/Algorithm.h"
#include "./player/Player.h"

Tank::Tank(int lc[2], Direction d):Moveable(lc, d) {}

void Tank::aim(Direction newDir) {

}

Shell* Tank::shoot() {
    if (ammoCount > 0) {
        ammoCount--;
        return new Shell((int[2]){location[0], location[1]}, dir);
    }
}

Tank::~Tank()
{
}

void Tank::assignPlayerId(Player::PlayerId id) {
    playerId = id;
}

const Player::PlayerId Tank::getPlayerId() {
    return playerId;
}
