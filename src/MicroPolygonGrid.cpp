#include "MicroPolygonGrid.h"
#include "Grid.h"
#include "MicroPolygon.h"
#include "Primitive.h"
#include "Transform.h"

#include <iostream>
using namespace std;

namespace ToyMan {

template<typename T>
void AddVar(const string &name, shared_ptr<T> var, map<string, shared_ptr<T> > &varMap)
{
    if(varMap.find(name) != varMap.end())
        throw ShaderVariableNameExistedException();
    varMap[name] = var;
}

template<typename T>
shared_ptr<T> FindVar(const string &name, map<string, shared_ptr<T> > &varMap)
{
    typename map<string, shared_ptr<T> >::iterator it = varMap.find(name);
    if(it == varMap.end()) {
        shared_ptr<T> empty;
        return empty;
    }

    return it->second;
}

MicroPolygonGrid::MicroPolygonGrid(shared_ptr<Primitive> prim, int uDim, int vDim)
	: m_Primitive(prim), m_UDim(uDim), m_VDim(vDim) {
    
    NewPointGrid("P");
    NewVectorGrid("dPdu");
    NewVectorGrid("dPdv");
    NewNormalGrid("N");
    NewColorGrid("Cs");
    NewVectorGrid("I");
    NewColorGrid("Ci");   
    NewFloatGrid("u");
    NewFloatGrid("v");
    NewFloatGrid("s");
    NewFloatGrid("t");
}

void MicroPolygonGrid::NewPointGrid(const string &name)
{
    shared_ptr<PointGrid> pointGrid = shared_ptr<PointGrid>(new PointGrid(m_UDim, m_VDim));
    AddPointGrid(name, pointGrid);
}

void MicroPolygonGrid::AddPointGrid(const string &name, shared_ptr<PointGrid> var)
{
    AddVar<PointGrid>(name, var, m_PointGridMap);
}

shared_ptr<PointGrid> MicroPolygonGrid::FindPointGrid(const string &name)
{
    return FindVar<PointGrid>(name, m_PointGridMap);
}

void MicroPolygonGrid::NewColorGrid(const string &name)
{
    shared_ptr<ColorGrid> colorGrid = shared_ptr<ColorGrid>(new ColorGrid(m_UDim, m_VDim));
    AddColorGrid(name, colorGrid);
}

void MicroPolygonGrid::AddColorGrid(const string &name, shared_ptr<ColorGrid> var)
{
    AddVar<ColorGrid>(name, var, m_ColorGridMap);
}

shared_ptr<ColorGrid> MicroPolygonGrid::FindColorGrid(const string &name)
{
    return FindVar<ColorGrid>(name, m_ColorGridMap);
}

void MicroPolygonGrid::NewNormalGrid(const string &name)
{
    shared_ptr<NormalGrid> normalGrid = shared_ptr<NormalGrid>(new NormalGrid(m_UDim, m_VDim));
    AddNormalGrid(name, normalGrid);
}

void MicroPolygonGrid::AddNormalGrid(const string &name, shared_ptr<NormalGrid> var)
{
    AddVar<NormalGrid>(name, var, m_NormalGridMap);
}

shared_ptr<NormalGrid> MicroPolygonGrid::FindNormalGrid(const string &name)
{
    return FindVar<NormalGrid>(name, m_NormalGridMap);
}

void MicroPolygonGrid::NewVectorGrid(const string &name)
{
    shared_ptr<VectorGrid> vectorGrid = shared_ptr<VectorGrid>(new VectorGrid(m_UDim, m_VDim));
    AddVectorGrid(name, vectorGrid);
}

void MicroPolygonGrid::AddVectorGrid(const string &name, shared_ptr<VectorGrid> var)
{
    AddVar<VectorGrid>(name, var, m_VectorGridMap);
}

shared_ptr<VectorGrid> MicroPolygonGrid::FindVectorGrid(const string &name)
{
    return FindVar<VectorGrid>(name, m_VectorGridMap);
}

void MicroPolygonGrid::NewFloatGrid(const string &name)
{
    shared_ptr<FloatGrid> floatGrid = shared_ptr<FloatGrid>(new FloatGrid(m_UDim, m_VDim));
    AddFloatGrid(name, floatGrid);
}

void MicroPolygonGrid::AddFloatGrid(const string &name, shared_ptr<FloatGrid> var)
{
    AddVar<FloatGrid>(name, var, m_FloatGridMap);
}

shared_ptr<FloatGrid> MicroPolygonGrid::FindFloatGrid(const string &name)
{
    return FindVar<FloatGrid>(name, m_FloatGridMap);
}

shared_ptr<LightList> MicroPolygonGrid::GetLightSources() const {
    return m_Primitive->GetLightSources();
}

void MicroPolygonGrid::BustColor(vector<shared_ptr<MicroPolygon> > &microPolygons, const Transform &cameraToRaster, 
								 const DOFParameters &dParams, const string &colorName)
{
    shared_ptr<PointGrid> pointGrid = FindPointGrid("P");
    if(!pointGrid)
        throw ShaderVariableNotFoundException();
	PointGrid& P = *pointGrid;

    shared_ptr<ColorGrid> colorGrid = FindColorGrid(colorName);
    if(!colorGrid) {//not found mean the grid is not shaded
		colorGrid = shared_ptr<ColorGrid>(new ColorGrid(pointGrid->GetUSize(), pointGrid->GetVSize()));
		*colorGrid = Color(0.f,0.f,0.f);
	}
    
    ColorGrid& Ci = *colorGrid;
    shared_ptr<MicroPolygonVertex> *vertices = new shared_ptr<MicroPolygonVertex>[m_UDim*m_VDim];
    for(int v = 0; v < m_VDim; v++) {
        for(int u = 0; u < m_UDim; u++) {
            Point oP = P(u,v);			
            Point tP = cameraToRaster(oP);
			Point mP = (*m_Primitive->GetMovementTransform())(oP);
			Point tmP = cameraToRaster(mP);
            shared_ptr<MicroPolygonVertex> mpv = shared_ptr<MicroPolygonVertex>(
				new MicroPolygonVertex(Point(tP.x,tP.y,oP.z), Point(tmP.x, tmP.y, mP.z), 
									   Ci(u,v), Color(1.f, 1.f, 1.f), dParams.CoC(oP.z)));
            vertices[v*m_UDim+u] = mpv;
        }
    }

    for(int v = 0; v < m_VDim-1; v++) {
        for(int u = 0; u < m_UDim-1; u++) {			
            #define VERT(x,y) vertices[(y)*m_UDim+(x)]
            shared_ptr<MicroPolygon> mp;			
            mp = shared_ptr<MicroPolygon>(new MicroPolygon(VERT(u  ,v  ),
                                                           VERT(u+1,v  ),
                                                           VERT(u  ,v+1),
														   VERT(u+1,v+1),
                                                           m_Primitive->GetShadingInterpolation()
                                                           == SMOOTH));
            microPolygons.push_back(mp);
            #undef VERT
        }
    }

    delete[] vertices;
}

bool MicroPolygonGrid::BackFaced() {
    shared_ptr<NormalGrid> N = FindNormalGrid("N");
    shared_ptr<VectorGrid> I = FindVectorGrid("I");
    for(int v = 0; v < m_VDim; v++)
        for(int u = 0; u < m_UDim; u++) {
            if(Dot((*N)(u,v),(*I)(u,v)) < 0.f)
                return false;
        }
    return true;
}

};
