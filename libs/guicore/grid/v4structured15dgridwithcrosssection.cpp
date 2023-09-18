#include "v4structured15dgridwithcrosssection.h"
#include "private/v4structured15dgridwithcrosssection_impl.h"

#include <vtkPolyData.h>

v4Structured15dGridWithCrossSection::Impl::Impl(v4Structured15dGridWithCrossSection* parent) :
	m_parent {parent}
{
	m_pointsGrid = vtkPolyData::New();
}

v4Structured15dGridWithCrossSection::Impl::~Impl()
{
	m_pointsGrid->Delete();
}

void v4Structured15dGridWithCrossSection::Impl::setupVtkObjects(int count)
{
	auto grid = m_parent->vtkConcreteData()->concreteData();
	grid->SetDimensions(count, 1, 1);

	auto points = grid->GetPoints();
	points->Initialize();
	points->Allocate(count);

	for (int i = 0; i < count; ++i) {
		points->InsertNextPoint(0, 0, 0);
	}

	auto pointsCells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType pointId;
	for (int i = 0; i < count; ++i) {
		pointId = i;
		pointsCells->InsertNextCell(1, &pointId);
	}
	m_pointsGrid->SetVerts(pointsCells);

	for (int i = 0; i < count; ++i) {
		auto name = QString("point%1").arg(i + 1);
		auto cs = new v4Structured15dGridWithCrossSectionCrossSection(name, m_parent);
		m_crossSections.push_back(cs);
	}
}

v4Structured15dGridWithCrossSection::v4Structured15dGridWithCrossSection() :
	v4Grid2dT<vtkStructuredGridExtended2d> {},
	impl {new Impl {this}}
{
	impl->m_pointsGrid->SetPoints(vtkData()->data()->GetPoints());
	setFilteredData(vtkData()->data());
}

v4Structured15dGridWithCrossSection::~v4Structured15dGridWithCrossSection()
{}

void v4Structured15dGridWithCrossSection::setCrossSectionCount(unsigned int count)
{
	impl->setupVtkObjects(count);
}

vtkPolyData* v4Structured15dGridWithCrossSection::pointsGrid() const
{
	return impl->m_pointsGrid;
}

const std::vector<v4Structured15dGridWithCrossSectionCrossSection*>& v4Structured15dGridWithCrossSection::crossSections() const
{
	return impl->m_crossSections;
}

std::vector<v4Structured15dGridWithCrossSectionCrossSection*>& v4Structured15dGridWithCrossSection::crossSections()
{
	return impl->m_crossSections;
}

v4Structured15dGridWithCrossSectionCrossSection* v4Structured15dGridWithCrossSection::crossSection(int index)
{
	return impl->m_crossSections[index];
}

void v4Structured15dGridWithCrossSection::updateFilteredData(double /*xMin*/, double /*xMax*/, double /*yMin*/, double /*yMax*/)
{}
