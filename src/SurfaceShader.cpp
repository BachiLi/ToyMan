#include "SurfaceShader.h"

namespace ToyMan {

Color SurfaceShader::Ambient(const LightList &lightList, RNG &rng) const
{
    Color result(0.f,0.f,0.f);
    for(size_t light = 0; light < lightList.size(); light++) {
        LightShaderSharedPtr shader = lightList.at(light);
        if(shader->IsAmbient()) {
            Vector L;
            Color Cl;
            if(shader->Shade(rng, Point(), Normal(), 0.f, &L, &Cl)) {
                result += Cl;
            }
        }
    }
    return result;
}

Color SurfaceShader::Diffuse(const LightList &lightList, RNG &rng, const Point &P, const Normal &N) const
{
    Color result(0.f,0.f,0.f);
    IlluminanceIterator iterator(lightList);
    Vector L;
    Color Cl;
    while(iterator.Next(rng, P, N, (float)M_PI_2, &L, &Cl)) {
        result += Cl * Dot(N, Normalize(L));
    }

    return result;
}

Color SurfaceShader::Specular(const LightList &lightList, RNG &rng, const Point &P, const Normal &N, const Vector &V, float roughness) const
{
    Color result(0.f,0.f,0.f);
    IlluminanceIterator iterator(lightList);
    Vector L;
    Color Cl;
    while(iterator.Next(rng, P, N, (float)M_PI_2, &L, &Cl)) {
        result += Cl * SpecularBRDF(Normalize(L), N, V, roughness);
    }
    return result;
}

Color SurfaceShader::SpecularBRDF(const Vector &L, const Normal &N, const Vector &V, float roughness) const
{
    Vector H = Normalize(L+V);
    float reflection = pow(max(0.f, Dot(N,H)), 1.f/roughness);
    return Color(reflection, reflection, reflection);
}

};