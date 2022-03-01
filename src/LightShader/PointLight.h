#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

#include "../LightShader.h"

namespace ToyMan {

class Transform;
class ParameterList;

class PointLight : public LightShader {
public:
    PointLight(float intensity=1.f, const Color &lightColor = Color(1.f,1.f,1.f),
                 const Point &from = Point(0.f, 0.f, 0.f))
                 : m_Intensity(intensity), m_LightColor(lightColor), m_From(from) {}

    bool IsAmbient() const {return false;}
    bool Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                       Vector *L, Color *Cl);

private:
    float m_Intensity;
    Color m_LightColor;
    Point m_From, m_To;  
};

shared_ptr<LightShader> CreatePointLight(
    const Transform &light2cam, const ParameterList &params);

} //namespace ToyMan

#endif //#ifndef __POINT_LIGHT_H__