#include "Solver.h"

#include <algorithm>


Solver::Solver(const Maze* m, Coord start, Coord end)
	: m{m}, solverAlgorithm{static_cast<SolverAlgorithm>(0)}, start{start}, end{end}
{
	//empty
}

Solver::Solver(const Maze* m, SolverAlgorithm solverAlgorithm, Coord start, Coord end)
	: m{m}, solverAlgorithm{solverAlgorithm}, start{start}, end{end}
{
	//empty
}


Coord Solver::getStart() const { return start; }


void Solver::set(const Maze* m, SolverAlgorithm solverAlgorithm, Coord start, Coord end)
{
	this->m = m;
	this->solverAlgorithm = solverAlgorithm;
	this->start = start;
	this->end = end;
	solution.clear();
}

void Solver::solve(SolverAlgorithm solverAlgorithm)
{
	if (solverAlgorithm == SolverAlgorithm::WallFollower) solveWallFollower();
}


Direction right(Direction facing)
{
	if (facing == Direction::W) return Direction::N;
	return static_cast<Direction>(facing + 1);
}

Direction left(Direction facing)
{
	if (facing == Direction::N) return Direction::W;
	return static_cast<Direction>(facing - 1);
}


//you can parameterize whether to follow right/left and initial facing 
void Solver::solveWallFollower()
{
	Coord p = start;
	Direction facing = Direction::N;

	while (!(p == end))
	{
		const std::vector<Coord>& conn = m->graph[p.i][p.j];
		Direction tryNewDir = right(facing);
		const auto& connBegin = conn.begin();
		const auto& connEnd = conn.end();
		if (std::find(connBegin, connEnd, p + nesw_addons[tryNewDir]) != connEnd);
		else if (std::find(connBegin, connEnd, p + nesw_addons[tryNewDir = facing]) != connEnd);
		else if (std::find(connBegin, connEnd, p + nesw_addons[tryNewDir = left(facing)]) != connEnd);
		else tryNewDir = static_cast<Direction>(facing ^ 2);

		facing = tryNewDir;
		if (solution.size() && static_cast<Direction>(tryNewDir ^ 2) == solution.back()) solution.pop_back();
		else solution.push_back(facing);
		p += nesw_addons[facing];
	}
}

