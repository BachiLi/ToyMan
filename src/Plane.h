#ifndef __PLANE_H__
#define __PLANE_H__

#include "Point.h"

namespace ToyMan {

class AABB;

/**
 *  A 3d plane of equation Ax+By+Cz+D=0
 */
class Plane
{
public:
	enum SIDE {
		INSIDE,
		OUTSIDE,
		BOTH_SIDES
	};

	Plane() {}
	///Create from 3 points
	Plane(const Point &p1, const Point &p2, const Point &p3);
	Plane(const Point &p, const Normal &n);

	//Inside means to be on the same side with normal
	SIDE GetSide(const Point &p) const;
	SIDE GetSide(const AABB &box) const;

private:
	float A,B,C,D;

};

};

#endif //#ifndef __PLANE_H__