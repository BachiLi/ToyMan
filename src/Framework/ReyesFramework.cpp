#include "ReyesFramework.h"
#include "../Primitive.h"
#include "../Hider.h"
#include "../Grid.h"
#include "../MicroPolygonGrid.h"
#include "../AABB.h"
#include "../Plane.h"
#include "../Display.h"

#include <iostream>

namespace ToyMan {

void ReyesFramework::WorldBegin() {	
    m_Hider->WorldBegin();
}

void ReyesFramework::Insert(shared_ptr<Primitive> primitive) {	
	internalInsert(primitive, 0);
}

void ReyesFramework::internalInsert(shared_ptr<Primitive> primitive, int depth) {
	if(depth > 20) { //eye split too much times, give up
		return;
    }

	Plane::SIDE side = m_ViewingFrustum.GetSide(primitive->BoundingBox());	
	if(side == Plane::INSIDE) {
		m_Hider->Insert(primitive);
	} else if(side == Plane::BOTH_SIDES) {
		vector<shared_ptr<Primitive> > splitted;
		primitive->Split(splitted, true, true);
		for(size_t i = 0; i < splitted.size(); i++)
			internalInsert(splitted[i], depth+1);		
	} 
}

void ReyesFramework::WorldEnd() {
    while(m_Hider->BucketBegin()) {
		shared_ptr<Primitive> p;
        while((p = m_Hider->PopPrimitive()).get() != NULL) {
            int uDim, vDim;
            bool uSplit, vSplit;
			if(p->Diceable(&uDim, &vDim, *m_Hider, &uSplit, &vSplit)) {                
				//grid is pretty memory consuming
				//so we must make sure that the resource it used is soon released
				MicroPolygonGrid grid(p, uDim, vDim); 
                p->Dice(&grid);

                if(grid.BackFaced()) {
                    continue;
                }

                if(m_Hider->NeedShade()) {
                    shared_ptr<SurfaceShader> surfaceShader = p->GetSurfaceShader();
	                surfaceShader->Shade(m_RNG, &grid);
                }
                m_Hider->Hide(&grid);
            } else if(p->Splittable()) {
				vector<shared_ptr<Primitive> > splitted;
                p->Split(splitted, uSplit, vSplit);
				for(size_t i = 0; i < splitted.size(); i++)
					Insert(splitted[i]);
            }
        }
        m_Hider->BucketEnd();
    }
    m_Hider->WorldEnd();
}

shared_ptr<Framework> CreateReyesFramework(
    shared_ptr<Hider> hider, const ViewingFrustum &vf, bool backFaceCulling) {
    shared_ptr<Framework> framework = 
        shared_ptr<Framework>(new ReyesFramework(hider, vf, backFaceCulling));
    return framework;
}

} //namespace ToyMan
