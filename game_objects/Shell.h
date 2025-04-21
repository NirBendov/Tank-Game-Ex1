#ifndef SHELL_H
#define SHELL_H
#include "Moveable.h"

class Shell: public Moveable
{
private:
public:
    Shell(int lc[2], Direction d);
    ~Shell();
};

#endif SHELL_H