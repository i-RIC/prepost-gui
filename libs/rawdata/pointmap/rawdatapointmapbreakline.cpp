#include "rawdatapointmap.h"
#include "rawdatapointmapbreakline.h"

#include <fstream>

#include <misc/errormessage.h>

#include <QPointF>

#include <vtkCellArray.h>
#include <vtkIndent.h>
#include <vtkLine.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkVertex.h>

RawDataPointmapBreakLine::RawDataPointmapBreakLine(RawDataPointmap* parent)
	: QObject(parent)
{
	m_parent = parent;

	setupContainers();
	setupActors();
	// only for test
	m_verticesActor->VisibilityOff();

	vtkRenderer* r = m_parent->renderer();
	r->AddActor(m_edgesActor);
	r->AddActor(m_verticesActor);
	vtkActorCollection* col = m_parent->actorCollection();
	col->AddItem(m_edgesActor);
	m_parent->updateVisibilityWithoutRendering();
}

RawDataPointmapBreakLine::~RawDataPointmapBreakLine()
{
	vtkRenderer* r = m_parent->renderer();
	r->RemoveActor(m_edgesActor);
	r->RemoveActor(m_verticesActor);
	vtkActorCollection* col = m_parent->actorCollection();
	col->RemoveItem(m_edgesActor);
	col->RemoveItem(m_verticesActor);
}

bool RawDataPointmapBreakLine::isEdgeSelectable(const QVector2D& pos, double limitdist)
{
	double x[3] = {pos.x(), pos.y(), 0.0};
	double closestPoint[3];
	int subId;
	double pcoords[3];
	double weights[32];
	double d = limitdist * limitdist;
	for (vtkIdType i = 0; i < m_edges->GetNumberOfCells(); ++i){
		vtkCell* cell = m_edges->GetCell(i);
		double dist;
		if (1 == cell->EvaluatePosition(x, closestPoint, subId, pcoords, dist, weights)){
			if (dist < d){
				// this is the selected edge.
				m_selectedEdgeId = i;
				return true;
			}
		}
	}
	return false;
}

const QVector<QPointF> RawDataPointmapBreakLine::polyLine() const
{
	QVector<QPointF> ret;
	vtkIdList* idlist = m_vtkPolyLine->GetPointIds();
	vtkPoints* points = m_vtkPolyLine->GetPoints();
	if (points->GetNumberOfPoints() == 0){return ret;}
	int vCount = idlist->GetNumberOfIds();
	QPointF lastP, newP;
	for (int i = 0; i < vCount; ++i){
		vtkIdType id = idlist->GetId(i);
		double* p = points->GetPoint(id);
		newP = QPointF(*p, *(p + 1));
		if (i == 0 || lastP != newP){
			ret << newP;
		}
		lastP = newP;
	}
	return ret;
}

void RawDataPointmapBreakLine::setVertexIndices(const QVector<vtkIdType>& indices)
{
	m_vertexIndices = indices;
	updateShapeData();
}

void RawDataPointmapBreakLine::setPolyLine(const QVector<QPointF>& polyline)
{
	m_vtkPolyLine->Initialize();
	vtkPoints* points = m_vtkPolyLine->GetPoints();
	points->SetNumberOfPoints(polyline.count());
	for (int i = 0; i < polyline.count(); ++i){
		QPointF point = polyline.at(i);
		points->SetPoint(i, point.x(), point.y(), 0);
	}
	points->Modified();
	updateShapeData();
}

void RawDataPointmapBreakLine::setupContainers()
{
	m_vtkPolyLine = vtkSmartPointer<vtkPolyLine>::New();
	m_vtkPolyLine->GetPoints()->SetDataTypeToDouble();

	// setup grid.
	m_edges = vtkSmartPointer<vtkPolyData>::New();
	m_vertices = vtkSmartPointer<vtkPolyData>::New();

	m_selectedVertexId = 0;
	m_selectedEdgeId = 0;
}

