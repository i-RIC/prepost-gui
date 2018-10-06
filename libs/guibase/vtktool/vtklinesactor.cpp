#include "vtklinesactor.h"
#include "private/vtklinesactor_impl.h"

#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QPointF>

vtkLinesActor::Impl::Impl() :
	m_pointsActor {},
	m_linesActor {},
	m_pointsPolyData {vtkPolyData::New()},
	m_polyData {vtkPolyData::New()}
{}

vtkLinesActor::Impl::~Impl()
{
	m_pointsPolyData->Delete();
	m_polyData->Delete();
}

vtkLinesActor::vtkLinesActor() :
	impl {new Impl()}
{
	impl->m_pointsActor.setPolyData(impl->m_pointsPolyData);
	impl->m_linesActor.setPolyData(impl->m_polyData);
}

vtkLinesActor::~vtkLinesActor()
{
	delete impl;
}

vtkPolyData* vtkLinesActor::pointsPolyData() const
{
	return impl->m_pointsPolyData;
}

vtkPolyData* vtkLinesActor::linesPolyData() const
{
	return impl->m_polyData;
}

vtkActor* vtkLinesActor::pointsActor() const
{
	return impl->m_pointsActor.actor();
}

vtkActor* vtkLinesActor::linesActor() const
{
	return impl->m_linesActor.actor();
}

void vtkLinesActor::setLines(const std::vector<std::vector<QPointF> >& lines)
{
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	vtkSmartPointer<vtkCellArray> linesCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkCellArray> pointsCells = vtkSmartPointer<vtkCellArray>::New();

	vtkIdType nodeId = 0;

	for (const std::vector<QPointF>& line : lines) {
		for (int i = 0; i < line.size(); ++i){
			const QPointF& p = line.at(i);
			points->InsertNextPoint(p.x(), p.y(), 0);
			pointsCells->InsertNextCell(1, &nodeId);

			if (i != line.size() - 1) {
				vtkIdType cellids[2];
				cellids[0] = nodeId;
				cellids[1] = nodeId + 1;
				linesCells->InsertNextCell(2, cellids);
			}

			++ nodeId;
		}
	}
	impl->m_polyData->SetPoints(points);
	impl->m_polyData->SetLines(linesCells);
	impl->m_polyData->BuildCells();
	impl->m_polyData->BuildLinks();
	impl->m_polyData->Modified();

	impl->m_pointsPolyData->SetPoints(points);
	impl->m_pointsPolyData->SetVerts(pointsCells);
	impl->m_pointsPolyData->BuildCells();
	impl->m_pointsPolyData->BuildLinks();
	impl->m_pointsPolyData->Modified();
}
