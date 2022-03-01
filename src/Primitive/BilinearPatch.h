#ifndef __BILINEAR_PATCH_H__
#define __BILINEAR_PATCH_H__

#include "../Parametrics.h"
#include "../Point.h"

namespace ToyMan {

class BilinearPatch : public Parametrics {
public:
	BilinearPatch(shared_ptr<Attributes> attr,
				  const Point &p0, const Point &p1, const Point &p2, const Point &p3,
                  float *st=NULL, bool pointsIsLocal=true);
	AABB BoundingBox() const;
    bool IntersectP(const Ray &ray) const;

protected:
	shared_ptr<Parametrics> Clone() const {
        shared_ptr<Parametrics> ptr = shared_ptr<Parametrics>(new BilinearPatch(*this));
        return ptr;
	}
    Point F(float u, float v) const;
	Vector dFdu(float u, float v) const;
	Vector dFdv(float u, float v) const;
    float EvalS(float uGrid, float vGrid) const;
    float EvalT(float uGrid, float vGrid) const;

private:
	Point m_Points[4];
    float m_ST[8];    
    bool m_UseST;
};

} //namespace ToyMan

#endif //#ifndef __BILINEAR_PATCH_H__