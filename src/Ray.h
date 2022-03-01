#ifndef __RAY_H__
#define __RAY_H__

#include "Point.h"
#include "Vector.h"

namespace ToyMan {

class Ray {
public:
    Ray() : mint(0.f), maxt(INFINITY), time(0.f), depth(0) { }
    Ray(const Point &origin, const Vector &direction,
        float start, float end = INFINITY, float t = 0.f, int d = 0)
        : o(origin), d(direction), mint(start), maxt(end), time(t), depth(d) { }
    Ray(const Point &origin, const Vector &direction, const Ray &parent,
        float start, float end = INFINITY)
        : o(origin), d(direction), mint(start), maxt(end),
          time(parent.time), depth(parent.depth+1) { }
    Point operator()(float t) const { return o + d * t; }
    bool HasNaNs() const {
        return (o.HasNaNs() || d.HasNaNs() ||
                isnan(mint) || isnan(maxt));
    }

    Point o;
    Vector d;
    mutable float mint, maxt;
    float time;
    int depth;
};

} //namespace ToyMan

#endif //#ifndef __RAY_H__