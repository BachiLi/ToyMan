#include "AmbientLight.h"
#include "../ParameterList.h"

namespace ToyMan {

bool AmbientLight::Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                       Vector *L, Color *Cl) {
    *L = Vector(0.f,0.f,0.f);
    *Cl = m_Intensity*m_LightColor;
    return true;
}

shared_ptr<LightShader> CreateAmbientLight(const ParameterList &params) {
    float intensity = params.FindFloat("intensity", 1.f);
    Color lightColor = params.FindColor("lightcolor", Color(1.f,1.f,1.f));
    shared_ptr<LightShader> light
        = shared_ptr<LightShader>(new AmbientLight(
            intensity, lightColor));
    return light;
}

};