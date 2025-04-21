#pragma once
#include "Moveable.h"

class Shell: public Moveable
{
private:
public:
    Shell(int lc[2], Direction d);
    ~Shell();
};
