#include "Random.h"
#include "util.h"


#include <vector>
#include <algorithm>


Random::Random()
    : gen(std::random_device()()), uniformIntDistrib(0, 0), bernoulliDistrib{0}
{
    //empty
}

int Random::uniformInt(int lo, int hi)
{
    return uniformIntDistrib(gen, std::uniform_int_distribution<int>::param_type(lo, hi));
}

bool Random::bernoulli(float p)
{
    return bernoulliDistrib(gen, std::bernoulli_distribution::param_type(p));
}