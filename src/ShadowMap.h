#ifndef __SHADOW_MAP_H__
#define __SHADOW_MAP_H__

#include "TwoDArray.h"
#include "Point.h"
#include "Transform.h"
#include <string>
#include <exception>

using namespace std;

namespace ToyMan {

class RNG;

class ShadowMapDepthFileLoadException : public exception {
public:
    virtual const char* what() const throw()
    {
        return "The depth file provided is not corrected.";
    }
};

class ShadowMap {
public:
    ShadowMap() {}
    ShadowMap(const string &filename, const Transform &cameraToWorld);
    static void MakeShadow(const string &from, const string &to,
                           const Transform &worldToLight,
                           const Transform &lightToRaster);

    float Lookup(RNG &rng, const Point &Ps, 
        float blur = 0.01f, int samples = 32, float bias=0.1f);

private:
    TwoDArray<float> m_DepthMap;
    Transform m_CameraToLight, m_LightToRaster;
};

};

#endif //#ifndef __SHADOW_MAP_H__