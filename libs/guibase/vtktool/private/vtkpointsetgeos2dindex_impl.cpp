#include "vtkpointsetgeos2dindex_impl.h"

#include <geos/geom/Envelope.h>
#include <geos/index/quadtree/Quadtree.h>

#include <vtkPointSet.h>

#include <unordered_set>

vtkPointSetGeos2dIndex::Impl::Impl(vtkPointSet* data) :
	m_data {data},
	m_qTree {new geos::index::quadtree::Quadtree()}
{
	buildIndex();
}

vtkPointSetGeos2dIndex::Impl::~Impl()
{
	delete m_qTree;
}

void vtkPointSetGeos2dIndex::Impl::buildIndex()
{
	std::unordered_set<unsigned int> ids;
	for (unsigned int i = 0; i < m_data->GetNumberOfCells(); ++i) {
		ids.insert(i);
	}

	double bounds[6];
	for (unsigned int cellid : ids) {
		m_data->GetCellBounds(cellid, bounds);

		auto env = new geos::geom::Envelope(bounds[0], bounds[1], bounds[2], bounds[3]);
		m_qTree->insert(env, reinterpret_cast<void*> (cellid));
	}
}
