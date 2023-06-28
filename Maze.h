#pragma once


#include "util.h"
#include "Random.h"

#include <random>
#include <vector>
#include <set>
#include <utility>

enum GenerationAlgorithm
{
    RecursiveBacktracker = 0,
    RecursiveDivision = 1,
    Kruskals = 2,
    BinaryTree = 3,
    Ellers = 4,
    Sidewinder = 5,
    PrimsModified = 6
};


enum Direction
{
    N = 0,
    E = 1,
    S = 2,
    W = 3
};


const Coord nesw_addons[4] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}; //NESW


class Maze
{
public:
    int height;
    int width;
    std::vector<std::vector<std::vector<Coord>>> graph;
    int edges;
    Random* r;

    Maze();
    void make(int height, int width, GenerationAlgorithm generationAlgorithm);

    struct CoordHasher
    {
        int operator()(Coord coord) const { return 40000 * coord.i + coord.j; };
    };

private:
    std::vector<Coord> neighbors(Coord node, const std::vector<std::vector<std::vector<Coord>>>& graph, bool carved);
    void makeRecursiveBacktracker();
    void makeRecursiveDivision(Coord tl, int height, int width);
    void makeKruskals();
    void makeBinaryTree();
    void makeEllers();
    void makeSidewinder();
    void makePrimsModified();
};