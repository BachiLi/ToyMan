#ifndef __SHADOW_POINT_LIGHT_H__
#define __SHADOW_POINT_LIGHT_H__

#include "../LightShader.h"
#include <memory>

namespace ToyMan {

class Transform;
class ParameterList;
class RayTracer;

class ShadowPointLight : public LightShader {
public:
    ShadowPointLight(shared_ptr<RayTracer> rayTracer, float intensity=1.f, 
                     const Color &lightColor = Color(1.f,1.f,1.f),
                     const Point &from = Point(0.f, 0.f, 0.f))
                 : m_RayTracer(rayTracer), m_Intensity(intensity), m_LightColor(lightColor), 
                   m_From(from) {}

    bool IsAmbient() const {return false;}
    bool Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                       Vector *L, Color *Cl);

private:
    shared_ptr<RayTracer> m_RayTracer;
    float m_Intensity;
    Color m_LightColor;
    Point m_From, m_To;      
};

shared_ptr<LightShader> CreateShadowPointLight(
    const Transform &light2cam,
    shared_ptr<RayTracer> rayTracer, 
    const ParameterList &params);

} //namespace ToyMan

#endif //#ifndef __SHADOW_POINT_LIGHT_H__