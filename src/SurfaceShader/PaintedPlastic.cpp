#include "PaintedPlastic.h"
#include "../RNG.h"
#include "../MicroPolygonGrid.h"
#include "../Util.h"
#include "../ParameterList.h"
#include <FreeImagePlus.h>

namespace ToyMan {

PaintedPlastic::PaintedPlastic(float Ka, float Kd, float Ks, 
    float roughness, const Color &specularColor,
    const string &texturename)
    : m_Ka(Ka), m_Kd(Kd), m_Ks(Ks), 
      m_Roughness(roughness), m_SpecularColor(specularColor) {    
    m_Texture = MipMap<Color>::GetMipMap(texturename);
}

void PaintedPlastic::Shade(RNG &rng, MicroPolygonGrid *grid)
{
    //setup grid symbols
    const int uDim = grid->GetUDimenstion();
	const int vDim = grid->GetVDimenstion();
    shared_ptr<PointGrid>  P = grid->FindPointGrid("P");
	shared_ptr<NormalGrid> N = grid->FindNormalGrid("N");
    shared_ptr<ColorGrid> Ci = grid->FindColorGrid("Ci");	
	shared_ptr<ColorGrid> Cs = grid->FindColorGrid("Cs");
    shared_ptr<VectorGrid> I = grid->FindVectorGrid("I");
    shared_ptr<FloatGrid>  s = grid->FindFloatGrid("s");
    shared_ptr<FloatGrid>  t = grid->FindFloatGrid("t");    

    FloatGrid dsdu(uDim,vDim), dsdv(uDim,vDim), dtdu(uDim,vDim), dtdv(uDim,vDim);

    for(int u = 0; u < uDim; u++)
        for(int v = 0; v < vDim; v++) {
            int up = Clamp(u+1,0,uDim-1), vp = Clamp(v+1,0,vDim-1);
            int um = Clamp(u-1,0,uDim-1), vm = Clamp(v-1,0,vDim-1);
            dsdu(u,v) = ((*s)(up,v) - (*s)(um,v))/(float)(up-um);
            dsdv(u,v) = ((*s)(u,vp) - (*s)(u,vm))/(float)(vp-vm);
            dtdu(u,v) = ((*t)(up,v) - (*t)(um,v))/(float)(up-um);
            dtdv(u,v) = ((*t)(u,vp) - (*t)(u,vm))/(float)(vp-vm);
        }    

    //execute
    for(int i = 0; i < uDim*vDim; i++) {
        Point  pi = (*P)(i);
        Normal ni = Faceforward(Normalize((*N)(i)), -(*I)(i));
        Vector vi = -Normalize((*I)(i));
        Color tex=m_Texture->Lookup((*s)(i), (*t)(i), 2.f*max(max(abs(dsdu(i)), abs(dsdv(i))),
                                                              max(abs(dtdu(i)), abs(dtdv(i)))));
        (*Ci)(i) = tex*(m_Ka*Ambient((*grid->GetLightSources()), rng)+
                        m_Kd*Diffuse((*grid->GetLightSources()), rng, pi, ni)) +
                        m_SpecularColor*m_Ks*Specular((*grid->GetLightSources()), rng, pi, ni, vi, m_Roughness);
    }
}

shared_ptr<SurfaceShader> CreatePaintedPlastic(const ParameterList &params) {
    float Ka = params.FindFloat("Ka", 1.f);
    float Kd = params.FindFloat("Kd", 0.5f);
    float Ks = params.FindFloat("Ks", 0.5f);
    float roughness = params.FindFloat("roughness", 0.1f);
    Color specularColor = params.FindColor("specularcolor", Color(1.f,1.f,1.f));
    string texturename = params.FindString("texturename", "");

    shared_ptr<SurfaceShader> surface
        = shared_ptr<SurfaceShader>(
        new PaintedPlastic(Ka, Kd, Ks, roughness, specularColor, texturename));
    return surface;
}

};