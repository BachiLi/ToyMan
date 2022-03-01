#include "TriangleMesh.h"
#include "../Transform.h"
#include "../AABB.h"
#include "BilinearPatch.h"

namespace ToyMan {

TriangleMesh::TriangleMesh(shared_ptr<Attributes> attr,
        int ntris, int nverts, const int *vi, const Point *P, const Normal *N,
        const float *st) : Primitive(attr) {
    m_NTris = ntris;
    m_NVerts = nverts;
    m_VertexIndex = new int[3*m_NTris];
    std::copy(vi, vi+3*m_NTris, m_VertexIndex);    

    m_Points = new Point[m_NVerts];
    for(int i = 0; i < m_NVerts; i++) {        
        m_Points[i] = (*GetObjectToCamera())(P[i]);  
    }

    if(N) {
        m_Normals = new Normal[m_NVerts];
        std::copy(N, N+m_NVerts, m_Normals);
    } else {
        m_Normals = NULL;
    }

    if(st) {
        m_ST = new float[2*m_NVerts];
        std::copy(st, st+2*m_NVerts, m_ST);
    } else {
        m_ST = NULL;
    }
}

TriangleMesh::~TriangleMesh() {    
    delete[] m_VertexIndex;
    delete[] m_Points;
    if(m_Normals)
        delete[] m_Normals;
    if(m_ST)
        delete[] m_ST;
}

AABB TriangleMesh::BoundingBox() const {
    AABB box;
    for(int i = 0; i < m_NVerts; i++) {
        box = Union(box, m_Points[i]);
    }
    return box;
}

bool TriangleMesh::Splittable() const {
    return true;
}

void TriangleMesh::Split(vector<shared_ptr<Primitive> > &splitted, bool uSplit, bool vSplit) const {        
    //std::cerr << "use:" << ptr.use_count() << std::endl;
    for(int i = 0; i < m_NTris; i++) {        
        shared_ptr<Primitive> prim = shared_ptr<Primitive>(
            new Triangle(m_Attributes, this, i));
        splitted.push_back(prim);
    }
}

Triangle::Triangle(shared_ptr<Attributes> attr, const TriangleMesh *m, int n)
    : Primitive(attr) {
    m_Mesh = m;
    m_Verts = &m_Mesh->m_VertexIndex[3*n];    
}

AABB Triangle::BoundingBox() const {
    const Point &p0 = m_Mesh->m_Points[m_Verts[0]];
    const Point &p1 = m_Mesh->m_Points[m_Verts[1]];
    const Point &p2 = m_Mesh->m_Points[m_Verts[2]];
    AABB box;    
    box = Union(box, p0);
    box = Union(box, p1);
    box = Union(box, p2);
    return box;
}

bool Triangle::Splittable() const {
    return true;
}

void Triangle::Split(vector<shared_ptr<Primitive> > &splitted, bool uSplit, bool vSplit) const {        
    const Point &p0 = m_Mesh->m_Points[m_Verts[0]];
    const Point &p1 = m_Mesh->m_Points[m_Verts[1]];
    const Point &p2 = m_Mesh->m_Points[m_Verts[2]];
    Point center = (p0+p1+p2)/3.f;
    Point e0 = (p1+p2)/2.f;
    Point e1 = (p0+p2)/2.f;
    Point e2 = (p0+p1)/2.f;    
    if(m_Mesh->m_ST != NULL) {
        float vs[3], vt[3];        
        for(int i = 0; i < 3; i++) {
            vs[i] = m_Mesh->m_ST[2*m_Verts[i]];
            vt[i] = m_Mesh->m_ST[2*m_Verts[i]+1];
        }        

        float st[8];
        //st = new float[8];
        //center st
        st[0] = (vs[0]+vs[1]+vs[2])/3.f;
        st[1] = (vt[0]+vt[1]+vt[2])/3.f;
        //e2 st
        st[2] = (vs[0]+vs[1])/2.f;
        st[3] = (vt[0]+vt[1])/2.f;
        //e1 st
        st[4] = (vs[0]+vs[2])/2.f;
        st[5] = (vt[0]+vt[2])/2.f;
        //p0 st
        st[6] = vs[0];
        st[7] = vt[0];
        splitted.push_back(
            shared_ptr<Primitive>(new BilinearPatch(m_Attributes, 
            center, e2, e1, p0, st, false)));

        //st = new float[8];
        //center st
        st[0] = (vs[0]+vs[1]+vs[2])/3.f;
        st[1] = (vt[0]+vt[1]+vt[2])/3.f;
        //e1 st
        st[2] = (vs[0]+vs[2])/2.f;
        st[3] = (vt[0]+vt[2])/2.f;
        //e0 st
        st[4] = (vs[1]+vs[2])/2.f;
        st[5] = (vt[1]+vt[2])/2.f;
        //p2 st
        st[6] = vs[2];
        st[7] = vt[2];
        splitted.push_back(
            shared_ptr<Primitive>(new BilinearPatch(m_Attributes, 
            center, e1, e0, p2, st, false)));

        //st = new float[8];
        //center st
        st[0] = (vs[0]+vs[1]+vs[2])/3.f;
        st[1] = (vt[0]+vt[1]+vt[2])/3.f;
        //e0 st
        st[2] = (vs[1]+vs[2])/2.f;
        st[3] = (vt[1]+vt[2])/2.f;
        //e2 st
        st[4] = (vs[0]+vs[1])/2.f;
        st[5] = (vt[0]+vt[1])/2.f;
        //p1 st
        st[6] = vs[1];
        st[7] = vt[1];
        splitted.push_back(
            shared_ptr<Primitive>(new BilinearPatch(m_Attributes, 
            center, e0, e2, p1, st, false)));
    } else {
        splitted.push_back(
            shared_ptr<Primitive>(new BilinearPatch(m_Attributes, 
            center, e2, e1, p0, NULL, false)));
        splitted.push_back(
            shared_ptr<Primitive>(new BilinearPatch(m_Attributes, 
            center, e1, e0, p2, NULL, false)));
        splitted.push_back(
            shared_ptr<Primitive>(new BilinearPatch(m_Attributes, 
            center, e0, e2, p1, NULL, false)));
    }
}

} //namespace ToyMan
