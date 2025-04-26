#include "Moveable.h"
#include "Shell.h"

    Shell::Shell(int location[2], int dir[2]) 
        : Moveable(location, dir), justFired(true) {
    }
