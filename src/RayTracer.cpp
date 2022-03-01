#include "RayTracer.h"
#include "Point.h"
#include "Vector.h"
#include "Primitive.h"

namespace ToyMan {

bool RayTracer::Visibility(const Point &p1, const Point &p2) const {
    Vector p1p2 = p2-p1;
    float dis = p1p2.Length();    
    if(dis < 0.001f)
        return true;
    Ray ray(p1, Normalize(p1p2), 0.001f, dis);
    for(size_t i = 0; i < m_Primitives.size(); i++) 
        if(m_Primitives[i]->IntersectP(ray))
            return false;
    
    return true;
}

} //namespace ToyMan