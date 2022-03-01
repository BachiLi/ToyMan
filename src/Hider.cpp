#include "Hider.h"
#include "AABB.h"
#include "Transform.h"
#include "Display.h"

#include <cmath>

using namespace std;

namespace ToyMan {

float Hider::RasterEstimate(const Point& p1, const Point& p2) const
{
    if(m_RasterOrient) {
        Point tP1 = (*m_CameraToRaster)(p1);
        Point tP2 = (*m_CameraToRaster)(p2);
        return sqrt((tP1.x-tP2.x)*(tP1.x-tP2.x)+(tP1.y-tP2.y)*(tP1.y-tP2.y));
    } else {                   
        Point tP1 = (*m_CameraToRaster)(p1);
        Point tP2 = (*m_CameraToRaster)(p2);
        float maxDim = (float)max(
            m_Display->GetXResolution(),
            m_Display->GetYResolution());
        tP1.z *= maxDim;
        tP2.z *= maxDim;
        return Distance(tP1,tP2);
    }
}

};
