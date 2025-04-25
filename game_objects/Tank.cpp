#include "Tank.h"

Tank::Tank(int location[2], int dir[2])
  : Moveable(location, dir)
  , ammoCount(MAX_AMMO)
  , algo(nullptr)
  , playerId(Player::PlayerId(/* some default, e.g. Player::None */))
{}

Tank::~Tank() {
    delete algo;
}

void Tank::turn(Turn t) {
    auto newDir = getDirection(info.dir.data(), t);
    info.dir[0] = newDir[0];
    info.dir[1] = newDir[1];
    moveForward();
}

Shell* Tank::shoot() {
    if (ammoCount > 0) {
        --ammoCount;
        moveForward();
        return new Shell(info.location.data(), info.dir.data());
    }
    // TODO: throw or handle out-of-ammo
    return nullptr;
}

void Tank::assignAlgorithm(Algorithm *a) {
    algo = a;
}

const Algorithm* Tank::getAlgorithm() const {
    return algo;
}

void Tank::assignPlayerId(Player::PlayerId id) {
    playerId = id;
}

Player::PlayerId Tank::getPlayerId() const {
    return playerId;
}
