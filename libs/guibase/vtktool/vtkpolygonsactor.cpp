#include "vtkpolygonsactor.h"
#include "private/vtkpolygonsactor_impl.h"

#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>

#include <QPolygonF>

vtkPolygonsActor::Impl::Impl() :
	m_pointsActor {},
	m_linesActor {},
	m_paintActor {},
	m_pointsPolyData {vtkPolyData::New()},
	m_linesPolyData {vtkPolyData::New()},
	m_polyData {vtkPolyData::New()}
{}

vtkPolygonsActor::Impl::~Impl()
{
	m_pointsPolyData->Delete();
	m_linesPolyData->Delete();
	m_polyData->Delete();
}

vtkPolygonsActor::vtkPolygonsActor() :
	impl {new Impl()}
{
	impl->m_pointsActor.setPolyData(impl->m_pointsPolyData);
	impl->m_linesActor.setPolyData(impl->m_linesPolyData);
	impl->m_paintActor.setPolyData(impl->m_polyData);
}

vtkPolygonsActor::~vtkPolygonsActor()
{
	delete impl;
}

vtkActor* vtkPolygonsActor::pointsActor()
{
	return impl->m_pointsActor.actor();
}

vtkActor* vtkPolygonsActor::linesActor()
{
	return impl->m_linesActor.actor();
}

vtkActor* vtkPolygonsActor::paintActor()
{
	return impl->m_paintActor.actor();
}

vtkPolyData* vtkPolygonsActor::pointsPolyData() const
{
	return impl->m_pointsPolyData;
}

vtkPolyData* vtkPolygonsActor::linesPolyData() const
{
	return impl->m_linesPolyData;
}

vtkPolyData* vtkPolygonsActor::polygonsPolyData() const
{
	return impl->m_polyData;
}

void vtkPolygonsActor::setPolygons(const std::vector<QPolygonF>& polygons)
{
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	vtkSmartPointer<vtkCellArray> paintCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkCellArray> linesCells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkCellArray> pointsCells = vtkSmartPointer<vtkCellArray>::New();

	vtkIdType nodeId = 0;

	for (const QPolygonF& polygon : polygons) {
		int size = static_cast<int>(polygon.size()) - 1;
		vtkIdType startId = nodeId;

		vtkSmartPointer<vtkPolygon> vtkPol = vtkSmartPointer<vtkPolygon>::New();
		auto polPoints = vtkPol->GetPoints();
		auto polIds = vtkPol->GetPointIds();
		for (int i = 0; i < size; ++i) {
			const QPointF& p = polygon.at(i);
			points->InsertNextPoint(p.x(), p.y(), 0);
			pointsCells->InsertNextCell(1, &nodeId);

			vtkIdType lineids[2];
			lineids[0] = nodeId;
			lineids[1] = startId + (nodeId - startId+ 1) % size;
			linesCells->InsertNextCell(2, lineids);

			polPoints->InsertNextPoint(p.x(), p.y(), 0);
			polIds->InsertNextId(i);

			++ nodeId;
		}
		if (size >= 3) {
			vtkSmartPointer<vtkIdList> triIds = vtkSmartPointer<vtkIdList>::New();
			vtkPol->Triangulate(triIds);

			vtkIdType tri_ids[3];
			vtkIdType triFirst = 0;
			while (triFirst < triIds->GetNumberOfIds()) {
				for (int i = 0; i < 3; ++i) {
					tri_ids[i] = triIds->GetId(triFirst + i) + startId;
				}
				paintCells->InsertNextCell(3, tri_ids);
				triFirst += 3;
			}
		}
	}
	impl->m_polyData->SetPoints(points);
	impl->m_polyData->SetPolys(paintCells);
	impl->m_polyData->BuildCells();
	impl->m_polyData->BuildLinks();
	impl->m_polyData->Modified();

	impl->m_linesPolyData->SetPoints(points);
	impl->m_linesPolyData->SetLines(linesCells);
	impl->m_linesPolyData->BuildCells();
	impl->m_linesPolyData->BuildLinks();
	impl->m_linesPolyData->Modified();

	impl->m_pointsPolyData->SetPoints(points);
	impl->m_pointsPolyData->SetVerts(pointsCells);
	impl->m_pointsPolyData->BuildCells();
	impl->m_pointsPolyData->BuildLinks();
	impl->m_pointsPolyData->Modified();
}
