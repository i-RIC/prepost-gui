#include "v4grid.h"
#include "private/v4grid_impl.h"

#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>

v4Grid::v4Grid(vtkPointSetExtended* data) :
	QObject {},
	impl {new Impl {data}}
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	impl->m_vtkData->data()->SetPoints(points);
}

v4Grid::~v4Grid()
{}

vtkPointSetExtended* v4Grid::vtkData() const
{
	return impl->m_vtkData;
}

vtkIdType v4Grid::nodeCount() const
{
	return impl->m_vtkData->data()->GetNumberOfPoints();
}

vtkIdType v4Grid::cellCount() const
{
	return impl->m_vtkData->data()->GetNumberOfCells();
}

v4Grid::AttributeDataProvider* v4Grid::attributeDataProvider() const
{
	return impl->m_attributeDataProvider;
}

void v4Grid::setAttributeDataProvider(AttributeDataProvider* provider)
{
	impl->m_attributeDataProvider = provider;
}

void v4Grid::applyOffset(const QPointF& point)
{
	auto points = impl->m_vtkData->data()->GetPoints();
	vtkIdType numPoints = points->GetNumberOfPoints();
	for (vtkIdType id = 0; id < numPoints; ++id) {
		double v[3];
		points->GetPoint(id, v);
		v[0] -= point.x();
		v[1] -= point.y();
		points->SetPoint(id, v);
	}

	pointsModified();
}

void v4Grid::pointsModified()
{
	impl->m_vtkData->data()->GetPoints()->Modified();
	updateCellIndex();
}

void v4Grid::updateCellIndex()
{
	vtkData()->updateCellIndex();
}

v4GridStructureCheckerI* v4Grid::structureChecker() const
{
	return impl->m_structureChecker;
}
