#ifndef V4STRUCTURED2DGRID_IMPL_H
#define V4STRUCTURED2DGRID_IMPL_H

#include "../v4structured2dgrid.h"
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>

class v4Structured2dGrid::Impl
{
public:
	Impl(v4Structured2dGrid* parent);
	~Impl();

	void setupVtkObjects();

	vtkPolyDataExtended2d m_vtkIEdgeData;
	vtkPolyDataExtended2d m_vtkJEdgeData;
	vtkPolyData* m_vtkIEdgeFilteredData;
	vtkPolyData* m_vtkJEdgeFilteredData;

	vtkIdType m_dimensionI;
	vtkIdType m_dimensionJ;

	vtkIdType m_drawnIMin;
	vtkIdType m_drawnIMax;
	vtkIdType m_drawnJMin;
	vtkIdType m_drawnJMax;

	v4GridStructureCheckerI* m_structureChecker;

private:
	v4Structured2dGrid* m_parent;
};

#endif // V4STRUCTURED2DGRID_IMPL_H
