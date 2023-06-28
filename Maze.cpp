#include "Maze.h"
#include "util.h"

#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>


Maze::Maze()
    : r{nullptr}
{
    //empty
}


void Maze::make(int height, int width, GenerationAlgorithm generationAlgorithm)
{
    this->height = height;
    this->width = width;
    this->edges = 0;
    graph.clear();
    graph.resize(height);
    for (int i = 0; i != height; ++i)
    {
        graph[i].resize(width);
        for (int j = 0; j != width; ++j)
        {
            graph[i][j].reserve(4);
        }
    }

    if (generationAlgorithm == GenerationAlgorithm::RecursiveBacktracker) makeRecursiveBacktracker();
    else if (generationAlgorithm == GenerationAlgorithm::RecursiveDivision) makeRecursiveDivision(Coord(0, 0), height, width);
    else if (generationAlgorithm == GenerationAlgorithm::Kruskals) makeKruskals();
    else if (generationAlgorithm == GenerationAlgorithm::BinaryTree) makeBinaryTree();
    else if (generationAlgorithm == GenerationAlgorithm::Ellers) makeEllers();
    else if (generationAlgorithm == GenerationAlgorithm::Sidewinder) makeSidewinder();
    else if (generationAlgorithm == GenerationAlgorithm::PrimsModified) makePrimsModified();
}


std::vector<Coord> Maze::neighbors(Coord coord, const std::vector<std::vector<std::vector<Coord>>>& graph, bool carved)
{
    std::vector<Coord> ret;
    ret.reserve(4);
    for (const auto addon : nesw_addons)
    {
        const int i2 = coord.i + addon.i;
        const int j2 = coord.j + addon.j;
        if (i2 >= 0 && i2 < height && j2 >= 0 && j2 < width)
        {
            if (carved ^ graph[i2][j2].empty()) ret.emplace_back(i2, j2);
        }
    }
    return ret;
}

void Maze::makeRecursiveBacktracker()
{
    std::vector<Coord> thestack;
    thestack.emplace_back(0, 0);
    do
    {
        const std::vector<Coord> neigh = neighbors(thestack.back(), graph, false);
        if (neigh.size())
        {
            Coord next = r->choice(neigh);
            graph[thestack.back().i][thestack.back().j].emplace_back(next.i, next.j);
            graph[next.i][next.j].emplace_back(thestack.back().i, thestack.back().j);
            ++edges;
            thestack.emplace_back(next.i, next.j);
        }
        else
        {
            thestack.pop_back();
        }
    } while (!thestack.empty());
}

void Maze::makeRecursiveDivision(Coord tl, int height, int width)
{
    if (height == 1)
    {
        for (int n = 0; n != width - 1; ++n)
        {
            graph[tl.i][tl.j + n].emplace_back(tl.i, tl.j + n + 1);
            graph[tl.i][tl.j + n + 1].emplace_back(tl.i, tl.j + n);
        }
        edges += width - 1;
        return;
    }
    if (width == 1)
    {
        for (int n = 0; n != height - 1; ++n)
        {
            graph[tl.i + n][tl.j].emplace_back(tl.i + n + 1, tl.j);
            graph[tl.i + n + 1][tl.j].emplace_back(tl.i + n, tl.j);
        }
        edges += height - 1;
        return;
    }

    if (r->bernoulli(static_cast<float>(height) / (height + width)))   //horizontal line, carving down
    {
        int i = r->uniformInt(0, height - 2);
        int j = r->uniformInt(0, width - 1);
        graph[tl.i + i][tl.j + j].emplace_back(tl.i + i + 1, tl.j + j);
        graph[tl.i + i + 1][tl.j + j].emplace_back(tl.i + i, tl.j + j);
        ++edges;

        makeRecursiveDivision(tl, i + 1, width);
        makeRecursiveDivision(Coord(tl.i + i + 1, tl.j), height - i - 1, width);
    }
    else                                                            //vertical line, carving right
    {
        int j = r->uniformInt(0, width - 2);
        int i = r->uniformInt(0, height - 1);
        graph[tl.i + i][tl.j + j].emplace_back(tl.i + i, tl.j + j + 1);
        graph[tl.i + i][tl.j + j + 1].emplace_back(tl.i + i, tl.j + j);
        ++edges;

        makeRecursiveDivision(tl, height, j + 1);
        makeRecursiveDivision(Coord(tl.i, tl.j + j + 1), height, width - j - 1);
    }
}

