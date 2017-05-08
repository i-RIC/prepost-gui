#include "geometry.h"
#include "rect.h"
#include "quadtreecontainer.h"

#include "private/quadtreecontainer_impl.h"

#include <geos/geom/Envelope.h>

QuadtreeContainer::Impl::Impl() :
	m_qTree {new geos::index::quadtree::Quadtree()}
{}

QuadtreeContainer::Impl::~Impl()
{
	delete m_qTree;
}

void QuadtreeContainer::Impl::clear()
{
	delete m_qTree;
	m_qTree = new geos::index::quadtree::Quadtree();
}

QuadtreeContainer::QuadtreeContainer() :
	impl {new Impl {}}
{}

QuadtreeContainer::~QuadtreeContainer()
{
	delete impl;
}

void QuadtreeContainer::add(Geometry* geometry)
{
	auto rect = geometry->rect();
	auto env = new geos::geom::Envelope(rect.xMin(), rect.xMax(), rect.yMin(), rect.yMax());
	impl->m_qTree->insert(env, geometry);
}

void QuadtreeContainer::clear()
{
	impl->clear();
}

std::vector<Geometry*> QuadtreeContainer::findIntersectRect(const Rect& rect)
{
	auto env = new geos::geom::Envelope(rect.xMin(), rect.xMax(), rect.yMin(), rect.yMax());
	std::vector<void*> qret;
	impl->m_qTree->query(env, qret);
	delete env;

	std::vector<Geometry*> ret;
	for (void* vptr : qret) {
		auto geom = reinterpret_cast<Geometry*> (vptr);
		if (geom == nullptr) {continue;}

		auto gRect = geom->rect();
		if (! gRect.intersects(rect)) {continue;}

		ret.push_back(geom);
	}

	return ret;
}
