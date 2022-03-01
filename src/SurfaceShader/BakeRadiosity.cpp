#include "BakeRadiosity.h"
#include "RadiosityPoint.h"
#include "../MicroPolygonGrid.h"
#include "../ParameterList.h"
#include "../Util.h"

#include <fstream>

using namespace std;

namespace ToyMan {

void BakeRadiosity::Shade(RNG &rng, MicroPolygonGrid *grid)
{
    //setup grid symbols
    shared_ptr<PointGrid>  P = grid->FindPointGrid("P");
	shared_ptr<NormalGrid> N = grid->FindNormalGrid("N");
    shared_ptr<ColorGrid> Ci = grid->FindColorGrid("Ci");	
	shared_ptr<ColorGrid> Cs = grid->FindColorGrid("Cs");
    shared_ptr<VectorGrid> I = grid->FindVectorGrid("I");    
    shared_ptr<VectorGrid> dPdu = grid->FindVectorGrid("dPdu");    
    shared_ptr<VectorGrid> dPdv = grid->FindVectorGrid("dPdv");    
    
    //execute
    const int uDim = grid->GetUDimenstion();
	const int vDim = grid->GetVDimenstion();

    for(int i = 0; i < uDim*vDim; i++) {
        Point  p = (*P)(i);
        Normal n = Normalize((*N)(i));
        (*Ci)(i) = (*Cs)(i)*(m_Ka*Ambient(*(grid->GetLightSources()), rng)+
                             m_Kd*Diffuse(*(grid->GetLightSources()), rng, p, n));
    }
    
    vector<RadiosityPoint> radPoints;

    for(int u = 0; u < uDim-1; u++)
        for(int v = 0; v < vDim-1; v++) {
            Point p = 0.25f*((*P)(u,v)+(*P)(u+1,v)+(*P)(u,v+1)+(*P)(u+1,v+1));
            Normal n = Normalize((*N)(u,v)+(*N)(u+1,v)+(*N)(u,v+1)+(*N)(u+1,v+1));
            Color ci = 0.25f*((*Ci)(u,v)+(*Ci)(u+1,v)+(*Ci)(u,v+1)+(*Ci)(u+1,v+1));
            Vector dpdu = 0.25f*((*dPdu)(u,v)+(*dPdu)(u+1,v)+(*dPdu)(u,v+1)+(*dPdu)(u+1,v+1));
            Vector dpdv = 0.25f*((*dPdv)(u,v)+(*dPdv)(u+1,v)+(*dPdv)(u,v+1)+(*dPdv)(u+1,v+1));
            float area = Cross(dpdu,dpdv).Length();
            radPoints.push_back(
                RadiosityPoint(p, n, ci, area));
        }  


    //append to file
    //fstream fs(m_Filename.c_str(), fstream::app);
    for(size_t i = 0; i < radPoints.size(); i++)
        m_FStream << radPoints[i] << endl;
    //fs.close();
}

shared_ptr<SurfaceShader> CreateBakeRadiosity(const ParameterList &params) {
    string filename = params.FindString("bakefile", "bake.txt");
    remove(filename.c_str());
    float Ka = params.FindFloat("Ka", 1.f);
    float Kd = params.FindFloat("Kd", 1.f);
    shared_ptr<SurfaceShader> surface
        = shared_ptr<SurfaceShader>(new BakeRadiosity(filename, Ka, Kd));

    return surface;
}

} //namespace ToyMan 
