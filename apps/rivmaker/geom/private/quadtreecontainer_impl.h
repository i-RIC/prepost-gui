#ifndef QUADTREECONTAINER_IMPL_H
#define QUADTREECONTAINER_IMPL_H

#include "../quadtreecontainer.h"

#include <geos/index/quadtree/Quadtree.h>
#include <geos/indexQuadtree.h>

class QuadtreeContainer::Impl
{
public:
	Impl();
	~Impl();

	void clear();

	geos::index::quadtree::Quadtree* m_qTree;
};

#endif // QUADTREECONTAINER_IMPL_H
