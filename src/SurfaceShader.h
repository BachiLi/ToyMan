#ifndef __SURFACE_SHADER_H__
#define __SURFACE_SHADER_H__

#include "LightShader.h"
#include "RNG.h"
#include "Grid.h"
#include <vector>
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER

using namespace std;

namespace ToyMan {

class MicroPolygonGrid;

class SurfaceShader {
public:
	virtual void Shade(RNG &rng, MicroPolygonGrid *grid) = 0;
protected:
    class IlluminanceIterator {
    public:
        IlluminanceIterator(const LightList &lightList)
            : m_Index(0), m_LightList(lightList) {}

        bool Next(RNG &rng, const Point &P, const Normal &N, float angle, Vector *L, Color *Cl) {
            for(; m_Index < m_LightList.size(); m_Index++) {
                LightShaderSharedPtr shader = m_LightList.at(m_Index);
                if(!shader->IsAmbient()) {
                    if(shader->Shade(rng, P, N, angle, L, Cl)) {
                        m_Index++;
		                return true;
                    }
                }
            }
            return false;
        }
    private:
        size_t m_Index;
        const LightList &m_LightList;
    };


    Color Ambient(const LightList &lightList, RNG &rng) const;
    Color Diffuse(const LightList &lightList, RNG &rng, const Point &P, const Normal &N) const;
    Color Specular(const LightList &lightList, RNG &rng, const Point &P, const Normal &N, const Vector &V, float roughness) const;

    virtual Color SpecularBRDF(const Vector &L, const Normal &N, const Vector &V, float roughness) const;
};

};

#endif //#ifndef __SURFACE_SHADER_H__