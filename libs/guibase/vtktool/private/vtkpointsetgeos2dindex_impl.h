#ifndef VTKPOINTSETGEOS2DINDEX_IMPL_H
#define VTKPOINTSETGEOS2DINDEX_IMPL_H

#include "../vtkpointsetgeos2dindex.h"

namespace geos {
	namespace index{
		namespace quadtree {
			class Quadtree;
		} // quadtree
	} // index
} // geos

class vtkPointSetGeos2dIndex::Impl
{
public:
	Impl(vtkPointSet* data);
	~Impl();

	void buildIndex();

	vtkPointSet* m_data;
	geos::index::quadtree::Quadtree* m_qTree;
};

#endif // VTKPOINTSETGEOS2DINDEX_IMPL_H
