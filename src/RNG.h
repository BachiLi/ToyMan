#ifndef __RNG_H__
#define __RNG_H__

#ifdef _MSC_VER
	#include <random>
#else
	#include <tr1/random>
#endif

using namespace std;

namespace ToyMan {

class RNG {
public:
    RNG(unsigned int seed=5227);
    //return a random int range [start, end]
    int NextInt(int start, int end);
    unsigned int NextUInt(unsigned int start, unsigned int end);
    //return a random float  range [start, end)
    float NextFloat(float start = 0.f, float end = 1.f);
    static RNG* GetGlobalRNG() {return &s_GlobalRNG;}
private:
    static RNG s_GlobalRNG;
    tr1::minstd_rand engine;
};

};

#endif //#ifndef __RNG_H__


