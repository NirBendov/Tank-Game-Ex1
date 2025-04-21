#include "Moveable.h"
#include "Shell.h"
#include "Tank.h"

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
