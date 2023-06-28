#pragma once

#include <random>
#include <algorithm>
#include <iterator>

class Random
{
private:
    std::mt19937 gen;
    std::uniform_int_distribution<int> uniformIntDistrib;
    std::bernoulli_distribution bernoulliDistrib;

public:
    Random();
    int uniformInt(int lo, int hi);

    template <typename T>
    T choice(const std::vector<T>& vec)
    {
        return vec[uniformInt(0, vec.size() - 1)];
    }

    bool bernoulli(float p);

    template <typename T>
    void shuffle(T& container)
    {
        std::shuffle(container.begin(), container.end(), gen);
    }
};