#include "ShadowPointLight.h"
#include "../Transform.h"
#include "../ParameterList.h"
#include "../RayTracer.h"
#include "../Normal.h"

namespace ToyMan {

bool ShadowPointLight::Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As,
                    Vector *L, Color *Cl) {
    if(!m_RayTracer->Visibility(Ps+0.0001f*Vector(Ns), m_From))
        return false;
    Vector dir(0.f,-1.f,0.f);
    Vector l;
    if(Illuminate(Ps, Ns, As, m_From, dir, (float)M_PI_2, &l)) {        
        float dist = l.Length();
        *Cl = (m_Intensity*m_LightColor*pow(dist, -2.f))*
              (Dot(l,dir) / dist);
        *L = -l;
        return true;
    }
    return false;
    //*L = m_From - Ps;
    //float invLen2 = 1.f/ L->LengthSquared();
    //*Cl = m_Intensity * m_LightColor * invLen2;
    //*L *= sqrtf(invLen2);
    //return true;
}

shared_ptr<LightShader> CreateShadowPointLight(
    const Transform &light2cam,
    shared_ptr<RayTracer> rayTracer, 
    const ParameterList &params) {
    
    float intensity = params.FindFloat("intensity", 1.f);
    Color lightColor = params.FindColor("lightcolor", Color(1.f,1.f,1.f));
    Point from = light2cam(params.FindPoint("from", Point(0.f,0.f,0.f)));
    shared_ptr<LightShader> light
        = shared_ptr<LightShader>(new ShadowPointLight(
            rayTracer, intensity, lightColor, from));
    return light;
}

} //namespace ToyMan
