#include "MicroPolygon.h"
#include "Point.h"
#include "ImageSampler.h"
#include "Util.h"
#include <algorithm>

using namespace std;

namespace ToyMan {

MicroPolygon::MicroPolygon(shared_ptr<MicroPolygonVertex> v0, shared_ptr<MicroPolygonVertex> v1,
                 shared_ptr<MicroPolygonVertex> v2, shared_ptr<MicroPolygonVertex> v3,
                 bool _interpolate)
{
    v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;
	float maxcoc = 0.f;
    for(int i = 0; i < 4; i++) {
		bBox = Union(bBox, v[i]->P);
		bBox = Union(bBox, v[i]->PClose);
		maxcoc = max(v[i]->coc, maxcoc);
	}
	bBox.pMin.x -= maxcoc;
	bBox.pMin.y -= maxcoc;
	bBox.pMax.x += maxcoc;
	bBox.pMax.y += maxcoc;
    interpolate = _interpolate;
} 

bool MicroPolygon::Sample(const ImageSample &sample, Color *color, Color *opacity, float *depth) const
{	
	float x = sample.imageX;
	float y = sample.imageY;
    if(x < bBox.pMin.x || x > bBox.pMax.x ||
       y < bBox.pMin.y || y > bBox.pMax.y)
       return false;
    
	float lensU = sample.lensU;
	float lensV = sample.lensV;

	Point movedP[4];
	for(int i = 0; i < 4; i++) {
		movedP[i] = Lerp(sample.time, v[i]->P, v[i]->PClose);
	}
    
	float x0 = movedP[0].x, x1 = movedP[1].x, x2 = movedP[3].x, x3 = movedP[2].x;
	float y0 = movedP[0].y, y1 = movedP[1].y, y2 = movedP[3].y, y3 = movedP[2].y;
	x0 += lensU*v[0]->coc; y0 += lensV*v[0]->coc;
	x1 += lensU*v[1]->coc; y1 += lensV*v[1]->coc;
	x2 += lensU*v[3]->coc; y2 += lensV*v[3]->coc;
	x3 += lensU*v[2]->coc; y3 += lensV*v[2]->coc;
	float x10 = x1-x0, x21 = x2-x1, x32 = x3-x2, x03 = x0-x3;
	float y10 = y1-y0, y21 = y2-y1, y32 = y3-y2, y03 = y0-y3;

	float cTop    = (y-y0)*(x10)-(x-x0)*(y10);
	float cRight  = (y-y1)*(x21)-(x-x1)*(y21);
	float cBottom = (y-y2)*(x32)-(x-x2)*(y32);
	float cLeft   = (y-y3)*(x03)-(x-x3)*(y03);
    
    const float EPSILON = 1e-6f;
	if((cTop <=  EPSILON && cRight <=  EPSILON && cBottom <=  EPSILON && cLeft <=  EPSILON) ||
       (cTop >= -EPSILON && cRight >= -EPSILON && cBottom >= -EPSILON && cLeft >= -EPSILON)) {
		float u = cLeft/(cLeft+cRight);
		float vv = cTop/(cTop+cBottom);
        if(interpolate) {
            *color = (v[0]->Ci*(1-u) + v[1]->Ci*u)*(1-vv) + (v[2]->Ci*(1-u) + v[3]->Ci*u)*vv;
            *opacity = (v[0]->Oi*(1-u) + v[1]->Oi*u)*(1-vv) + (v[2]->Oi*(1-u) + v[3]->Oi*u)*vv;;
        } else {
		    *color = v[0]->Ci;
		    *opacity = v[0]->Oi;
        }
		*depth = (movedP[0].z*(1-u) + movedP[1].z*u)*(1-vv) + (movedP[2].z*(1-u) + movedP[3].z*u)*vv;                
		return true;
	}
	return false;
}

};
