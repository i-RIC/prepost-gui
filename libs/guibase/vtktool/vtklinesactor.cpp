#include "vtklinesactor.h"
#include "private/vtklinesactor_impl.h"

#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

vtkLinesActor::Impl::Impl() :
	m_linesActor {},
	m_pointsActor {},
	m_polyData {vtkPolyData::New()}
{}

vtkLinesActor::Impl::~Impl()
{
	m_polyData->Delete();
}

vtkLinesActor::vtkLinesActor() :
	impl {new Impl()}
{
	impl->m_pointsActor.setPolyData(impl->m_polyData);
	impl->m_linesActor.setPolyData(impl->m_polyData);
}

vtkLinesActor::~vtkLinesActor()
{
	delete impl;
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
	impl->m_polyData->Initialize();

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

	vtkIdType nodeId = 0;

	for (const std::vector<QPointF>& line : lines) {
		std::vector<vtkIdType> cellIds(line.size());
		for (int i = 0; i < line.size(); ++i){
			const QPointF& p = line.at(i);
			points->InsertNextPoint(p.x(), p.y(), 0);
			cellIds[i] = nodeId ++;
		}
		cells->InsertNextCell(line.size(), cellIds.data());
	}
	impl->m_polyData->SetPoints(points);
	impl->m_polyData->SetLines(cells);
}
