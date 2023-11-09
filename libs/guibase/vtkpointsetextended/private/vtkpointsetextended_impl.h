#ifndef VTKPOINTSETEXTENDED_IMPL_H
#define VTKPOINTSETEXTENDED_IMPL_H

#include "../vtkpointsetextended.h"
#include "../../vtktool/vtkpointsetvaluerangeset.h"

class vtkPointSetGeos2dIndex;

class vtkPointSetExtended::Impl
{
public:
	Impl(vtkPointSet* data, vtkPointSetExtended* p);
	~Impl();

	void rebuildCellIndex();

	vtkPointSet* m_data;
	vtkAbstractPointLocator* m_pointLocator;
	vtkPointSetGeos2dIndex* m_cellIndex;
	vtkPointSetValueRangeSet m_valueRangeSet;

private:
	vtkPointSetExtended* m_parent;
};

#endif // VTKPOINTSETEXTENDED_IMPL_H
