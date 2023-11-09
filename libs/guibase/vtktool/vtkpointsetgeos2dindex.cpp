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

vtkIdType vtkPointSetGeos2dIndex::findCell(double x, double y, double radius, double* weights)
{
	auto env = new geos::geom::Envelope(x - radius, x + radius, y - radius, y + radius);
	std::vector<void*> ret;
	impl->m_qTree->query(env, ret);
	delete env;

	double point[3], closestPoint[3], pcoords[3], dist2;
	int subId;

	point[0] = x;
	point[1] = y;
	point[2] = 0;

	double radius2 = radius * radius;
	for (void* vptr : ret) {
		vtkIdType cellId = reinterpret_cast<vtkIdType> (vptr);
		vtkCell* cell = impl->m_data->GetCell(cellId);
		int result = cell->EvaluatePosition(point, closestPoint, subId, pcoords, dist2, weights);
		if (result != 1) {continue;}
		if (dist2 > radius2) {continue;}

		return cellId;
	}

	return -1;
}
