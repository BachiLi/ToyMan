#include "RNG.h"

#include <cstdlib>
#include <cassert>

using namespace std;

namespace ToyMan {

RNG RNG::s_GlobalRNG;

RNG::RNG(unsigned int seed) {
    engine.seed(seed);
}

int RNG::NextInt(int start, int end) {
    assert(end>=start);
    tr1::uniform_int<int> rng(start, end);
    return rng(engine);
}

unsigned int RNG::NextUInt(
    unsigned int start, unsigned int end) {
    assert(end>=start);
    tr1::uniform_int<unsigned int> rng(start, end);
    return rng(engine);
}

float RNG::NextFloat(float start, float end) {
    assert(end>=start);
    tr1::uniform_real<float> uni(start, end);
    return uni(engine);
}

};
