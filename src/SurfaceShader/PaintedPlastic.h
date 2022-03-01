#ifndef __PAINTED_PLASTIC_H__
#define __PAINTED_PLASTIC_H__

#include "../SurfaceShader.h"
#include "../Color.h"
#include "../MipMap.h"
#include <memory>

using namespace std::tr1;

namespace ToyMan {

class ParameterList;

class PaintedPlastic : public SurfaceShader {
public:
    PaintedPlastic(float Ka=1.f, float Kd=0.5f, float Ks=0.5f,
        float roughness=0.1f, const Color &specularColor = Color(1.f,1.f,1.f),
        const string &texturename="");

    virtual void Shade(RNG &rng, MicroPolygonGrid *grid);

private:
    float m_Ka, m_Kd, m_Ks, m_Roughness;
    Color m_SpecularColor;
    shared_ptr<MipMap<Color> > m_Texture;
};

shared_ptr<SurfaceShader> CreatePaintedPlastic(const ParameterList &params);

};

#endif //#ifndef __PAINTED_PLASTIC_H__
