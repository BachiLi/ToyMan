#include "LightShader.h"
#include "MicroPolygonGrid.h"

namespace ToyMan {

bool LightShader::Illuminate(const Point &Ps, const Normal &Ns, float As,
        const Point &position, const Vector &axis, float angle,
        Vector *L) {
    Vector l = Ps - position;
    Vector nl = Normalize(l);
    if(Dot(nl, axis) < cos(angle))
        return false;
    if(Dot(-nl, Ns) < cos(As))
        return false;
    *L = l;
    return true;
}

bool LightShader::Solar(const Point &Ps, const Normal &Ns, float As, 
        const Vector &axis, float angle, Vector *L) {
    if(Dot(Ns, -axis) < cos(As+angle))
        return false;
    *L = axis;
    return true;
}

};