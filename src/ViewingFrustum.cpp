#include "ViewingFrustum.h"
#include "AABB.h"
#include "Point.h"
#include "Normal.h"
#include "Util.h"

namespace ToyMan {

void ViewingFrustum::AddPerspective(float zNear, float zFar, float aspectRatio, float fov) {
	float tangent = tan(Radians(fov*0.5f));
	float halfNearHeight = zNear*tangent;
	float halfNearWidth = halfNearHeight*aspectRatio;
	float halfFarHeight = zFar*tangent;
	float halfFarWidth = halfFarHeight*aspectRatio;	
	Point farTopLeft      = Point( -halfFarWidth,  halfFarHeight, zFar);
	Point farTopRight     = Point(  halfFarWidth,  halfFarHeight, zFar);
	Point farBottomLeft   = Point( -halfFarWidth, -halfFarHeight, zFar);
	Point farBottomRight  = Point(  halfFarWidth, -halfFarHeight, zFar);
	Point nearTopLeft     = Point(-halfNearWidth, halfNearHeight, zNear);
	Point nearTopRight    = Point( halfNearWidth, halfNearHeight, zNear);
	Point nearBottomLeft  = Point(-halfNearWidth,-halfNearHeight, zNear);
	Point nearBottomRight = Point( halfNearWidth,-halfNearHeight, zNear);
	
    m_Planes.push_back(Plane(Point(0.f,0.f,zNear), Normal(0.f,0.f, 1.f))); //near
    m_Planes.push_back(Plane(Point(0.f,0.f,zFar ), Normal(0.f,0.f,-1.f))); //far
    m_Planes.push_back(Plane(nearBottomLeft, nearTopLeft, farBottomLeft)); //left
    m_Planes.push_back(Plane(nearTopLeft, nearTopRight, farTopLeft)); //top
    m_Planes.push_back(Plane(nearTopRight, nearBottomRight, farBottomRight)); //right
    m_Planes.push_back(Plane(nearBottomRight, nearBottomLeft, farBottomRight)); //bottom	
}

void ViewingFrustum::AddOrthogonal(float zNear, float zFar, float aspectRatio)
{	
	float halfNearHeight = aspectRatio;
	float halfNearWidth = 1.f;
	float halfFarHeight = aspectRatio;
	float halfFarWidth = 1.f;	
	Point farTopLeft      = Point(-halfFarWidth, halfFarHeight, zFar);
	Point farTopRight     = Point( halfFarWidth, halfFarHeight, zFar);
	Point farBottomLeft   = Point(-halfFarWidth,-halfFarHeight, zFar);
	Point farBottomRight  = Point( halfFarWidth,-halfFarHeight, zFar);
	Point nearTopLeft     = Point(-halfNearWidth, halfNearHeight, zNear);
	Point nearTopRight    = Point( halfNearWidth, halfNearHeight, zNear);
	Point nearBottomLeft  = Point(-halfNearWidth,-halfNearHeight, zNear);
	Point nearBottomRight = Point( halfNearWidth,-halfNearHeight, zNear);

    m_Planes.push_back(Plane(Point(0.f,0.f,zNear), Normal(0.f,0.f, 1.f))); //near
    m_Planes.push_back(Plane(Point(0.f,0.f,zFar ), Normal(0.f,0.f,-1.f))); //far
    m_Planes.push_back(Plane(nearBottomLeft, nearTopLeft, farBottomLeft)); //left
    m_Planes.push_back(Plane(nearTopLeft, nearTopRight, farTopLeft)); //top
    m_Planes.push_back(Plane(nearTopRight, nearBottomRight, farBottomRight)); //right
    m_Planes.push_back(Plane(nearBottomRight, nearBottomLeft, farBottomRight)); //bottom	
}

Plane::SIDE ViewingFrustum::GetSide(const Point &p) const
{
	for(size_t i = 0; i < m_Planes.size(); i++)
		if(m_Planes[i].GetSide(p) == Plane::OUTSIDE)
			return Plane::OUTSIDE;
	return Plane::INSIDE;
}

Plane::SIDE ViewingFrustum::GetSide(const AABB &box) const
{
	Plane::SIDE side = Plane::INSIDE;
	//for each plane
	for(size_t i = 0; i < m_Planes.size(); i++) {
		if((side = m_Planes[i].GetSide(box))==Plane::OUTSIDE)
			return Plane::OUTSIDE;
	}
	return side;
}

};