void Maze::makeKruskals()
{
    std::vector<std::pair<Coord, Coord>> edgesVec;
    edgesVec.reserve(2*height*width - height - width);
    for (int i = 0; i != height - 1; ++i)
    {
        edgesVec.emplace_back(Coord(i, width - 1), Coord(i + 1, width - 1));
        for (int j = 0; j != width - 1; ++j)
        {
            edgesVec.emplace_back(Coord(i, j), Coord(i, j + 1));
            edgesVec.emplace_back(Coord(i, j), Coord(i + 1, j));
        }
    }
    for (int j = 0; j != width - 1; ++j)
    {
        edgesVec.emplace_back(Coord(height - 1, j), Coord(height - 1, j + 1));
    }
    r->shuffle(edgesVec);

    GraphSets graphSets(height, width);
    int count = 0;
    for (const auto& edge : edgesVec)
    {
        if (!graphSets.same(edge.first, edge.second))
        {
            graph[edge.first.i][edge.first.j].emplace_back(edge.second.i, edge.second.j);
            graph[edge.second.i][edge.second.j].emplace_back(edge.first.i, edge.first.j);
            graphSets.merge(edge.first, edge.second);
            ++edges;
        }
        ++count;
    }
}

void Maze::makeBinaryTree()
{
    for (int i = 1; i != height; ++i)
    {
        graph[i][0].emplace_back(i - 1, 0);
        graph[i - 1][0].emplace_back(i, 0);
        for (int j = 1; j != width; ++j)
        {
            if (r->bernoulli(0.5))
            {
                graph[i][j].emplace_back(i, j - 1);
                graph[i][j - 1].emplace_back(i, j);
            }
            else
            {
                graph[i][j].emplace_back(i - 1, j);
                graph[i - 1][j].emplace_back(i, j);
            }
        }
    }

    for (int j = 1; j != width; ++j)
    {
        graph[0][j].emplace_back(0, j - 1);
        graph[0][j - 1].emplace_back(0, j);
    }

    edges = height * width - 1;
}

//TODO you can try using a vector instead of LL
//TODO go through and be meticulous with pointer management ... delete at the end and everyhting yea how do you delete LLNode hjuh
void Maze::makeEllers() //you can customize weights
{
    std::vector<LinkedList> LLVec;
    std::vector<int> vec;
    LLVec.reserve(width);
    vec.reserve(width);
    for (int j = 0; j < width; ++j)
    {
        LLVec.emplace_back(j);
        vec.emplace_back(j);
    }

    for (int i = 0; i < height - 1; ++i)
    {
        //horizontal carving
        for (int j = 0; j < width - 1; ++j)
        {
            if (vec[j] != vec[j + 1] && r->bernoulli(0.5))
            {
                graph[i][j].emplace_back(i, j + 1);
                graph[i][j + 1].emplace_back(i, j);
                ++edges;
                const int indexOfLargerLL = (LLVec[vec[j]].size >= LLVec[vec[j + 1]].size) ? vec[j] : vec[j + 1];
                const int indexOfSmallerLL = (LLVec[vec[j]].size >= LLVec[vec[j + 1]].size) ? vec[j + 1] : vec[j];
                LLVec[indexOfLargerLL].extend(LLVec[indexOfSmallerLL]);
                if (indexOfLargerLL != LLVec.size() - 1) //because then smaller LL still points to indexOfSmallerLL, so skip the pointing to indexOfLargerLL ... try without guard
                {
                    for (auto x = LLVec[indexOfSmallerLL].first; x != nullptr; x = x->next)
                    {
                        vec[x->val] = indexOfLargerLL;
                    }
                }
                if (indexOfSmallerLL != LLVec.size() - 1) //because youre repointing and "moving" LLVec.back() to preserve it, but here you want to pop LLVec.back() ... try without guard
                {
                    LLVec[indexOfSmallerLL] = LLVec.back();
                    for (auto x = LLVec.back().first; x != nullptr; x = x->next)
                    {
                        vec[x->val] = indexOfSmallerLL;
                    }
                }
                LLVec.back().first = nullptr;
                LLVec.pop_back();
            }
        }

        //vertical carving
        const int oldLLVecSize = LLVec.size();
        for (int LLVec_i = 0; LLVec_i < oldLLVecSize; ++LLVec_i)
        {
            auto& rLL = LLVec[LLVec_i];
            const int oldLLSize = rLL.size;
            const int guaranteed = r->uniformInt(0, rLL.size - 1);

            //go until first vertical carve
            int rLL_i = 0;
            for (/*empty*/; rLL_i != guaranteed && !r->bernoulli(0.5); ++rLL_i)
            {
                LLVec.emplace_back(rLL.first->val);
                vec[rLL.first->val] = LLVec.size() - 1;
                auto const deleteThisNode = rLL.first;
                rLL.first = rLL.first->next;
                delete deleteThisNode;
                --rLL.size;
            }

            //at this point, rLL.first is the first with a vertical carve, so unconditionally carve that
            auto x = rLL.first;
            graph[i][x->val].emplace_back(i + 1, x->val);
            graph[i + 1][x->val].emplace_back(i, x->val);
            ++edges;
            ++rLL_i; //now, x is staggered 1 behind rLL_i since you need to be able to delete nodes (and this isnt bidirectional because youre stupid)

            for (/*empty*/; rLL_i < oldLLSize; ++rLL_i)
            {
                if (rLL_i == guaranteed || r->bernoulli(0.5))
                {
                    x = x->next;
                    graph[i][x->val].emplace_back(i + 1, x->val);
                    graph[i + 1][x->val].emplace_back(i, x->val);
                    ++edges;
                }
                else
                {
                    LLVec.emplace_back(x->next->val);
                    vec[x->next->val] = LLVec.size() - 1;
                    auto const deleteThisNode = x->next;
                    x->next = x->next->next;
                    delete deleteThisNode;
                    --rLL.size;
                    if (rLL_i == oldLLSize - 1) rLL.last = x;
                }
            }
        }
    }

    //final row horizontal carving
    for (int j = 0; j < width - 1; ++j)
    {
        if (vec[j] != vec[j + 1])
        {
            graph[height - 1][j].emplace_back(height - 1, j + 1);
            graph[height - 1][j + 1].emplace_back(height - 1, j);
            ++edges;
            const int indexOfLargerLL = (LLVec[vec[j]].size >= LLVec[vec[j + 1]].size) ? vec[j] : vec[j + 1];
            const int indexOfSmallerLL = (LLVec[vec[j]].size >= LLVec[vec[j + 1]].size) ? vec[j + 1] : vec[j];
            LLVec[indexOfLargerLL].extend(LLVec[indexOfSmallerLL]);
            if (indexOfLargerLL != LLVec.size() - 1) //because then smaller LL still points to indexOfSmallerLL, so skip the pointing to indexOfLargerLL ... try without guard
            {
                for (auto x = LLVec[indexOfSmallerLL].first; x != nullptr; x = x->next)
                {
                    vec[x->val] = indexOfLargerLL;
                }
            }
            if (indexOfSmallerLL != LLVec.size() - 1) //because youre repointing and "moving" LLVec.back() to preserve it, but here you want to pop LLVec.back() ... try without guard
            {
                LLVec[indexOfSmallerLL] = LLVec.back();
                for (auto x = LLVec.back().first; x != nullptr; x = x->next)
                {
                    vec[x->val] = indexOfSmallerLL;
                }
            }
            LLVec.back().first = nullptr;
            LLVec.pop_back();
        }
    }


}

