#include "AABB.h"

//the code is borrowed from pbrt
//http://www.pbrt.org

namespace ToyMan {

// AABB Method Definitions
AABB Union(const AABB &b, const Point &p) {
    AABB ret = b;
    ret.pMin.x = min(b.pMin.x, p.x);
    ret.pMin.y = min(b.pMin.y, p.y);
    ret.pMin.z = min(b.pMin.z, p.z);
    ret.pMax.x = max(b.pMax.x, p.x);
    ret.pMax.y = max(b.pMax.y, p.y);
    ret.pMax.z = max(b.pMax.z, p.z);
    return ret;
}


AABB Union(const AABB &b, const AABB &b2) {
    AABB ret;
    ret.pMin.x = min(b.pMin.x, b2.pMin.x);
    ret.pMin.y = min(b.pMin.y, b2.pMin.y);
    ret.pMin.z = min(b.pMin.z, b2.pMin.z);
    ret.pMax.x = max(b.pMax.x, b2.pMax.x);
    ret.pMax.y = max(b.pMax.y, b2.pMax.y);
    ret.pMax.z = max(b.pMax.z, b2.pMax.z);
    return ret;
}


void AABB::BoundingSphere(Point *c, float *rad) const {
    *c = .5f * pMin + .5f * pMax;
    *rad = Inside(*c) ? Distance(*c, pMax) : 0.f;
}

void AABB::GetAllPoints(Point p[8]) const {
	p[0] = Point(pMin.x, pMin.y, pMin.z);
	p[1] = Point(pMin.x, pMin.y, pMax.z);
	p[2] = Point(pMin.x, pMax.y, pMin.z);
	p[3] = Point(pMin.x, pMax.y, pMax.z);
	p[4] = Point(pMax.x, pMin.y, pMin.z);
	p[5] = Point(pMax.x, pMin.y, pMax.z);
	p[6] = Point(pMax.x, pMax.y, pMin.z);
	p[7] = Point(pMax.x, pMax.y, pMax.z);
}

};
