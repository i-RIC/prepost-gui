#include "v4polydata2d.h"

v4PolyData2d::v4PolyData2d() :
	v4Grid2dT<vtkPolyDataExtended2d>()
{
	setFilteredData(vtkData()->data());
}

const std::vector<vtkIdType>& v4PolyData2d::cellIds() const
{
	return m_cellIds;
}

void v4PolyData2d::setCellIds(const std::vector<vtkIdType>& ids)
{
	m_cellIds = ids;
}

void v4PolyData2d::updateFilteredData(double /*xMin*/, double /*xMax*/, double /*yMin*/, double /*yMax*/)
{}
