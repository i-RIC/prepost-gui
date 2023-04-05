#include "../datamodel/vtk2dgraphicsview.h"
#include "mouseboundingbox.h"

#include <misc/mathsupport.h>

#include <vtkCamera.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>

MouseBoundingBox::MouseBoundingBox(VTK2DGraphicsView* view, QObject* parent) :
	QObject(parent),
	m_graphicsView {view}
{
	m_polygon = vtkSmartPointer<vtkPolygon>::New();
	vtkIdList* idlist = m_polygon->GetPointIds();
	vtkPoints* points = m_polygon->GetPoints();
	idlist->SetNumberOfIds(4);
	points->SetNumberOfPoints(4);
	for (int i = 0; i < 4; ++i) {
		idlist->SetId(i, i);
		points->SetPoint(i, 0, 0, 0);
	}
	points->Modified();
	m_lineGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	int edgeCount = m_polygon->GetNumberOfEdges();
	m_lineGrid->Allocate(edgeCount);
	for (int i = 0; i < edgeCount; ++i) {
		vtkCell* nextCell = m_polygon->GetEdge(i);
		m_lineGrid->InsertNextCell(nextCell->GetCellType(), nextCell->GetPointIds());
	}
	m_lineGrid->SetPoints(m_polygon->GetPoints());

	m_regionGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_regionGrid->Allocate(2, 1);

	vtkTriangle* tri = vtkTriangle::New();
	tri->GetPointIds()->SetId(0, 0);
	tri->GetPointIds()->SetId(1, 1);
	tri->GetPointIds()->SetId(2, 2);
	m_regionGrid->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	tri->GetPointIds()->SetId(0, 2);
	tri->GetPointIds()->SetId(1, 3);
	tri->GetPointIds()->SetId(2, 0);
	m_regionGrid->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	m_regionGrid->SetPoints(points);

	m_lineActor = vtkSmartPointer<vtkActor>::New();
	m_lineActor->GetProperty()->SetColor(.2, .2, .2);
	m_lineActor->GetProperty()->SetLineWidth(2 * view->devicePixelRatioF());
	m_lineActor->GetProperty()->SetLighting(false);
	m_lineMapper = vtkSmartPointer<vtkDataSetMapper>::New();

	m_lineActor->SetMapper(m_lineMapper);
	m_lineMapper->SetInputData(m_lineGrid);

	m_lineMapper->SetScalarVisibility(false);
	m_lineActor->GetProperty()->SetRepresentationToWireframe();

	m_paintGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_paintGrid->Reset();
	m_paintGrid->Allocate(1);
	m_paintGrid->InsertNextCell(m_polygon->GetCellType(), m_polygon->GetPointIds());
	m_paintGrid->SetPoints(m_polygon->GetPoints());

	m_paintActor = vtkSmartPointer<vtkActor>::New();
	m_paintActor->GetProperty()->SetColor(.5, .5, .5);
	m_paintActor->GetProperty()->SetOpacity(.5);
	m_paintActor->GetProperty()->SetLighting(false);

	m_paintMapper = vtkSmartPointer<vtkDataSetMapper>::New();

	m_paintActor->SetMapper(m_paintMapper);
	m_paintMapper->SetInputData(m_paintGrid);

	m_paintMapper->SetScalarVisibility(false);
	m_paintActor->VisibilityOff();

	view->mainRenderer()->AddActor(m_lineActor);
	view->mainRenderer()->AddActor(m_paintActor);

	disable();
}

QPoint MouseBoundingBox::startPoint() const
{
	return m_startPoint;
}

void MouseBoundingBox::setStartPoint(int x, int y)
{
	m_startPoint = QPoint(x, y);
	setEndPoint(x, y);
}

QPoint MouseBoundingBox::endPoint() const
{
	return m_endPoint;
}

void MouseBoundingBox::setEndPoint(int x, int y)
{
	m_endPoint = QPoint(x, y);

	// update points.
	double dx, dy;
//	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
//	points->Allocate(4);
	vtkPoints* points = m_polygon->GetPoints();

	// point 0
	dx = m_startPoint.x();
	dy = m_startPoint.y();
	m_graphicsView->viewportToWorld(dx, dy);
	points->SetPoint(0, dx, dy, 0);
	// point 1
	dx = m_endPoint.x();
	dy = m_startPoint.y();
	m_graphicsView->viewportToWorld(dx, dy);
	points->SetPoint(1, dx, dy, 0);
	// point 2
	dx = m_endPoint.x();
	dy = m_endPoint.y();
	m_graphicsView->viewportToWorld(dx, dy);
	points->SetPoint(2, dx, dy, 0);
	// point 3
	dx = m_startPoint.x(); dy = m_endPoint.y();
	m_graphicsView->viewportToWorld(dx, dy);
	points->SetPoint(3, dx, dy, 0);

	points->Modified();
}

void MouseBoundingBox::enable()
{
	m_lineActor->VisibilityOn();
	m_paintActor->VisibilityOn();
	m_graphicsView->ResetCameraClippingRange();
}
void MouseBoundingBox::disable()
{
	m_lineActor->VisibilityOff();
	m_paintActor->VisibilityOff();
}

void MouseBoundingBox::setZDepth(double z)
{
	m_lineActor->SetPosition(0, 0, z);
	m_paintActor->SetPosition(0, 0, z);
}

bool MouseBoundingBox::isInsideBox(double x, double y) const
{
	double p[3];
	vtkCell* hintCell = 0;
	double pcoords[3];
	int subid;
	double weights[3];
	p[0] = x;
	p[1] = y;
	p[2] = 0;
	vtkIdType cellid = m_regionGrid->FindCell(p, hintCell, 0, 1e-4, subid, pcoords, weights);
	return (cellid >= 0);
}

QPointF MouseBoundingBox::center() const
{
	double center[3], tmpp[3];
	for (int i = 0; i < 3; ++i) {
		center[i] = 0;
	}

	int pnum = m_lineGrid->GetNumberOfPoints();
	for (vtkIdType i = 0; i < pnum; ++i) {
		m_lineGrid->GetPoint(i, tmpp);
		center[0] += tmpp[0];
		center[1] += tmpp[1];
	}

	return QPointF(center[0] / pnum, center[1] / pnum);
}

double MouseBoundingBox::circumfenceCircleRadius() const
{
	double tmpp[3];

	m_lineGrid->GetPoint(0, tmpp);
	QPointF p(tmpp[0], tmpp[1]);

	QPointF c = center();

	return iRIC::distance(c, p);
}

vtkUnstructuredGrid* MouseBoundingBox::vtkGrid() const
{
	return m_lineGrid;
}

vtkUnstructuredGrid* MouseBoundingBox::vtkRegionGrid() const
{
	return m_regionGrid;
}

vtkPolygon* MouseBoundingBox::getPolygon() const
{
	return m_polygon;
}
