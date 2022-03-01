#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__

#include "Ray.h"
#include "Color.h"
#include <vector>
#include <memory>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

class Primitive;
class Point;

class RayTracer {
public:
    void Insert(const shared_ptr<Primitive> &prim) {        
        m_Primitives.push_back(prim);
    }    
    Color Trace(const Ray &ray) const {return Color();}
    Color Transmission(const Point &p1, const Point &p2) const {return Color();}
    bool Visibility(const Point &p1, const Point &p2) const;
    size_t PrimNum() const {return m_Primitives.size();}
private:
    vector<shared_ptr<Primitive> > m_Primitives;
};

} //namespace ToyMan

#endif //#ifndef __RAY_TRACER_H__