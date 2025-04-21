#pragma once
#include "Moveable.h"
#include "Shell.h"

class Tank: public Moveable
{
private:
    int ammoCount;
public:
    Tank(int lc[2], Direction d);
    ~Tank();
    void aim(Direction newDir); // changes tank's direction
    Shell* shoot(); // creates a new bullet with the same current location and direction
};