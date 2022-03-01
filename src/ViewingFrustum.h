#ifndef __VIEWING_FRUSTUM_H__
#define __VIEWING_FRUSTUM_H__

#include "Plane.h"
#include <vector>

using namespace std;

namespace ToyMan {

class Point;
class AABB;

class ViewingFrustum
{
public:

    void AddPerspective(float zNear, float zFar, float aspectRatio, float fov);
    void AddOrthogonal(float zNear, float zFar, float aspectRatio);

	Plane::SIDE GetSide(const Point &p) const;	
	Plane::SIDE GetSide(const AABB &box) const;
private:
	vector<Plane> m_Planes;
	
};

};

#endif //#ifndef __VIEWING_FRUSTUM_H__