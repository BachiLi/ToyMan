#ifndef __NORMAL_H__
#define __NORMAL_H__

#include "CommonDef.h"
#include "Vector.h"
#include <cassert>
#include <iostream>

using namespace std;

//the code is borrowed from pbrt
//http://www.pbrt.org

namespace ToyMan {

class Normal {
public:
    // Normal Public Methods
    inline Normal() { x = y = z = 0.f; }
    inline Normal(float xx, float yy, float zz)
        : x(xx), y(yy), z(zz) {
        assert(!HasNaNs());
    }
    inline Normal operator-() const {
        return Normal(-x, -y, -z);
    }

    inline Normal operator+ (const Normal &n) const {
        assert(!n.HasNaNs());
        return Normal(x + n.x, y + n.y, z + n.z);
    }

    inline Normal& operator+=(const Normal &n) {
        assert(!n.HasNaNs());
        x += n.x; y += n.y; z += n.z;
        return *this;
    }
    inline Normal operator- (const Normal &n) const {
        assert(!n.HasNaNs());
        return Normal(x - n.x, y - n.y, z - n.z);
    }

    inline Normal& operator-=(const Normal &n) {
        assert(!n.HasNaNs());
        x -= n.x; y -= n.y; z -= n.z;
        return *this;
    }
    inline bool HasNaNs() const {
        return isnan(x) || isnan(y) || isnan(z);
    }
    inline Normal operator*(float f) const {
        return Normal(f*x, f*y, f*z);
    }

    inline Normal &operator*=(float f) {
        x *= f; y *= f; z *= f;
        return *this;
    }
    inline Normal operator/(float f) const {
        assert(f != 0);
        float inv = 1.f/f;
        return Normal(x * inv, y * inv, z * inv);
    }

    inline Normal &operator/=(float f) {
        assert(f != 0);
        float inv = 1.f/f;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    inline float LengthSquared() const { return x*x + y*y + z*z; }
    inline float Length() const        { return sqrtf(LengthSquared()); }

#ifndef NDEBUG
    inline Normal(const Normal &n) {
        assert(!n.HasNaNs());
        x = n.x; y = n.y; z = n.z;
    }

    inline Normal &operator=(const Normal &n) {
        assert(!n.HasNaNs());
        x = n.x; y = n.y; z = n.z;
        return *this;
    }
#endif // !NDEBUG
    inline explicit Normal(const Vector &v)
      : x(v.x), y(v.y), z(v.z) {
        assert(!v.HasNaNs());
    }
    inline float operator[](int i) const {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }

    inline float &operator[](int i) {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }

    // Normal Public Data
    float x, y, z;
};

inline Vector::Vector(const Normal &n) {
    x = n.x; y = n.y; z = n.z;
}

inline ostream& operator<<(ostream &out, const Normal &n) {
	out << "Normal(" << n.x << "," << n.y << "," << n.z << ")";
	return out;
}

inline Normal operator*(float v, const Normal &n) {
    return n*v;
}

inline Normal Normalize(const Normal &n) {
    return n / n.Length();
}

inline float Dot(const Normal &n1, const Vector &v2) {
    assert(!n1.HasNaNs() && !v2.HasNaNs());
    return n1.x * v2.x + n1.y * v2.y + n1.z * v2.z;
}


inline float Dot(const Vector &v1, const Normal &n2) {
    assert(!v1.HasNaNs() && !n2.HasNaNs());
    return v1.x * n2.x + v1.y * n2.y + v1.z * n2.z;
}


inline float Dot(const Normal &n1, const Normal &n2) {
    assert(!n1.HasNaNs() && !n2.HasNaNs());
    return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}


inline float AbsDot(const Normal &n1, const Vector &v2) {
    assert(!n1.HasNaNs() && !v2.HasNaNs());
    return fabsf(n1.x * v2.x + n1.y * v2.y + n1.z * v2.z);
}

inline float AbsDot(const Vector &v1, const Normal &n2) {
    assert(!v1.HasNaNs() && !n2.HasNaNs());
    return fabsf(v1.x * n2.x + v1.y * n2.y + v1.z * n2.z);
}


inline float AbsDot(const Normal &n1, const Normal &n2) {
    assert(!n1.HasNaNs() && !n2.HasNaNs());
    return fabsf(n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);
}


inline Normal Faceforward(const Normal &n, const Vector &v) {
    return (Dot(n, v) < 0.f) ? -n : n;
}


inline Normal Faceforward(const Normal &n, const Normal &n2) {
    return (Dot(n, n2) < 0.f) ? -n : n;
}



inline Vector Faceforward(const Vector &v, const Vector &v2) {
    return (Dot(v, v2) < 0.f) ? -v : v;
}



inline Vector Faceforward(const Vector &v, const Normal &n2) {
    return (Dot(v, n2) < 0.f) ? -v : v;
}

};

#endif //#ifndef __NORMAL_H__
