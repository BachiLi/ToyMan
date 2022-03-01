#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cmath>
#include <cassert>
#include <limits>
#include <iostream>

using namespace std;

//the code is borrowed from pbrt
//http://www.pbrt.org

namespace ToyMan {

class Point;
class Normal;

class Vector {
public:
    // Vector Public Methods
    inline Vector() { x = y = z = 0.f;}
	inline Vector(float xx, float yy, float zz)
        : x(xx), y(yy), z(zz) {
        assert(!HasNaNs());
    }

    inline bool HasNaNs() const {
        return x == numeric_limits<float>::quiet_NaN() ||
               y == numeric_limits<float>::quiet_NaN() ||
               z == numeric_limits<float>::quiet_NaN();
    }    
    inline explicit Vector(const Point &p);
#ifndef NDEBUG
    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the Assert checks.
    inline Vector(const Vector &v) {
        assert(!v.HasNaNs());
        x = v.x; y = v.y; z = v.z;
    }

    inline Vector &operator=(const Vector &v) {
        assert(!v.HasNaNs());
        x = v.x; y = v.y; z = v.z;
        return *this;
    }
#endif // !NDEBUG
    inline Vector operator+(const Vector &v) const {
        assert(!v.HasNaNs());
		return Vector(x + v.x, y + v.y, z + v.z);
    }

    inline Vector& operator+=(const Vector &v) {
        assert(!v.HasNaNs());
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    inline Vector operator-(const Vector &v) const {
        assert(!v.HasNaNs());
        return Vector(x - v.x, y - v.y, z - v.z);
    }

    inline Vector& operator-=(const Vector &v) {
        assert(!v.HasNaNs());
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    inline bool operator==(const Vector &v) const {
        return x == v.x && y == v.y && z == v.z;
    }
    inline Vector operator*(float f) const { return Vector(f*x, f*y, f*z); }

    inline Vector &operator*=(float f) {
        assert(f != numeric_limits<float>::quiet_NaN());
        x *= f; y *= f; z *= f;
        return *this;
    }
    inline Vector operator/(float f) const {
        assert(f != 0);
        float inv = 1.f / f;
        return Vector(x * inv, y * inv, z * inv);
    }

    inline Vector &operator/=(float f) {
        assert(f != 0);
        float inv = 1.f / f;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    inline Vector operator-() const { return Vector(-x, -y, -z); }
    inline float operator[](int i) const {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }

    inline float &operator[](int i) {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }
    inline float LengthSquared() const {
		return x*x + y*y + z*z;
	}
    inline float Length() const { return sqrtf(LengthSquared()); }
    inline explicit Vector(const Normal &n);

    // Vector Public Data
	float x, y, z;
};

inline ostream& operator<<(ostream &out, const Vector &v) {
	out << "Vector(" << v.x << "," << v.y << "," << v.z << ")";
	return out;
}

inline Vector operator*(float f, const Vector &v) { return v*f; }

inline float Dot(const Vector &v1, const Vector &v2) {
    assert(!v1.HasNaNs() && !v2.HasNaNs());
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline float AbsDot(const Vector &v1, const Vector &v2) {
    assert(!v1.HasNaNs() && !v2.HasNaNs());
    return fabsf(Dot(v1, v2));
}

inline Vector Normalize(const Vector &v) { return v / v.Length(); }

inline Vector Cross(const Vector &v1, const Vector &v2) {
    assert(!v1.HasNaNs() && !v2.HasNaNs());
    float v1x = v1.x, v1y = v1.y, v1z = v1.z;
    float v2x = v2.x, v2y = v2.y, v2z = v2.z;
    return Vector((v1y * v2z) - (v1z * v2y),
                  (v1z * v2x) - (v1x * v2z),
                  (v1x * v2y) - (v1y * v2x));
}

};

#endif //#ifndef __VECTOR_H__
