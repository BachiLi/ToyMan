#include "PointLight.h"
#include "../Transform.h"
#include "../ParameterList.h"

namespace ToyMan {

bool PointLight::Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                    Vector *L, Color *Cl) {
    
    *L = m_From - Ps;
    float invLen2 = 1.f/ L->LengthSquared();
    *Cl = m_Intensity * m_LightColor * invLen2;
    *L *= sqrtf(invLen2);
    return true;
}

shared_ptr<LightShader> CreatePointLight(
    const Transform &light2cam, const ParameterList &params) {

    float intensity = params.FindFloat("intensity", 1.f);
    Color lightColor = params.FindColor("lightcolor", Color(1.f,1.f,1.f));
    Point from = light2cam(params.FindPoint("from", Point(0.f,0.f,0.f)));
    shared_ptr<LightShader> light
        = shared_ptr<LightShader>(new PointLight(
            intensity, lightColor, from));
    return light;
}

} //namespace ToyMan
