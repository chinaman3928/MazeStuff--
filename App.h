#pragma once


#include "Maze.h"
#include "Random.h"


class App
{
public:
    App();
    void run();

private:
    Random r;
    Maze m;
};