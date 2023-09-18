#include "v4structured2dgrid_impl.h"
#include "v4structured2dgrid_structurechecker.h"

v4Structured2dGrid::Impl::Impl(v4Structured2dGrid* parent) :
	m_vtkIEdgeData {},
	m_vtkJEdgeData {},
	m_vtkIEdgeFilteredData {vtkPolyData::New()},
	m_vtkJEdgeFilteredData {vtkPolyData::New()},
	m_dimensionI {0},
	m_dimensionJ {0},
	m_drawnIMin {0},
	m_drawnIMax {0},
	m_drawnJMin {0},
	m_drawnJMax {0},
	m_structureChecker {new StructureChecker {}},
	m_parent {parent}
{
	auto points = m_parent->vtkData()->data()->GetPoints();
	m_vtkIEdgeData.data()->SetPoints(points);
	m_vtkJEdgeData.data()->SetPoints(points);
	m_vtkIEdgeFilteredData->SetPoints(points);
	m_vtkJEdgeFilteredData->SetPoints(points);
}

v4Structured2dGrid::Impl::~Impl()
{
	m_vtkIEdgeFilteredData->Delete();
	m_vtkJEdgeFilteredData->Delete();
	delete m_structureChecker;
}

void v4Structured2dGrid::Impl::setupVtkObjects()
{
	auto grid = m_parent->vtkConcreteData()->concreteData();
	grid->SetDimensions(m_dimensionI, m_dimensionJ, 1);

	auto pointCount = m_dimensionI * m_dimensionJ;
	auto points = grid->GetPoints();
	points->Initialize();
	points->Allocate(pointCount);

	for (int i = 0; i < pointCount; ++i) {
		points->InsertNextPoint(0, 0, 0);
	}

	vtkIdType indices[2];

	auto iEdgeLines = vtkSmartPointer<vtkCellArray>::New();
	iEdgeLines->Allocate(m_dimensionI * (m_dimensionJ - 1));
	for (int j = 0; j < m_dimensionJ - 1; ++j) {
		for (int i = 0; i < m_dimensionI; ++i) {
			indices[0] = m_parent->pointIndex(i, j);
			indices[1] = m_parent->pointIndex(i, j + 1);
			iEdgeLines->InsertNextCell(2, indices);
		}
	}
	m_vtkIEdgeData.concreteData()->SetLines(iEdgeLines);
	m_vtkIEdgeData.concreteData()->BuildLinks();

	auto jEdgeLines = vtkSmartPointer<vtkCellArray>::New();
	jEdgeLines->Allocate((m_dimensionI - 1) * m_dimensionJ);
	for (int j = 0; j < m_dimensionJ; ++j) {
		for (int i = 0; i < m_dimensionI - 1; ++i) {
			indices[0] = m_parent->pointIndex(i, j);
			indices[1] = m_parent->pointIndex(i + 1, j);
			jEdgeLines->InsertNextCell(2, indices);
		}
	}
	m_vtkJEdgeData.concreteData()->SetLines(jEdgeLines);
	m_vtkJEdgeData.concreteData()->BuildLinks();
}

