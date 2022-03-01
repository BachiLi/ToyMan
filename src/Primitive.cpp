#include "Primitive.h"
#include "MicroPolygonGrid.h"
#include "Point.h"
#include "Vector.h"
#include "Normal.h"
#include "Transform.h"
#include "SurfaceShader/Matte.h"
#include <iostream>
#include <cstdlib>

namespace ToyMan {

Attributes::Attributes()
{
    //initialize default values
    objectToCamera       = shared_ptr<Transform>(new Transform());
    movementTransform    = shared_ptr<Transform>(new Transform());
    color                = Color(1.f, 1.f, 1.f);
    opacity              = Color(1.f, 1.f, 1.f);
    lightSources         = shared_ptr<LightList>(new LightList());
    shadingInterpolation = CONSTANT;
    surfaceShader        = shared_ptr<SurfaceShader>(new Matte());
    shadingRate          = 1.f;
}

void Primitive::Dice(shared_ptr<PointGrid> pointGrid) const
{
	int uDim = pointGrid->GetUSize();
	int vDim = pointGrid->GetVSize();
    float uStep = 1.f/float(uDim-1);
    float vStep = 1.f/float(vDim-1);
    float vGrid = 0.f;
    for(int v = 0; v < vDim; v++) {		
		float uGrid = 0.f;
        for(int u = 0; u < uDim; u++) {
            Point p = EvalPoint(uGrid,vGrid);
            (*pointGrid)(u,v) = p;
			uGrid += uStep;
        }
		vGrid += vStep;
    }
}

void Primitive::Dice(MicroPolygonGrid *grid) const
{
	int uDim = grid->GetUDimenstion();
	int vDim = grid->GetVDimenstion();
	shared_ptr<PointGrid> P = grid->FindPointGrid("P");
	shared_ptr<VectorGrid> dPdu = grid->FindVectorGrid("dPdu");
	shared_ptr<VectorGrid> dPdv = grid->FindVectorGrid("dPdv");
	shared_ptr<NormalGrid> N = grid->FindNormalGrid("N");
    shared_ptr<ColorGrid> Cs = grid->FindColorGrid("Cs");
    shared_ptr<VectorGrid> I = grid->FindVectorGrid("I");
    shared_ptr<FloatGrid> u = grid->FindFloatGrid("u");
    shared_ptr<FloatGrid> v = grid->FindFloatGrid("v");
    shared_ptr<FloatGrid> s = grid->FindFloatGrid("s");
    shared_ptr<FloatGrid> t = grid->FindFloatGrid("t");
    float uStep = 1.f/float(uDim-1);
    float vStep = 1.f/float(vDim-1);
    float vGrid = 0.f;
    for(int vIdx = 0; vIdx < vDim; vIdx++) {		
		float uGrid = 0.f;
        for(int uIdx = 0; uIdx < uDim; uIdx++) {            
            Point p = EvalPoint(uGrid,vGrid);
            (*P)(uIdx,vIdx) = p;
			Vector dpdu = Normalize(EvaldPdu(uGrid, vGrid));
			(*dPdu)(uIdx,vIdx) = dpdu;
			Vector dpdv = Normalize(EvaldPdv(uGrid, vGrid));
			(*dPdv)(uIdx,vIdx) = dpdv;
			(*N)(uIdx,vIdx) = (Normal)Cross(dpdu, dpdv);
			(*Cs)(uIdx,vIdx) = EvalCs(uGrid, vGrid);
			(*I)(uIdx,vIdx) = p - Point(0.f,0.f,0.f);	
            (*u)(uIdx,vIdx) = EvalU(uGrid, vGrid);
            (*v)(uIdx,vIdx) = EvalV(uGrid, vGrid);
            (*s)(uIdx,vIdx) = EvalS(uGrid, vGrid);
            (*t)(uIdx,vIdx) = EvalT(uGrid, vGrid);
            uGrid += uStep;            
        }
		vGrid += vStep;
    }

    vGrid = 0.f;
    for(int vIdx = 0; vIdx < vDim; vIdx++) {
        float uGrid = 0.f;
        for(int uIdx = 0; uIdx < uDim; uIdx++) {
            int up = Clamp(uIdx+1,0,uDim-1), vp = Clamp(vIdx+1,0,vDim-1);
            int um = Clamp(uIdx-1,0,uDim-1), vm = Clamp(vIdx-1,0,vDim-1);
            float du = Distance((*P)(up,vIdx), (*P)(um,vIdx))/float(up-um);
            float dv = Distance((*P)(uIdx,vp), (*P)(uIdx,vm))/float(vp-vm);
            (*dPdu)(uIdx,vIdx) = (*dPdu)(uIdx,vIdx)*du;
            (*dPdv)(uIdx,vIdx) = (*dPdv)(uIdx,vIdx)*dv;
            uGrid += uStep;
        }
        vGrid += vStep;
    }
}


Vector Primitive::EvaldPdu(float uGrid, float vGrid) const
{
    //central difference
    float h = 0.001f;
    return (EvalPoint(uGrid+h, vGrid) - EvalPoint(uGrid-h, vGrid))/(2.f*h);
}

Vector Primitive::EvaldPdv(float uGrid, float vGrid) const
{
    //central difference
    float h = 0.001f;
    return (EvalPoint(uGrid, vGrid+h) - EvalPoint(uGrid, vGrid-h))/(2.f*h);
}

Normal Primitive::EvalNormal(float uGrid, float vGrid) const
{
	Normal n = Normalize((Normal)Cross(EvaldPdu(uGrid,vGrid), EvaldPdv(uGrid, vGrid)));
	return n;
}

};
