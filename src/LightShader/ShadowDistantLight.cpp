#include "ShadowDistantLight.h"
#include "../ParameterList.h"
#include "../Transform.h"

namespace ToyMan {

bool ShadowDistantLight::Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                         Vector *L, Color *Cl)
{
    Vector l;
    if(Solar(Ps, Ns, As, Normalize(m_To-m_From), 0.f, &l)) {
        *L = -l;
        *Cl = (1.f - m_ShadowMap.Lookup(rng, Ps, m_Blur, m_Samples, m_Bias)) * 
              m_Intensity * m_LightColor;
        return true;
    }
    return false;
}

shared_ptr<LightShader> CreateShadowDistantLight(
    const Transform &light2cam, const ParameterList &params, const Transform &cam2World) {
    float intensity = params.FindFloat("intensity", 1.f);
    Color lightColor = params.FindColor("lightcolor", Color(1.f,1.f,1.f));
    Point from = light2cam(params.FindPoint("from", Point(0.f,0.f,0.f)));
    Point to = light2cam(params.FindPoint("to", Point(0.f,0.f,1.f)));
    string shadowMapName = params.FindString("shadowmap", "");
    ShadowMap shadowMap(shadowMapName, cam2World);
    float blur = params.FindFloat("blur", 0.01f);
    int samples = (int)params.FindFloat("samples", 32.f);
    float bias = params.FindFloat("bias", 0.1f);
    shared_ptr<LightShader> light
        = shared_ptr<LightShader>(new ShadowDistantLight(
            intensity, lightColor, from, to, shadowMap));
    return light;
}


};