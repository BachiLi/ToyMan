#ifndef __AABB_H__
#define __AABB_H__

//the code is borrowed from pbrt
//http://www.pbrt.org

#include "CommonDef.h"
#include "Util.h"
#include "Point.h"
#include <algorithm>

using namespace std;

namespace ToyMan {

/**
 *  @brief A axis-aligned bounding box
 */
class AABB {
public:
    // AABB Public Methods
    AABB() {
        pMin = Point( INFINITY,  INFINITY,  INFINITY);
        pMax = Point(-INFINITY, -INFINITY, -INFINITY);
    }
    AABB(const Point &p) : pMin(p), pMax(p) { }
    AABB(const Point &p1, const Point &p2) {
        pMin = Point(min(p1.x, p2.x), min(p1.y, p2.y), min(p1.z, p2.z));
        pMax = Point(max(p1.x, p2.x), max(p1.y, p2.y), max(p1.z, p2.z));
    }
    friend AABB Union(const AABB &b, const Point &p);
    friend AABB Union(const AABB &b, const AABB &b2);
    bool Overlaps(const AABB &b) const {
        bool x = (pMax.x >= b.pMin.x) && (pMin.x <= b.pMax.x);
        bool y = (pMax.y >= b.pMin.y) && (pMin.y <= b.pMax.y);
        bool z = (pMax.z >= b.pMin.z) && (pMin.z <= b.pMax.z);
        return (x && y && z);
    }
    bool Inside(const Point &pt) const {
        return (pt.x >= pMin.x && pt.x <= pMax.x &&
                pt.y >= pMin.y && pt.y <= pMax.y &&
                pt.z >= pMin.z && pt.z <= pMax.z);
    }
    void Expand(float delta) {
        pMin -= Vector(delta, delta, delta);
        pMax += Vector(delta, delta, delta);
    }
    float SurfaceArea() const {
        Vector d = pMax - pMin;
        return 2.f * (d.x * d.y + d.x * d.z + d.y * d.z);
    }
    float Volume() const {
        Vector d = pMax - pMin;
        return d.x * d.y * d.z;
    }
    int MaximumExtent() const {
        Vector diag = pMax - pMin;
        if (diag.x > diag.y && diag.x > diag.z)
            return 0;
        else if (diag.y > diag.z)
            return 1;
        else
            return 2;
    }
    const Point &operator[](int i) const;
    Point &operator[](int i);
    Point Lerp(float tx, float ty, float tz) const {
        return Point(ToyMan::Lerp(tx, pMin.x, pMax.x), ToyMan::Lerp(ty, pMin.y, pMax.y),
                     ToyMan::Lerp(tz, pMin.z, pMax.z));
    }
    Vector Offset(const Point &p) const {
        return Vector((p.x - pMin.x) / (pMax.x - pMin.x),
                      (p.y - pMin.y) / (pMax.y - pMin.y),
                      (p.z - pMin.z) / (pMax.z - pMin.z));
    }
    void BoundingSphere(Point *c, float *rad) const;
	void GetAllPoints(Point p[8]) const;

    // AABB Public Data
    Point pMin, pMax;
};

};

#endif //#ifndef __BBOX_H__
