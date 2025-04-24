#include "Moveable.h"
#include "Shell.h"
#include "Tank.h"
#include "Direction.h"
#include "./algorithms/Algorithm.h"
#include "./player/Player.h"

Tank::Tank(int lc[2], int d[2]):Moveable(lc, d) {
    ammoCount = MAX_AMMO;
}

void Tank::turn(Turn t) {
    *dir = *getDirection(dir, t);
}

Shell* Tank::shoot() {
    if (ammoCount > 0) {
        ammoCount--;
        return new Shell((int[2]){location[0], location[1]}, dir);
    }
}

// Tanks should only attack tanks with different playerId than their own
void Tank::assignPlayerId(Player::PlayerId id) {
    playerId = id;
}

const Player::PlayerId Tank::getPlayerId() {
    return playerId;
}
