#include "vtkpointsetgeos2dindex.h"
#include "private/vtkpointsetgeos2dindex_impl.h"

#include <geos/geom/Envelope.h>
#include <geos/index/quadtree/Quadtree.h>

#include <vtkCell.h>
#include <vtkPointSet.h>

vtkPointSetGeos2dIndex::vtkPointSetGeos2dIndex(vtkPointSet* ps) :
	impl {new Impl {ps}}
{}

vtkPointSetGeos2dIndex::~vtkPointSetGeos2dIndex()
{
	delete impl;
}

vtkPointSet* vtkPointSetGeos2dIndex::data() const
{
	return impl->m_data;
}

vtkCell* vtkPointSetGeos2dIndex::findCell(double x, double y, double* weights)
{
	auto env = new geos::geom::Envelope(x, x, y, y);
	std::vector<void*> ret;
	impl->m_qTree->query(env, ret);
	delete env;

	double point[3], closestPoint[3], pcoords[3], dist;
	double bounds[6];
	int subId;

	point[0] = x;
	point[1] = y;
	point[2] = 0;

	for (void* vptr : ret) {
		vtkIdType cellId = reinterpret_cast<vtkIdType> (vptr);
		impl->m_data->GetCellBounds(cellId, bounds);

		if (point[0] < bounds[0]) {continue;}
		if (point[0] > bounds[1]) {continue;}
		if (point[1] < bounds[2]) {continue;}
		if (point[1] > bounds[3]) {continue;}

		vtkCell* cell = impl->m_data->GetCell(cellId);
		if (1 == cell->EvaluatePosition(point, closestPoint, subId, pcoords, dist, weights)) {
			return cell;
		}
	}

	return nullptr;
}
