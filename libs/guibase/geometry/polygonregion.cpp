#include "polygonregion.h"
#include "pointring.h"
#include "rect.h"

PolygonRegion::PolygonRegion(PointRing* region) :
	m_region {region}
{}

PolygonRegion::PolygonRegion(const std::vector<vtkIdType>& points, vtkPoints* vtkPs) :
	m_region {new PointRing {points, vtkPs}}
{}

PolygonRegion::PolygonRegion(vtkCell* cell, vtkPoints* vtkPs) :
	m_region {new PointRing {cell, vtkPs}}
{}

PolygonRegion::~PolygonRegion()
{
	delete m_region;
	for (auto h : m_holes) {
		delete h;
	}
}

PointRing* PolygonRegion::region() const
{
	return m_region;
}

const std::vector<PointRing*>& PolygonRegion::holes() const
{
	return m_holes;
}

bool PolygonRegion::merge(PolygonRegion* region)
{
	PointRing* another = nullptr;
	bool ok = m_region->merge(region->region(), &another);
	if (ok) {
		if (another != nullptr) {
			for (auto h : region->m_holes) {
				m_holes.push_back(h);
			}
			region->m_holes.clear();

			if (another->boundingRect() <= m_region->boundingRect()) {
				m_holes.push_back(another);
			} else {
				m_holes.push_back(m_region);
				m_region = another;
			}
		}
		return true;
	}
	if (region->holes().size() > 0) {return false;}

	for (auto h : m_holes) {
		PointRing* another = nullptr;
		bool ok = h->merge(region->region(), &another);
		if (! ok) {continue;}
		if (another != nullptr) {m_holes.push_back(another);}
		return true;
	}
	return false;
}

void PolygonRegion::clean()
{
	std::vector<PointRing*> holes = m_region->clean();
	for (auto h : m_holes) {
		h->clean();
	}
	std::vector<PointRing*> allRings;
	allRings.push_back(m_region);
	for (auto h : m_holes) {
		allRings.push_back(h);
	}
	for (auto h : holes) {
		allRings.push_back(h);
	}
	int largestId = 0;
	Rect largestRect;
	for (int i = 0; i < allRings.size(); ++i) {
		PointRing* r = allRings.at(i);
		Rect rect = r->boundingRect();
		if (i == 0 || largestRect <= rect) {
			largestId = i;
			largestRect = rect;
		}
	}
	m_region = allRings.at(largestId);
	allRings.erase(allRings.begin() + largestId);
	m_holes = allRings;
}
