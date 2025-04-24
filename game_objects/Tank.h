#pragma once
#include "Moveable.h"
#include "Shell.h"
#include "./algorithms/Algorithm.h"
#include "./player/Player.h"
class Tank: public Moveable
{
private:
    int ammoCount;
    Algorithm *algo;
    Player::PlayerId playerId;
public:
    Tank(int lc[2], int d[2]);
    ~Tank();
    void turn(Turn t); // changes tank's direction
    Shell* shoot(); // creates a new bullet with the same current location and direction
    void assignAlgorithm(Algorithm *a);
    const Algorithm *getAlgorithm();
    void assignPlayerId(Player::PlayerId id);
    const Player::PlayerId getPlayerId();
};

const int MAX_AMMO = 16;