//you can customize horizontal carve chance
void Maze::makeSidewinder()
{
    for (int j = 0; j < width - 1; ++j)
    {
        graph[0][j].emplace_back(0, j + 1);
        graph[0][j + 1].emplace_back(0, j);
    }

    std::vector<int> vec;
    for (int i = 1; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            vec.emplace_back(j);
            if (j != width - 1 && r->bernoulli(0.5))
            {
                graph[i][j].emplace_back(i, j + 1);
                graph[i][j + 1].emplace_back(i, j);
            }
            else
            {
                int choice = r->choice(vec);
                graph[i][choice].emplace_back(i - 1, choice);
                graph[i - 1][choice].emplace_back(i, choice);
                vec.clear();
            }
        }
    }
    edges = width - 1 + (height - 1) * width;
}

void Maze::makePrimsModified()
{
    Coord curr(r->uniformInt(0, height - 1), r->uniformInt(0, width - 1));
    HashTableVector<Coord> frontiers(height > width ? height : width, [this](Coord coord) { return coord.i * width + coord.j; }); 

    for (const Coord add : nesw_addons)
    {
        const int xi = curr.i + add.i;
        const int xj = curr.j + add.j;
        if (xi >= 0 && xi < height && xj >= 0 && xj < width) frontiers.add(Coord(xi, xj));
    }

    const Coord oldCurr = curr;
    curr = frontiers.removeRandomElement(*r);
    graph[curr.i][curr.j].emplace_back(oldCurr.i, oldCurr.j);
    graph[oldCurr.i][oldCurr.j].emplace_back(curr.i, curr.j);
    ++edges;

    while (frontiers.size())
    {
        for (const Coord add : nesw_addons)
        {
            const int xi = curr.i + add.i;
            const int xj = curr.j + add.j;
            if (xi >= 0 && xi < height && xj >= 0 && xj < width && graph[xi][xj].empty()) frontiers.add(Coord(xi, xj));
        }
        curr = frontiers.removeRandomElement(*r);
        const Coord carveInto = r->choice(neighbors(curr, graph, true));
        graph[curr.i][curr.j].emplace_back(carveInto.i, carveInto.j);
        graph[carveInto.i][carveInto.j].emplace_back(curr.i, curr.j);
        ++edges;
    }
}