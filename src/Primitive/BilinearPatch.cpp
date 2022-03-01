#include "BilinearPatch.h"
#include "../AABB.h"
#include "../Transform.h"
#include "../Ray.h"

namespace ToyMan {

BilinearPatch::BilinearPatch(shared_ptr<Attributes> attr, 
    const Point &p0, const Point &p1, 
	const Point &p2, const Point &p3,
    float *st, bool pointsIsLocal)
	: Parametrics(attr) {    
    if(pointsIsLocal) {
	    m_Points[0] = (*GetObjectToCamera())(p0); 
	    m_Points[1] = (*GetObjectToCamera())(p1);
	    m_Points[2] = (*GetObjectToCamera())(p2); 
	    m_Points[3] = (*GetObjectToCamera())(p3);
    } else {
	    m_Points[0] = p0;         
	    m_Points[1] = p1;
	    m_Points[2] = p2; 
	    m_Points[3] = p3;
    }
    if(st != NULL) {      
        copy(st, st+8, m_ST);
    }
    m_UseST = st != NULL;
}

AABB BilinearPatch::BoundingBox() const {
	AABB box(F(m_UMin, m_VMin));
	box = Union(box, F(m_UMax, m_VMin));
	box = Union(box, F(m_UMin, m_VMax));
	box = Union(box, F(m_UMax, m_VMax));
	return box;
}

Point BilinearPatch::F(float u, float v) const {
	//TODO: override dice method to precompute left&right
	Point left  = m_Points[0]+v*(m_Points[2]-m_Points[0]);
	Point right = m_Points[1]+v*(m_Points[3]-m_Points[1]);	
	return left+u*(right-left);
}

Vector BilinearPatch::dFdu(float u, float v) const {
	Vector bottom = m_Points[1]-m_Points[0];
	Vector top    = m_Points[3]-m_Points[2];
	return bottom+v*(top-bottom);
}

Vector BilinearPatch::dFdv(float u, float v) const {
	Vector left  = m_Points[2]-m_Points[0];
	Vector right = m_Points[3]-m_Points[1];
	return left+u*(right-left);
}

float BilinearPatch::EvalS(float uGrid, float vGrid) const {
    if(!m_UseST) {
        return Primitive::EvalS(uGrid, vGrid);
    } else {
        float u = EvalU(uGrid, vGrid), v = EvalV(uGrid, vGrid);
        //bilinear interpolation
        float b1 = m_ST[0];
        float b2 = m_ST[2]-m_ST[0];
        float b3 = m_ST[4]-m_ST[0];
        float b4 = m_ST[0]-m_ST[2]-m_ST[4]+m_ST[6];
        return b1+b2*u+b3*v+b4*u*v;
    }
}

float BilinearPatch::EvalT(float uGrid, float vGrid) const {
    if(!m_UseST) {
        return Primitive::EvalS(uGrid, vGrid);
    } else {
        float u = EvalU(uGrid, vGrid), v = EvalV(uGrid, vGrid);
        //bilinear interpolation
        float b1 = m_ST[1];
        float b2 = m_ST[3]-m_ST[1];
        float b3 = m_ST[5]-m_ST[1];
        float b4 = m_ST[1]-m_ST[3]-m_ST[5]+m_ST[7];
        return b1+b2*u+b3*v+b4*u*v;
    }
}

bool RayTriangleIntersection(const Ray &ray, 
    const Point &p1, const Point &p2, const Point &p3) {
    Vector e1 = p2 - p1;
    Vector e2 = p3 - p1;
    Vector s1 = Cross(ray.d, e2);
    float divisor = Dot(s1, e1);
    
    if (divisor == 0.)
        return false;
    float invDivisor = 1.f / divisor;

    // Compute first barycentric coordinate
    Vector d = ray.o - p1;
    float b1 = Dot(d, s1) * invDivisor;
    if (b1 < 0. || b1 > 1.)
        return false;

    // Compute second barycentric coordinate
    Vector s2 = Cross(d, e1);
    float b2 = Dot(ray.d, s2) * invDivisor;
    if (b2 < 0. || b1 + b2 > 1.)
        return false;

    // Compute _t_ to intersection point
    float t = Dot(e2, s2) * invDivisor;
    if (t < ray.mint || t > ray.maxt)
        return false;

    return true;
}

bool BilinearPatch::IntersectP(const Ray &ray) const {
    //approximation: intersect with 2 triangles
    //triangle 1: P(0,1,2), triangle 2: P(3,2,1)
    return RayTriangleIntersection(ray, m_Points[0], m_Points[1], m_Points[2]) ||
           RayTriangleIntersection(ray, m_Points[3], m_Points[2], m_Points[1]);
}

} //namespace ToyMan