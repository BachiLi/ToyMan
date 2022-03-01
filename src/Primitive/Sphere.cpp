#include "Sphere.h"
#include "../Util.h"
#include "../AABB.h"
#include "../Transform.h"
#include "../Ray.h"
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

namespace ToyMan {

inline static AABB Bounds2DSweepL(float rmin, float rmax, float tmin, float tmax)
{
    AABB box(Polar(rmin,tmin));
	box = Union(box, Polar(rmax,tmin));
	box = Union(box, Polar(rmin,tmax));
	box = Union(box, Polar(rmax,tmax));

	if(tmin < float(M_PI_2) && tmax > float(M_PI_2))
        box = Union(box, Polar(rmax, float(M_PI_2)));

	if(tmin < float(M_PI) && tmax > float(M_PI))
        box = Union(box, Polar(rmax, float(M_PI)));

	if(tmin < (float(M_PI)+float(M_PI_2)) && tmax > (float(M_PI)+float(M_PI_2)) )
		box = Union(box, Polar(rmax, (float(M_PI)+float(M_PI_2))));
    return box;
}


Sphere::Sphere(shared_ptr<Attributes> attr, float radius, float zmin, float zmax, float thetaMax)
        : Parametrics(attr), m_Radius(radius), m_ThetaMax(thetaMax) 
{
	m_AlphaMin = asin(zmin/m_Radius); //precalculated value for alpha calculation
	m_AlphaDelta = asin(zmax/m_Radius)-m_AlphaMin;
}

AABB Sphere::BoundingBox() const
{
	float avMin = m_AlphaMin+m_VMin*m_AlphaDelta;
	float avMax = m_AlphaMin+m_VMax*m_AlphaDelta;
	float rvMin = cos(avMin)*m_Radius;
	float rvMax = cos(avMax)*m_Radius;
	float rMin = min(rvMin, rvMax);
	float rMax = (avMin < 0.f && avMax > 0.f) ? m_Radius : max(rvMin, rvMax);
	float tMin = m_UMin*m_ThetaMax;
	float tMax = m_UMax*m_ThetaMax;
	AABB box = Bounds2DSweepL(rMin, rMax, tMin, tMax);
	box.pMin.z = sin(avMin)*m_Radius;
	box.pMax.z = sin(avMax)*m_Radius;
    box = (*GetObjectToCamera())(box);
    return box;
}

Point Sphere::F(float u, float v) const
{
	float alpha = m_AlphaMin+v*m_AlphaDelta;
	Point p = Point(cos(u*m_ThetaMax)*cos(alpha)*m_Radius,
                    sin(u*m_ThetaMax)*cos(alpha)*m_Radius,
                                      sin(alpha)*m_Radius);
	Point cP = (*GetObjectToCamera())(p);
	return cP;
}

Vector Sphere::dFdu(float u, float v) const
{
    float alpha = m_AlphaMin+v*m_AlphaDelta;
	float tu = u*m_ThetaMax;
	Vector vec = Vector(m_ThetaMax*(-sin(tu)*cos(alpha)*m_Radius),
    		            m_ThetaMax*( cos(tu)*cos(alpha)*m_Radius),
				        0);
	Vector cV = (*GetObjectToCamera())(vec);    
	return cV;
}

Vector Sphere::dFdv(float u, float v) const
{
    float alpha = m_AlphaMin+v*m_AlphaDelta;
	float tu = u*m_ThetaMax;
	Vector vec = Vector(cos(tu)*m_AlphaDelta*(-sin(alpha))*m_Radius,
		                sin(tu)*m_AlphaDelta*(-sin(alpha))*m_Radius,
				                m_AlphaDelta*( cos(alpha))*m_Radius);
	Vector cV = (*GetObjectToCamera())(vec);
	return cV;
}

bool Sphere::IntersectP(const Ray &ray) const {    
    Vector op = (*GetObjectToCamera())(Point(0.f,0.f,0.f)) - ray.o;
    float b = Dot(op, Normalize(ray.d));
    float det = b*b-Dot(op,op)+m_Radius*m_Radius;
    if(det < 0.f)
        return false;
    det = sqrt(det);
    float t1 = b-det;        
    if(t1 > ray.mint && t1 < ray.maxt) return true;    
    float t2 = b+det;    
    if(t2 > ray.mint && t2 < ray.maxt) return true;
    return false;
}

};
