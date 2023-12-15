#include "vtkpointsetextended.h"
#include "private/vtkpointsetextended_impl.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpointsetgeos2dindex.h>

#include <vtkStaticPointLocator.h>
#include <vtkStaticPointLocator2D.h>

vtkPointSetExtended::vtkPointSetExtended(vtkPointSet* data, bool twoDimensional, bool geosIndex) :
	impl {new Impl {data, this}}
{
	if (twoDimensional) {
		impl->m_pointLocator = vtkStaticPointLocator2D::New();
	} else {
		impl->m_pointLocator = vtkStaticPointLocator::New();
	}
	impl->m_pointLocator->SetDataSet(data);

	if (geosIndex) {
		impl->rebuildCellIndex();
	}

	if (data->GetNumberOfPoints() > 0) {
		impl->m_pointLocator->BuildLocator();
	}
}

vtkPointSetExtended::~vtkPointSetExtended()
{
	delete impl;
}

vtkPointSet* vtkPointSetExtended::data() const
{
	return impl->m_data;
}

const vtkPointSetValueRangeSet& vtkPointSetExtended::valueRangeSet() const
{
	return impl->m_valueRangeSet;
}

vtkPointSetValueRangeSet& vtkPointSetExtended::valueRangeSet()
{
	return impl->m_valueRangeSet;
}

bool vtkPointSetExtended::pointScalarValueExists() const
{
	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(impl->m_data->GetPointData());
	return (names.size() > 0);
}

bool vtkPointSetExtended::cellScalarValueExists() const
{
	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(impl->m_data->GetCellData());
	return (names.size() > 0);
}

vtkIdType vtkPointSetExtended::findClosestPoint(double x, double y, double z) const
{
	return impl->m_pointLocator->FindClosestPoint(x, y, z);
}

vtkIdType vtkPointSetExtended::findClosestPointWithinRadius(double radius, double x, double y, double z) const
{
	double p[3] = {x, y, z};
	double dist2;

	return impl->m_pointLocator->FindClosestPointWithinRadius(radius, p, dist2);
}

vtkIdType vtkPointSetExtended::findCell(double x, double y, double z, double radius, double* weights)
{
	if (impl->m_cellIndex != nullptr) {
		return impl->m_cellIndex->findCell(x, y, radius, weights);
	} else {
		double p[3] = {x, y, z};

		double d2 = 0;
		int subId;
		double pcoords[3];
		double weights[32];

		return impl->m_data->FindCell(p, nullptr, 0, d2, subId, pcoords, weights);
	}
}

vtkAbstractPointLocator* vtkPointSetExtended::pointLocator() const
{
	return impl->m_pointLocator;
}

void vtkPointSetExtended::updateCellIndex()
{
	if (impl->m_cellIndex == nullptr) {return;}

	impl->rebuildCellIndex();
}

void vtkPointSetExtended::updateValueRangeSet()
{
	impl->m_valueRangeSet.update(impl->m_data);
}

void vtkPointSetExtended::updatePointValueRange(const std::string& name)
{
	impl->m_valueRangeSet.updatePointDataValueRange(impl->m_data, name);
}

void vtkPointSetExtended::updateCellValueRange(const std::string& name)
{
	impl->m_valueRangeSet.updateCellDataValueRange(impl->m_data, name);
}
