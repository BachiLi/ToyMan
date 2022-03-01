#ifndef __TRIANGLE_MESH_H__
#define __TRIANGLE_MESH_H__

#include "../Primitive.h"
#include "../Point.h"

namespace ToyMan {

class TriangleMesh : public Primitive {
public:
    TriangleMesh(shared_ptr<Attributes> attr,
        int ntris, int nverts, const int *vi, const Point *P, const Normal *N,
        const float *st);
    ~TriangleMesh();
    AABB BoundingBox() const;
    bool Splittable() const;
    void Split(vector<shared_ptr<Primitive> > &splitted, bool uSplit, bool vSplit) const;
    bool Diceable(int *uDim, int *vDim, const Hider &hider, bool *uSplit, bool *vSplit) const {
        return false;
    }

    friend class Triangle;

protected:
    //since we are not diceable, there is no need to evaluate point
    Point EvalPoint(float uGrid, float vGrid) const {
        return Point();
    }
private:
    int m_NTris, m_NVerts;
    int *m_VertexIndex;
    Point *m_Points;
    Normal *m_Normals;
    float *m_ST;
};

class Triangle : public Primitive {
public:
    Triangle(shared_ptr<Attributes> attr, 
        const TriangleMesh *m, int n);

    AABB BoundingBox() const;
    bool Splittable() const;
    void Split(vector<shared_ptr<Primitive> > &splitted, bool uSplit, bool vSplit) const;
    bool Diceable(int *uDim, int *vDim, const Hider &hider, bool *uSplit, bool *vSplit) const {
        return false;
    }

protected:
    //since we are not diceable, there is no need to evaluate point
    Point EvalPoint(float uGrid, float vGrid) const {
        return Point();
    }

private:
    const TriangleMesh *m_Mesh;
    int *m_Verts;
};

} //namespace ToyMan

#endif //#ifndef __TRIANGLE_MESH_H__