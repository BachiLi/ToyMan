#ifndef __DISTANT_LIGHT_H__
#define __DISTANT_LIGHT_H__

#include "../LightShader.h"
#include "../Color.h"

namespace ToyMan {

class ParameterList;
class Transform;

class DistantLight : public LightShader {
public:
    DistantLight(float intensity=1.f, const Color &lightColor = Color(1.f,1.f,1.f),
                 const Point &from = Point(0.f, 0.f, 0.f), const Point &to = Point(0.f, 0.f, 1.f))
                 : m_Intensity(intensity), m_LightColor(lightColor) {
        m_Dir = Normalize(to-from);
    }

    bool IsAmbient() const {return false;}
    bool Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                       Vector *L, Color *Cl);

private:
    float m_Intensity;
    Color m_LightColor;
    Vector m_Dir;
};

shared_ptr<LightShader> CreateDistantLight(
    const Transform &light2cam, const ParameterList &params);

};

#endif //#ifndef __DISTANT_LIGHT_H__