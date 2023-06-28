#pragma once


#include "Maze.h"
#include "util.h"


enum SolverAlgorithm
{
	WallFollower = 0
};

class Solver
{
private:
	const Maze* m;
	SolverAlgorithm solverAlgorithm;
	Coord start;
	Coord end;

public:
	std::vector<Direction> solution;

	Solver(const Maze* m, Coord start, Coord end);
	Solver(const Maze* m, SolverAlgorithm solverAlgorithm, Coord start, Coord end);
	void set(const Maze* m, SolverAlgorithm solverAlgorithm, Coord start, Coord end);

	Coord getStart() const;

	void solve(SolverAlgorithm solverAlgorithm);
	void solveWallFollower();
};