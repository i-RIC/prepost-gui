#ifndef V4STRUCTURED3DGRID_IMPL_H
#define V4STRUCTURED3DGRID_IMPL_H

#include "../v4structured3dgrid.h"
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>

class v4Structured3dGrid::Impl
{
public:
	Impl(v4Structured3dGrid* parent);
	~Impl();

	void setupVtkObjects();

	vtkPolyData* m_vtkIndexData;

	vtkPolyDataExtended3d m_vtkIFaceData;
	vtkPolyDataExtended3d m_vtkJFaceData;
	vtkPolyDataExtended3d m_vtkKFaceData;

	vtkIdType m_dimensionI;
	vtkIdType m_dimensionJ;
	vtkIdType m_dimensionK;

private:
	v4Structured3dGrid* m_parent;
};

#endif // V4STRUCTURED3DGRID_IMPL_H
