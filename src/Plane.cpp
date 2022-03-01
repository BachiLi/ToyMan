#include "Plane.h"
#include "Vector.h"
#include "Normal.h"
#include "AABB.h"

namespace ToyMan {

Plane::Plane(const Point &p1, const Point &p2, const Point &p3)
{
	Vector v12 = p2-p1, v13 = p3-p1;
	Normal n = (Normal)Cross(v12, v13);
	A=n.x; B=n.y; C=n.z;
	D = -(A*p1.x+B*p1.y+C*p1.z);
}

Plane::Plane(const Point &p, const Normal &n)
{
	A=n.x; B=n.y; C=n.z;
	D = -(A*p.x+B*p.y+C*p.z);
}

Plane::SIDE Plane::GetSide(const Point &p) const
{
	return A*p.x+B*p.y+C*p.z+D >= 0.f ? INSIDE : OUTSIDE;
}

Plane::SIDE Plane::GetSide(const AABB &box) const
{
	Point pVertex = box.pMin;
	if(A >= 0.f)
		pVertex.x = box.pMax.x;
	if(B >= 0.f)
		pVertex.y = box.pMax.y;
	if(C >= 0.f)
		pVertex.z = box.pMax.z;		

	if(GetSide(pVertex) == OUTSIDE) 
		return OUTSIDE;

	Point nVertex = box.pMax;
	if(A >= 0.f)
		nVertex.x = box.pMin.x;
	if(B >= 0.f)
		nVertex.y = box.pMin.y;
	if(C >= 0.f)
		nVertex.z = box.pMin.z;

	if(GetSide(nVertex) == OUTSIDE)
		return BOTH_SIDES;

	return INSIDE;
}

};