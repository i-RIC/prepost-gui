#include "attributebrowsertargetdataitem.h"
#include "vtkgraphicsview.h"

#include <guibase/graphicsmisc.h>
#include <misc/errormessage.h>

#include <QPolygonF>
#include <QVector2D>

#include <vtkCellArray.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

AttributeBrowserTargetDataItem::AttributeBrowserTargetDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(parent)
{
	setupContainers();
	setupActors();

	clear();
}

AttributeBrowserTargetDataItem::~AttributeBrowserTargetDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_paintActor);
	r->RemoveActor(m_edgeActor);
	r->RemoveActor(m_vertexActor);
}

void AttributeBrowserTargetDataItem::setupContainers()
{
	m_polygon = vtkSmartPointer<vtkPolygon>::New();

	m_polygonPaint = vtkSmartPointer<vtkPolyData>::New();
	m_polygonEdge = vtkSmartPointer<vtkPolyData>::New();
	m_vertex = vtkSmartPointer<vtkPolyData>::New();
}

void AttributeBrowserTargetDataItem::setupActors()
{
	m_paintActor = vtkSmartPointer<vtkActor>::New();
	m_paintActor->GetProperty()->SetColor(0, 0, 0);
	m_paintActor->GetProperty()->SetOpacity(0.6);
	renderer()->AddActor(m_paintActor);

	m_edgeActor = vtkSmartPointer<vtkActor>::New();
	m_edgeActor->GetProperty()->SetLineWidth(2);
	m_edgeActor->GetProperty()->SetColor(0, 0, 0);
	m_edgeActor->GetProperty()->SetOpacity(0.6);
	renderer()->AddActor(m_edgeActor);

	m_vertexActor = vtkSmartPointer<vtkActor>::New();
	m_vertexActor->GetProperty()->SetPointSize(7);
	m_vertexActor->GetProperty()->SetColor(0, 0, 0);
	m_vertexActor->GetProperty()->SetOpacity(0.6);
	m_vertexActor->GetProperty()->LightingOff();
	renderer()->AddActor(m_vertexActor);

	m_paintMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_paintMapper->SetInputData(m_polygonPaint);

	m_edgeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_edgeMapper->SetInputData(m_polygonEdge);

	m_vertexMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_vertexMapper->SetInputData(m_vertex);

	m_paintActor->SetMapper(m_paintMapper);
	m_edgeActor->SetMapper(m_edgeMapper);
	m_vertexActor->SetMapper(m_vertexMapper);
}

void AttributeBrowserTargetDataItem::setPoint(const QVector2D& v)
{
	m_vertex->Initialize();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->InsertNextPoint(v.x(), v.y(), 0);
	m_vertex->SetPoints(points);
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
	ca->InsertNextCell(1);
	ca->InsertCellPoint(0);

	m_vertex->SetVerts(ca);
	m_vertex->Modified();

	m_paintActor->VisibilityOff();
	m_edgeActor->VisibilityOff();
	m_vertexActor->VisibilityOn();

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::setPolygon(const QPolygonF& p)
{
	if (p.size() == 0) {
		m_polygon->Initialize();
		return;
	}
	if (! p.isClosed()) {
		throw ErrorMessage(tr("Please specify a closed polygon!"));
	}
	m_polygon->Initialize();
	vtkPoints* points = m_polygon->GetPoints();
	points->SetNumberOfPoints(p.count() - 1);
	for (int i = 0; i < p.count() - 1; ++i) {
		QPointF point = p.at(i);
		points->SetPoint(i, point.x(), point.y(), 0);
	}

	vtkIdList* idlist = m_polygon->GetPointIds();
	idlist->Reset();
	idlist->SetNumberOfIds(points->GetNumberOfPoints());
	for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
		idlist->SetId(i, i);
	}
	idlist->Modified();

	// set the polygon into grid.
	m_polygonPaint->Reset();
	if (m_polygon->GetPoints()->GetNumberOfPoints() > 0) {
		// triangulate the polygon, and add the triangle cells into the grid.
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		vtkSmartPointer<vtkIdList> triIds = vtkSmartPointer<vtkIdList>::New();
		m_polygon->Triangulate(triIds);
		vtkIdType triFirst = 0;
		while (triFirst < triIds->GetNumberOfIds()) {
			vtkIdType vertices[3];
			for (int i = 0; i < 3; ++i) {
				vertices[i] = triIds->GetId(triFirst + i);
			}
			ca->InsertNextCell(3, &(vertices[0]));
			triFirst += 3;
		}
		m_polygonPaint->SetPoints(m_polygon->GetPoints());
		m_polygonPaint->SetPolys(ca);
		m_polygonPaint->Modified();
	}
	// Edge line representation.
	m_polygonEdge->Reset();
	int edgeCount = m_polygon->GetNumberOfEdges();
	vtkSmartPointer<vtkCellArray> ca2 = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < edgeCount; ++i) {
		vtkIdType vertices[2];
		vtkCell* nextCell = m_polygon->GetEdge(i);
		vertices[0] = nextCell->GetPointId(0);
		vertices[1] = nextCell->GetPointId(1);
		ca2->InsertNextCell(2, &(vertices[0]));
	}
	m_polygonEdge->SetPoints(m_polygon->GetPoints());
	m_polygonEdge->SetLines(ca2);
	m_polygonEdge->Modified();

	m_paintActor->VisibilityOn();
	m_edgeActor->VisibilityOn();
	m_vertexActor->VisibilityOff();

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::clear()
{

	m_polygonPaint->Initialize();
	m_polygonEdge->Initialize();
	m_vertex->Initialize();

	m_paintActor->VisibilityOff();
	m_edgeActor->VisibilityOff();
	m_vertexActor->VisibilityOff();

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void AttributeBrowserTargetDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_paintActor->SetPosition(0, 0, range.min());
	m_edgeActor->SetPosition(0, 0, range.max());
	m_vertexActor->SetPosition(0, 0, range.max());
}

void AttributeBrowserTargetDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void AttributeBrowserTargetDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
