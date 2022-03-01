#ifndef __AMBIENT_LIGHT_H__
#define __AMBIENT_LIGHT_H__

#include "../LightShader.h"

namespace ToyMan {

class ParameterList;

class AmbientLight : public LightShader {
public:
    AmbientLight(float intensity=1.f, const Color &lightColor=Color(1.f,1.f,1.f))
        : m_Intensity(intensity), m_LightColor(lightColor) {}

    bool IsAmbient() const {return true;}
    bool Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                       Vector *L, Color *Cl);

private:
    float m_Intensity;
    Color m_LightColor;
};

shared_ptr<LightShader> CreateAmbientLight(const ParameterList &params);

};

#endif //#ifndef __AMBIENT_LIGHT_H__