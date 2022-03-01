#ifndef __PLASTIC_H__
#define __PLASTIC_H__

#include "../SurfaceShader.h"
#include "../Color.h"

namespace ToyMan {

class ParameterList;

class Plastic : public SurfaceShader {
public:
    Plastic(float Ka=1.f, float Kd=0.5f, float Ks=0.5f,
        float roughness=0.1f, const Color &specularColor = Color(1.f,1.f,1.f))
        : m_Ka(Ka), m_Kd(Kd), m_Ks(Ks),
          m_Roughness(roughness), m_SpecularColor(specularColor) {}

    virtual void Shade(RNG &rng, MicroPolygonGrid *grid);

private:
    float m_Ka, m_Kd, m_Ks, m_Roughness;
    Color m_SpecularColor;

};

shared_ptr<SurfaceShader> CreatePlastic(const ParameterList &params);

};

#endif //#ifndef __PLASTIC_H__
