#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "../Parametrics.h"

namespace ToyMan {

class Sphere : public Parametrics {
public:
    Sphere(shared_ptr<Attributes> attr, float radius, float zmin, float zmax, float thetaMax);
    AABB BoundingBox() const;
    bool IntersectP(const Ray &ray) const;

protected:
    shared_ptr<Parametrics> Clone() const {
        shared_ptr<Parametrics> ptr = shared_ptr<Parametrics>(new Sphere(*this));
        return ptr;
    };
    Point F(float u, float v) const;
	Vector dFdu(float u, float v) const;
	Vector dFdv(float u, float v) const;
private:
    float m_Radius;
    float m_AlphaMin, m_AlphaDelta;
    float m_ThetaMax;
};

};

#endif //#ifndef __SPHERE_H__
