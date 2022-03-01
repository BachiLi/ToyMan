#include "Matte.h"
#include "../MicroPolygonGrid.h"
#include "../Util.h"
#include "../ParameterList.h"

#include <cmath>

using namespace std;

namespace ToyMan {

void Matte::Shade(RNG &rng, MicroPolygonGrid *grid)
{
	//a simple matte shader

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
        (*Ci)(i) = (*Cs)(i)*(m_Ka*Ambient(*(grid->GetLightSources()), rng)+
                             m_Kd*Diffuse(*(grid->GetLightSources()), rng, p, n));
    }
}

shared_ptr<SurfaceShader> CreateMatte(const ParameterList &params) {
    float Ka = params.FindFloat("Ka", 1.f);
    float Kd = params.FindFloat("Kd", 1.f);
    shared_ptr<SurfaceShader> surface
        = shared_ptr<SurfaceShader>(new Matte(Ka, Kd));

    return surface;
}

};
