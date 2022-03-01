#ifndef __POINT_H__
#define __POINT_H__

#include "CommonDef.h"
#include "vector.h"
#include "Normal.h"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

//the code is borrowed from pbrt
//http://www.pbrt.org

namespace ToyMan {

class Point {
public:
    // Point Public Methods
    Point() { x = y = z = 0.f; }
    Point(float xx, float yy, float zz)
        : x(xx), y(yy), z(zz) {
        assert(!HasNaNs());
    }
#ifndef NDEBUG
    Point(const Point &p) {
        assert(!p.HasNaNs());
        x = p.x; y = p.y; z = p.z;
    }

    Point &operator=(const Point &p) {
        assert(!p.HasNaNs());
        x = p.x; y = p.y; z = p.z;
        return *this;
    }
#endif // !NDEBUG
    Point operator+(const Vector &v) const {
        assert(!v.HasNaNs());
        return Point(x + v.x, y + v.y, z + v.z);
    }

    Point &operator+=(const Vector &v) {
        assert(!v.HasNaNs());
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vector operator-(const Point &p) const {
        assert(!p.HasNaNs());
        return Vector(x - p.x, y - p.y, z - p.z);
    }

    Point operator-(const Vector &v) const {
        assert(!v.HasNaNs());
        return Point(x - v.x, y - v.y, z - v.z);
    }

    Point &operator-=(const Vector &v) {
        assert(!v.HasNaNs());
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    Point &operator+=(const Point &p) {
        assert(!p.HasNaNs());
        x += p.x; y += p.y; z += p.z;
        return *this;
    }
    Point operator+(const Point &p) const {
        assert(!p.HasNaNs());
        return Point(x + p.x, y + p.y, z + p.z);
    }
    Point operator* (float f) const {
        return Point(f*x, f*y, f*z);
    }
    Point &operator*=(float f) {
        x *= f; y *= f; z *= f;
        return *this;
    }
    Point operator/ (float f) const {
        float inv = 1.f/f;
        return Point(inv*x, inv*y, inv*z);
    }
    Point &operator/=(float f) {
        float inv = 1.f/f;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    float operator[](int i) const {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }

    float &operator[](int i) {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }
    bool HasNaNs() const {
        return isnan(x) || isnan(y) || isnan(z);
    }

    // Point Public Data
    float x, y, z;
};

inline ostream& operator<<(ostream &out, const Point &p) {
	out << "Point(" << p.x << "," << p.y << "," << p.z << ")";
	return out;
}

inline Point operator*(float f, const Point &p) {
    assert(!p.HasNaNs());
    return p*f;
}

inline float Distance(const Point &p1, const Point &p2) {
    return (p1 - p2).Length();
}

inline Point operator+(const Point &p, const Normal &n) {
    return Point(p.x+n.x,p.y+n.y,p.z+n.z);
}

inline Point operator-(const Point &p, const Normal &n) {
    return p+(-n);
}

};

#endif //#ifndef __POINT_H__

