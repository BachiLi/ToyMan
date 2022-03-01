#ifndef __UTIL_H__
#define __UTIL_H__

#include "Color.h"
#include "Point.h"
#include "RNG.h"
#include <cmath>
#include <string>
#include <vector>

using namespace std;

namespace ToyMan {

class AABB;

// Global Inline Functions
template<typename T>
inline T Lerp(float t, T v1, T v2) {
    return (1.f - t) * v1 + t * v2;
}

template<typename T>
inline T Clamp(T val, T low, T high) {
    if (val < low) return low;
    else if (val > high) return high;
    else return val;
}

inline Color Clamp(Color val, float low, float high) {
    val.x = Clamp(val.x, low, high);
    val.y = Clamp(val.y, low, high);
    val.z = Clamp(val.z, low, high);
    return val;
}

inline float Radians(float deg) {
    return ((float)M_PI/180.f) * deg;
}

inline float Degrees(float rad) {
    return (180.f/(float)M_PI) * rad;
}

inline int Round(float v) {
    return int(v+.5f);
}

inline Point Polar( float radius, float theta ) {
	return Point(cos(theta)*radius, sin(theta)*radius, 0.f);
}

inline float SphericalTheta(const Vector &v) {
    return acosf(Clamp(v.z, -1.f, 1.f));
}


inline float SphericalPhi(const Vector &v) {
    float p = atan2f(v.y, v.x);
    return (p < 0.f) ? p + 2.f*(float)(M_PI) : p;
}


inline string GetFileExtension(const string &filename) {
    size_t pos = filename.find_last_of('.');
    return pos != string::npos ? filename.substr(pos+1) : string("");
}

template <typename T>
inline void Shuffle(vector<T> &data, int count, int dims, RNG &rng) {
	for(int i = 0; i < count; i++) {
		int other = i + rng.NextInt(0, count-i-1);
		for(int j = 0; j < dims; j++) 
			swap(data[dims*i+j], data[dims*other+j]);
	}
}

void ConcentricSampleDisk(float u1, float u2, float *dx, float *dy);

inline void CoordinateSystem(const Vector &v1, Vector *v2, Vector *v3) {
    if (fabsf(v1.x) > fabsf(v1.y)) {
        float invLen = 1.f / sqrtf(v1.x*v1.x + v1.z*v1.z);
        *v2 = Vector(-v1.z * invLen, 0.f, v1.x * invLen);
    }
    else {
        float invLen = 1.f / sqrtf(v1.y*v1.y + v1.z*v1.z);
        *v2 = Vector(0.f, v1.z * invLen, -v1.y * invLen);
    }
    *v3 = Cross(v1, *v2);
}


inline unsigned int RoundUpPow2(unsigned int v) {
    v--;
    v |= v >> 1;    v |= v >> 2;
    v |= v >> 4;    v |= v >> 8;
    v |= v >> 16;
    return v+1;
}

inline int Mod(int a, int b) {
    int n = int(a/b);
    a -= n*b;
    if (a < 0) a += b;
    return a;
}

inline float Log2(float x) {
    const static float invLog2 = 1.f / log(2.f);
    return log(x) * invLog2;
}

inline float SmoothStep(float min, float max, float val) {
    if(val < min) return 0.f;
    if(val >= max) return 1.f;
    val = (val-min)/(max-min);
    return val*val*(3.f-2*val);
}


static const float OneMinusEpsilon=0.9999999403953552f;

inline float VanDerCorput(unsigned int n, unsigned int scramble) {
    // Reverse bits of _n_
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    n ^= scramble;
    return min(((n>>8) & 0xffffff) / float(1 << 24), OneMinusEpsilon);
}


inline float Sobol2(unsigned int n, unsigned int scramble) {
    for (unsigned int v = 1 << 31; n != 0; n >>= 1, v ^= v >> 1)
        if (n & 0x1) scramble ^= v;
    return min(((scramble>>8) & 0xffffff) / float(1 << 24), OneMinusEpsilon);
}

inline void Sample02(unsigned int n, const unsigned int scramble[2],
                     float sample[2]) {
    sample[0] = VanDerCorput(n, scramble[0]);
    sample[1] = Sobol2(n, scramble[1]);
}

};

#endif //#ifndef __UTIL_H__
