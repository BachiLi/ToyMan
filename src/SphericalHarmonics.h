#ifndef __SPHERICAL_HARMONICS_H__
#define __SPHERICAL_HARMONICS_H__

namespace ToyMan {

class Vector;
class Color;

inline int SHTerms(int lmax) {
    return (lmax + 1) * (lmax + 1);
}

inline int SHIndex(int l, int m) {
    return l*l + l + m;
}

void SHEvaluate(const Vector &v, int lmax, float *out);

void SHReduceRinging(Color *c, int lmax, float lambda = .005f);

} //namespace ToyMan

#endif //#ifndef __SPHERICAL_HARMONICS_H__