void RawDataPointmapBreakLine::setupActors()
{
	m_edgesActor = vtkSmartPointer<vtkActor>::New();
	m_verticesActor = vtkSmartPointer<vtkActor>::New();

	m_edgesActor->GetProperty()->SetLighting(false);
	m_verticesActor->GetProperty()->SetLighting(false);

	// set properties
	m_edgesActor->GetProperty()->SetLineWidth(3);
	m_edgesActor->GetProperty()->SetColor(0, 0, 0);
	m_verticesActor->GetProperty()->SetPointSize(9);
	m_verticesActor->GetProperty()->SetColor(0, 0, 0);

	m_edgesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_edgesMapper->UseLookupTableScalarRangeOff();
	m_edgesMapper->ImmediateModeRenderingOn();

	m_verticesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_verticesMapper->UseLookupTableScalarRangeOff();
	m_verticesMapper->ImmediateModeRenderingOn();

	m_edgesMapper->SetInputData(m_edges);
	m_verticesMapper->SetInputData(m_vertices);

	m_edgesActor->SetMapper(m_edgesMapper);
	m_verticesActor->SetMapper(m_verticesMapper);
}

void RawDataPointmapBreakLine::updateShapeData()
{
	// update points.
	m_vtkPolyLine->Points->UnRegister(m_vtkPolyLine);
	m_vtkPolyLine->Points = m_parent->vtkGrid()->GetPoints();
	m_vtkPolyLine->Points->Register(m_vtkPolyLine);

	vtkIdList* idlist = m_vtkPolyLine->GetPointIds();
	idlist->Initialize();
	for (int i = 0; i < m_vertexIndices.count(); ++i){
		idlist->InsertNextId(m_vertexIndices[i]);
	}
	// edge grid is constructed.
	m_edges->Reset();
	m_edges->SetPoints(m_vtkPolyLine->GetPoints());

	int edgeCount = idlist->GetNumberOfIds() - 1;
	vtkCellArray* edges = vtkCellArray::New();
	edges->Allocate(edgeCount);
	vtkIdType points[2];
	for (int i = 0; i < edgeCount; ++i){
		points[0] = idlist->GetId(i);
		points[1] = idlist->GetId(i + 1);
		edges->InsertNextCell(2, points);
	}
	m_edges->SetLines(edges);
	edges->Delete();
/*
	vtkLine* tmpEdge = vtkLine::New();
	for (int i = 0; i < edgeCount; ++i){
		tmpEdge->GetPointIds()->SetId(0, idlist->GetId(i));
		tmpEdge->GetPointIds()->SetId(1, idlist->GetId(i + 1));
		m_edges->InsertNextCell(tmpEdge->GetCellType(), tmpEdge->GetPointIds());
	}
	tmpEdge->Delete();
*/
// m_edges->BuildLinks();
	m_edges->Modified();

	// points are constructed.
	m_vertices->Reset();
	m_vertices->SetPoints(m_vtkPolyLine->GetPoints());

	int vertexCount = idlist->GetNumberOfIds();
	vtkCellArray* vertices = vtkCellArray::New();
	vertices->Allocate(vertexCount);
	vtkIdType pointId;
	for (int i = 0; i < vertexCount; ++i){
		pointId = idlist->GetId(i);
		vertices->InsertNextCell(1, &pointId);
	}
	m_vertices->SetVerts(vertices);
	vertices->Delete();
/*
	vtkVertex* tmpVertex = vtkVertex::New();
	for (int i = 0; i < vertexCount; ++i){
		tmpVertex->GetPointIds()->SetId(0, idlist->GetId(i));
		m_vertices->InsertNextCell(tmpVertex->GetCellType(), tmpVertex->GetPointIds());
	}
	tmpVertex->Delete();
*/
//    m_vertices->BuildLinks();
	m_vertices->Modified();
}

void RawDataPointmapBreakLine::setZDepthRange(double /*min*/, double max)
{
	m_verticesActor->SetPosition(0, 0, max);
	m_edgesActor->SetPosition(0, 0, max);
}

void RawDataPointmapBreakLine::setActive(bool active)
{
	vtkActorCollection* col = m_parent->actorCollection();
	m_verticesActor->VisibilityOff();
	col->RemoveItem(m_verticesActor);
	if (active){
		col->AddItem(m_verticesActor);
	}
	m_parent->updateVisibilityWithoutRendering();
}

void RawDataPointmapBreakLine::setHidden(bool hidden)
{
	vtkActorCollection* col = m_parent->actorCollection();
	col->RemoveItem(m_verticesActor);
	col->RemoveItem(m_edgesActor);
	m_verticesActor->VisibilityOff();
	m_edgesActor->VisibilityOff();
	if (! hidden){
		col->AddItem(m_edgesActor);
	}
	m_parent->updateVisibilityWithoutRendering();
}
