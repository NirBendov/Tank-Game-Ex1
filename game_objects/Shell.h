#pragma once
#include "Moveable.h"

class Shell: public Moveable
{
private:
    bool justFired;
public:
    Shell(int lc[2], int d[2]);
    ~Shell() override = default;
    void setJustFired(bool value) { justFired = value; }
    bool getJustFired() const { return justFired; }
};
