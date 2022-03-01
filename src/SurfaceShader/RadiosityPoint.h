#ifndef __RADIOSITY_POINT_H__
#define __RADIOSITY_POINT_H__

#include "../Point.h"
#include "../Normal.h"
#include "../Color.h"
#include <iostream>
#include <array>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

class RNG;
    
struct RadiosityPoint {
    RadiosityPoint() {}
    RadiosityPoint(
        const Point &_P, const Normal &_N, 
        const Color &_Ci, float _area)
        : P(_P), N(_N), Ci(_Ci), radius(sqrt(_area/(float)M_PI)) {}    

    void SHProject(RNG &rng, 
        array<Color, 9> &shPower, 
        array<float, 9> &shArea,
        unsigned int nSamples = 32);

    Point P;
    Normal N;
    Color Ci;
    float radius;    
};

inline ostream& operator<<(ostream &out, const RadiosityPoint &rPt) {
    out << rPt.P.x << " " << rPt.P.y << " " << rPt.P.z << "  ";
    out << rPt.N.x << " " << rPt.N.y << " " << rPt.N.z << "  ";
    out << rPt.Ci.x << " " << rPt.Ci.y << " " << rPt.Ci.z << "  ";
    out << rPt.radius;
    return out;
}

inline istream& operator>>(istream &in, RadiosityPoint &rPt) {
    in >> rPt.P.x >> rPt.P.y >> rPt.P.z >>
          rPt.N.x >> rPt.N.y >> rPt.N.z >>
          rPt.Ci.x >> rPt.Ci.y >> rPt.Ci.z >>
          rPt.radius;
    return in;
}

} //namespace ToyMan

#endif //#ifndef __RADIOSITY_POINT_H__