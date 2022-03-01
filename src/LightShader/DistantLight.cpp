#include "DistantLight.h"
#include "../ParameterList.h"
#include "../Transform.h"
#include <iostream>

namespace ToyMan {

bool DistantLight::Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                         Vector *L, Color *Cl)
{
    Vector l;
    if(Solar(Ps, Ns, As, m_Dir, 0.f, &l)) {
        *L = -l;
        *Cl = m_Intensity * m_LightColor;
        return true;
    }
    return false;
}

shared_ptr<LightShader> CreateDistantLight(
    const Transform &light2cam, const ParameterList &params) {
    float intensity = params.FindFloat("intensity", 1.f);
    Color lightColor = params.FindColor("lightcolor", Color(1.f,1.f,1.f));
    Point from = light2cam(params.FindPoint("from", Point(0.f,0.f,0.f)));
    Point to = light2cam(params.FindPoint("to", Point(0.f,0.f,1.f)));
    cerr << "from:" << from << endl;
    cerr << "to" << to << endl;
    shared_ptr<LightShader> light
        = shared_ptr<LightShader>(new DistantLight(
            intensity, lightColor, from, to));
    return light;
}

};