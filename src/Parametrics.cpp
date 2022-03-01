#include "Parametrics.h"
#include "MicroPolygonGrid.h"
#include "Point.h"
#include "Vector.h"
#include "Grid.h"
#include "Framework.h"
#include "Hider.h"
#include "Transform.h"
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER
#include <algorithm>
#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

bool Parametrics::Splittable() const
{
    //special handle?
    //return m_UMin < m_UMax && m_VMin < m_VMax;
    return true;
}

void Parametrics::Split(vector<shared_ptr<Primitive> > &splitted, bool uSplit, bool vSplit) const
{    
	if(uSplit && vSplit) {
		//choose seemingly longer side
		if(m_UMax-m_UMin > m_VMax-m_VMin) vSplit = false;
		else uSplit = false;
	}

    if(uSplit) {		
        shared_ptr<Parametrics> left = Clone();
        shared_ptr<Parametrics> right = Clone();
        float uMid = (m_UMin+m_UMax)*0.5f;
        left->m_UMax = uMid;
        right->m_UMin = uMid;
		splitted.push_back(left);
        splitted.push_back(right);
    } else if(vSplit) { 
        shared_ptr<Parametrics> down = Clone();
        shared_ptr<Parametrics> up = Clone();
        float vMid = (m_VMin+m_VMax)*0.5f;
        down->m_VMax = vMid;
        up->m_VMin = vMid;
        splitted.push_back(down);
        splitted.push_back(up);
    }
}


bool Parametrics::Diceable(int* uDim, int* vDim, const Hider& hider, bool* uSplit, bool* vSplit) const
{
    const int TEST_DICE_SIZE = 8;
    shared_ptr<PointGrid> pointGrid = shared_ptr<PointGrid>(new PointGrid(TEST_DICE_SIZE, TEST_DICE_SIZE));
    Dice(pointGrid);
    PointGrid &grid = *pointGrid;
    float maxUDist = 0.f;
    float maxVDist = 0.f;
    for(int u = 0; u < TEST_DICE_SIZE-1; u++)
        for(int v = 0; v < TEST_DICE_SIZE-1; v++) {
            float uDist = hider.RasterEstimate(grid(u,v), grid(u+1,v));
            float vDist = hider.RasterEstimate(grid(u,v), grid(u,v+1));
            maxUDist = max(uDist, maxUDist);
            maxVDist = max(vDist, maxVDist);
        }

    float tmpUDim = std::max(ceil((TEST_DICE_SIZE*maxUDist)/hider.GetShadingRate(*this)), 2.f);
    float tmpVDim = std::max(ceil((TEST_DICE_SIZE*maxVDist)/hider.GetShadingRate(*this)), 2.f);

    float maxGridSize = (float)hider.MaxGridSize();
    //prevent overflow
    if(tmpUDim <= maxGridSize && tmpVDim <= maxGridSize &&
        tmpUDim*tmpVDim <= maxGridSize) {
        *uDim = (int)tmpUDim;
        *vDim = (int)tmpVDim;
        return true;
    }

    //grid too large, need to split
    if(tmpUDim > tmpVDim) {
        *uSplit = true;
        *vSplit = false;
    } else {
        *uSplit = false;
        *vSplit = true;
    }
    return false;
}

Vector Parametrics::dFdu(float u, float v) const
{
	//central difference
	float h = 0.001f;
	return (F(u+h,v) - F(u-h,v))/(2.f*h);
}

Vector Parametrics::dFdv(float u, float v) const
{
	//central difference
	float h = 0.001f;
	return (F(u,v+h) - F(u,v-h))/(2.f*h);
}

Point Parametrics::EvalPoint(float uGrid, float vGrid) const
{
    float u = m_UMin+(m_UMax-m_UMin)*uGrid;
    float v = m_VMin+(m_VMax-m_VMin)*vGrid;

    return F(u,v);
}

Vector Parametrics::EvaldPdu(float uGrid, float vGrid) const
{
    float u = m_UMin+(m_UMax-m_UMin)*uGrid;
    float v = m_VMin+(m_VMax-m_VMin)*vGrid;

	return dFdu(u,v);
}

Vector Parametrics::EvaldPdv(float uGrid, float vGrid) const
{
    float u = m_UMin+(m_UMax-m_UMin)*uGrid;
    float v = m_VMin+(m_VMax-m_VMin)*vGrid;

	return dFdv(u,v);
}

float Parametrics::EvalU(float uGrid, float vGrid) const
{
    return m_UMin+(m_UMax-m_UMin)*uGrid;
}

float Parametrics::EvalV(float uGrid, float vGrid) const
{
    return m_VMin+(m_VMax-m_VMin)*vGrid;
}

};
