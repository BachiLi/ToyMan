#ifndef __MICRO_POLYGON_GRID_H__
#define __MICRO_POLYGON_GRID_H__

#include "Grid.h"
#include "Point.h"
#include "DOFParameters.h"
#include "RNG.h"
#include "LightShader.h"
#include <map>
#include <vector>
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER
#include <string>
#include <exception>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

class Primitive;
class ShaderVariable;
class MicroPolygon;
class Transform;

class ShaderVariableNameExistedException : public exception {
public:
    virtual const char* what() const throw()
    {
        return "The name of the shader variable has already existed";
    }
};

class ShaderVariableNotFoundException : public exception {
public:
    virtual const char* what() const throw()
    {
        return "Shader variable not found!";
    }
};

class MicroPolygonGrid {
public:
    MicroPolygonGrid(shared_ptr<Primitive> prim, int uDim, int vDim);

    int GetUDimenstion() const {return m_UDim;}
    int GetVDimenstion() const {return m_VDim;}

    void NewPointGrid(const string &name);
    void AddPointGrid(const string &name, shared_ptr<PointGrid> var);
    shared_ptr<PointGrid> FindPointGrid(const string &name);

    void NewColorGrid(const string &name);
    void AddColorGrid(const string &name, shared_ptr<ColorGrid> var);
    shared_ptr<ColorGrid> FindColorGrid(const string &name);

    void NewNormalGrid(const string &name);
	void AddNormalGrid(const string &name, shared_ptr<NormalGrid> var);
    shared_ptr<NormalGrid> FindNormalGrid(const string &name);

    void NewVectorGrid(const string &name);
	void AddVectorGrid(const string &name, shared_ptr<VectorGrid> var);
	shared_ptr<VectorGrid> FindVectorGrid(const string &name);

    void NewFloatGrid(const string &name);
	void AddFloatGrid(const string &name, shared_ptr<FloatGrid> var);
	shared_ptr<FloatGrid> FindFloatGrid(const string &name);

    //note that the busted micropolygons will be in the raster space
    void BustColor(vector<shared_ptr<MicroPolygon> > &microPolygons, const Transform &cameraToRaster, 
		           const DOFParameters &dParams, const string &colorName = string("Ci"));

    bool BackFaced();

    shared_ptr<LightList> GetLightSources() const;

private:
    shared_ptr<Primitive> m_Primitive;
    int m_UDim, m_VDim;
    map<string, shared_ptr<PointGrid> > m_PointGridMap;
    map<string, shared_ptr<ColorGrid> > m_ColorGridMap;
	map<string, shared_ptr<NormalGrid> > m_NormalGridMap;
	map<string, shared_ptr<VectorGrid> > m_VectorGridMap;
    map<string, shared_ptr<FloatGrid> > m_FloatGridMap;
};

};

#endif //#ifndef __MICRO_POLYGON_GRID_H__
