#ifndef __BUCKET_H__
#define __BUCKET_H__

#ifdef _MSC_VER
	#include <memory>
#else
	#include <tr1/memory>
#endif 
#include <deque>

using namespace std;

namespace ToyMan {

class Primitive;
class MicroPolygon;

class Bucket {
public:
	void Insert(shared_ptr<Primitive> prim) {m_PrimitiveList.push_back(prim);}
	void Insert(shared_ptr<MicroPolygon> mp) {m_MicroPolygonList.push_back(mp);}
	shared_ptr<Primitive> PopPrimitive();
	shared_ptr<MicroPolygon> PopMicroPolygon();
	void SortPrimitivesByZ();
	void SortMicroPolygonByZ();

private:
	deque<shared_ptr<Primitive> > m_PrimitiveList;
	deque<shared_ptr<MicroPolygon> > m_MicroPolygonList;
};

};

#endif //#ifndef __BUCKET_H__
