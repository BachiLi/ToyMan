#ifndef __SHADOW_DISTANT_LIGHT_H__
#define __SHADOW_DISTANT_LIGHT_H__

#include "../LightShader.h"
#include "../Color.h"
#include "../ShadowMap.h"
#include <string>

using namespace std;

namespace ToyMan {

class ParameterList;
class Transform;

class ShadowDistantLight : public LightShader {
public:
    ShadowDistantLight(float intensity, const Color &lightColor,
        const Point &from, const Point &to, const ShadowMap &shadowMap,
        float blur=0.01f, int samples=32, float bias=0.1f)
        : m_Intensity(intensity), m_LightColor(lightColor),
          m_From(from), m_To(to), m_ShadowMap(shadowMap), 
          m_Blur(blur), m_Samples(samples), m_Bias(bias) {}

    virtual bool IsAmbient() const {return false;}
    virtual bool Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                       Vector *L, Color *Cl);

private:
    float m_Intensity;
    Color m_LightColor;
    Point m_From, m_To;

    ShadowMap m_ShadowMap;
    float m_Blur;
    int m_Samples;
    float m_Bias;
};

shared_ptr<LightShader> CreateShadowDistantLight(
    const Transform &light2cam, const ParameterList &params, const Transform &cam2World);

};

#endif //#ifndef __SHADOW_DISTANT_LIGHT_H__