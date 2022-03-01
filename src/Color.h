#ifndef __COLOR_H__
#define __COLOR_H__

#include <iostream>
#include <cassert>
#include <limits>

using namespace std;

namespace ToyMan {

/**
 * @brief A 3 tuple represents a 3 components color
 */
class Color {
public:
    // Color Public Methods
    inline Color() { x = y = z = 0.f; }
    inline Color(float xx, float yy, float zz)
        : x(xx), y(yy), z(zz) {
        assert(!HasNaNs());
    }
#ifndef NDEBUG
    inline Color(const Color &c) {
        assert(!c.HasNaNs());
        x = c.x; y = c.y; z = c.z;
    }

    inline Color &operator=(const Color &c) {
        assert(!c.HasNaNs());
        x = c.x; y = c.y; z = c.z;
        return *this;
    }
#endif // !NDEBUG
    inline Color operator+(const Color &c) const {
        assert(!c.HasNaNs());
        return Color(x + c.x, y + c.y, z + c.z);
    }

    inline Color &operator+=(const Color &c) {
        assert(!c.HasNaNs());
        x += c.x; y += c.y; z += c.z;
        return *this;
    }

    inline Color operator-(const Color &c) const {
        assert(!c.HasNaNs());
        return Color(x - c.x, y - c.y, z - c.z);
    }

    inline Color &operator-=(const Color &c) {
        assert(!c.HasNaNs());
        x -= c.x; y -= c.y; z -= c.z;
        return *this;
    }

    inline Color operator* (const Color &c) const {
        return Color(c.x*x, c.y*y, c.z*z);
    }
    inline Color &operator*=(const Color &c) {
        x *= c.x; y *= c.y; z *= c.z;
        return *this;
    }

    inline Color operator* (float f) const {
        return Color(f*x, f*y, f*z);
    }
    inline Color &operator*=(float f) {
        x *= f; y *= f; z *= f;
        return *this;
    }
    inline Color operator/ (float f) const {
        float inv = 1.f/f;
        return Color(inv*x, inv*y, inv*z);
    }
    inline Color &operator/=(float f) {
        float inv = 1.f/f;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    inline float operator[](int i) const {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }

    inline float &operator[](int i) {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }
    inline bool HasNaNs() const {
        return x == numeric_limits<float>::quiet_NaN() ||
               y == numeric_limits<float>::quiet_NaN() ||
               z == numeric_limits<float>::quiet_NaN();
    }

    float Y() const {
        const float YWeight[3] = { 0.212671f, 0.715160f, 0.072169f };
        return YWeight[0] * x + YWeight[1] * y + YWeight[2] * z;
    }

    // Color Public Data
    float x, y, z;
};


inline Color operator*(float f, const Color &c) {
    return c*f;
}

inline ostream& operator<<(ostream &out, const Color &c) {
    out << "Color(" << c.x << "," << c.y << "," << c.z << ")";
    return out;
}

};

#endif //#ifndef __COLOR_H__
