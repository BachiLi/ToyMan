#include "Bucket.h"
#include "Primitive.h"
#include "AABB.h"
#include "MicroPolygon.h"

namespace ToyMan {

shared_ptr<Primitive> Bucket::PopPrimitive() {
	if(m_PrimitiveList.empty()) {
		shared_ptr<Primitive> empty;
		return empty;
	}
	shared_ptr<Primitive> p = m_PrimitiveList.front();
	m_PrimitiveList.pop_front();
	return p;
}

shared_ptr<MicroPolygon> Bucket::PopMicroPolygon() {
	if(m_MicroPolygonList.empty()) {
		shared_ptr<MicroPolygon> empty;
		return empty;
	}
	shared_ptr<MicroPolygon> mp = m_MicroPolygonList.front();
	m_MicroPolygonList.pop_front();
	return mp;
}

void Bucket::SortPrimitivesByZ() {
	struct PrimComparator {
		bool operator()(const shared_ptr<Primitive> &p1, const shared_ptr<Primitive> &p2) {
			AABB b1 = p1->BoundingBox();
			AABB b2 = p2->BoundingBox();
			return b1.pMin.z < b2.pMin.z;
		}
	};
	sort(m_PrimitiveList.begin(), m_PrimitiveList.end(), PrimComparator());
}

void Bucket::SortMicroPolygonByZ() {
	struct MPComparator {
		bool operator()(const shared_ptr<MicroPolygon> &m1, const shared_ptr<MicroPolygon> &m2) {
			AABB b1 = m1->bBox;
			AABB b2 = m2->bBox;
			return b1.pMin.z < b2.pMin.z;
		}
	};
	sort(m_MicroPolygonList.begin(), m_MicroPolygonList.end(), MPComparator());
}

};