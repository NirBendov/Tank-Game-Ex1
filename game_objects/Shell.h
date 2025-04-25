#pragma once
#include "Moveable.h"

class Shell: public Moveable
{
private:
public:
    Shell(int location[2], int direction[2]);
    ~Shell();
};
