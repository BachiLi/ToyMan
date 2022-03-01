#ifndef __MICROPOLYGON_H__
#define __MICROPOLYGON_H__

#include "Point.h"
#include "Color.h"
#include "AABB.h"
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER

using namespace std::tr1;

namespace ToyMan {

struct ImageSample;

/**
 * @brief Represents a single vertex in the micropolygon
 * note that the coordinates of P used in micropolygon
 * is in a "hybrid" space, the x,y component is in raster space,
 * while z is in camera space, so that the z component is more
 * accurate
 */
class MicroPolygonVertex {
public:
    MicroPolygonVertex(const Point &p, const Point &pc, 
					   const Color &ci, const Color &oi, float _coc)
		: P(p), PClose(pc), Ci(ci), Oi(oi), coc(_coc) {}

    Point P, PClose;
    Color Ci, Oi;
	float coc; //circle of confusion
};

class MicroPolygon {
public:
    MicroPolygon(shared_ptr<MicroPolygonVertex> v0, shared_ptr<MicroPolygonVertex> v1,
                 shared_ptr<MicroPolygonVertex> v2, shared_ptr<MicroPolygonVertex> v3,
                 bool _interpolate);

    bool Sample(const ImageSample &sample, Color *color, Color *opacity, float *depth) const;
	
    shared_ptr<MicroPolygonVertex> v[4];
    AABB bBox;
    bool interpolate;
};

};

#endif //#ifndef __MICROPOLYGON_H__
