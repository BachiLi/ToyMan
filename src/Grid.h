#ifndef __GRID_H__
#define __GRID_H__

#include "Point.h"
#include "Color.h"
#include "Normal.h"
#include "TwoDArray.h"
#ifdef _MSC_VER
	#include <memory>
#else
	#include <tr1/memory>
#endif

namespace ToyMan {

/**
 *  @brief Generic data structure represents a grid of data.
 *  Used for shading,  stored in MicroPolygonGrid
 */

template<typename T>
class Grid {
public:
    Grid(int uSize, int vSize) : m_Data(uSize, vSize) {}
    Grid(const TwoDArray<T> &data) : m_Data(data) {}
	inline int GetUSize() const {return m_Data.GetColNum();}
	inline int GetVSize() const {return m_Data.GetRowNum();}
    inline T& operator()(int u, int v) {return m_Data(u,v);}
	inline const T& operator()(int u, int v) const {return m_Data(u,v);}
	inline T& operator()(int i) {return m_Data(i);}
	inline const T& operator()(int i) const {return m_Data(i);}
	inline Grid<T>& operator=(const T &val) {m_Data=val; return *this;}	
private:
    TwoDArray<T> m_Data;
};

typedef Grid<Point> PointGrid;
typedef Grid<Color> ColorGrid;
typedef Grid<Normal> NormalGrid;
typedef Grid<Vector> VectorGrid;
typedef Grid<float> FloatGrid;
typedef Grid<bool> BoolGrid;

};

#endif //#ifndef __GRID_H__
