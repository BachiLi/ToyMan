#include "Plastic.h"
#include "../RNG.h"
#include "../MicroPolygonGrid.h"
#include "../ParameterList.h"

namespace ToyMan {

void Plastic::Shade(RNG &rng, MicroPolygonGrid *grid)
{
    //setup grid symbols
    shared_ptr<PointGrid>  P = grid->FindPointGrid("P");
	shared_ptr<NormalGrid> N = grid->FindNormalGrid("N");
    shared_ptr<ColorGrid> Ci = grid->FindColorGrid("Ci");	
	shared_ptr<ColorGrid> Cs = grid->FindColorGrid("Cs");
    shared_ptr<VectorGrid> I = grid->FindVectorGrid("I");
    
    //execute
    const int uDim = grid->GetUDimenstion();
	const int vDim = grid->GetVDimenstion();
    for(int i = 0; i < uDim*vDim; i++) {
        Point  p = (*P)(i);
        Normal n = Faceforward(Normalize((*N)(i)), -(*I)(i));
        Vector v = -Normalize((*I)(i));
        (*Ci)(i) = (*Cs)(i)*(m_Ka*Ambient((*grid->GetLightSources()), rng)+
                             m_Kd*Diffuse((*grid->GetLightSources()), rng, p, n)) +
                             m_SpecularColor*m_Ks*Specular((*grid->GetLightSources()), rng, p, n, v, m_Roughness);
    }
}

shared_ptr<SurfaceShader> CreatePlastic(const ParameterList &params) {
    float Ka = params.FindFloat("Ka", 1.f);
    float Kd = params.FindFloat("Kd", 0.5f);
    float Ks = params.FindFloat("Ks", 0.5f);
    float roughness = params.FindFloat("roughness", 0.1f);
    Color specularColor = params.FindColor("specularcolor", Color(1.f,1.f,1.f));
    
    shared_ptr<SurfaceShader> surface
        = shared_ptr<SurfaceShader>(
        new Plastic(Ka, Kd, Ks, roughness, specularColor));
    return surface;
}

};