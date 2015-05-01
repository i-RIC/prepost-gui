#include "gridcreatingconditiontriangleabstractpolygon.h"
#include "gridcreatingconditiontriangle.h"

#include <misc/errormessage.h>

#include <QPolygonF>
#include <QPointF>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkVertex.h>
#include <vtkTriangle.h>

GridCreatingConditionTriangleAbstractPolygon::GridCreatingConditionTriangleAbstractPolygon(GridCreatingConditionTriangle* parent)
{
	m_parent = parent;

	setupContainers();
	setupActors();
	m_vertexActor->VisibilityOff();

	vtkRenderer* r = m_parent->renderer();
	r->AddActor(m_paintActor);
	r->AddActor(m_edgeActor);
	r->AddActor(m_vertexActor);
	vtkActorCollection* col = m_parent->actorCollection();
	col->AddItem(m_paintActor);
	col->AddItem(m_edgeActor);
	m_parent->updateVisibilityWithoutRendering();
}

GridCreatingConditionTriangleAbstractPolygon::~GridCreatingConditionTriangleAbstractPolygon()
{
	vtkRenderer* r = m_parent->renderer();
	r->RemoveActor(m_paintActor);
	r->RemoveActor(m_edgeActor);
	r->RemoveActor(m_vertexActor);
	vtkActorCollection* col = m_parent->actorCollection();
	col->RemoveItem(m_paintActor);
	col->RemoveItem(m_edgeActor);
	col->RemoveItem(m_vertexActor);
}

bool GridCreatingConditionTriangleAbstractPolygon::isVertexSelectable(const QVector2D& pos, double limitdist)
{
	m_selectedVertexId = m_edgeGrid->FindPoint(pos.x(), pos.y(), 0.0);
	double point[3];
	m_vtkPolygon->GetPoints()->GetPoint(m_selectedVertexId, point);
	QVector2D vertexPos(point[0], point[1]);
	return ((vertexPos - pos).length() < limitdist);
}
bool GridCreatingConditionTriangleAbstractPolygon::isEdgeSelectable(const QVector2D& pos, double limitdist)
{
	double x[3] = {pos.x(), pos.y(), 0.0};
	double closestPoint[3];
	int subId;
	double pcoords[3];
	double weights[32];
	double d = limitdist * limitdist;
	for (vtkIdType i = 0; i < m_edgeGrid->GetNumberOfCells(); ++i) {
		vtkCell* cell = m_edgeGrid->GetCell(i);
		double dist;
		if (1 == cell->EvaluatePosition(x, closestPoint, subId, pcoords, dist, weights)) {
			if (dist < d) {
				// this is the selected edge.
				m_selectedEdgeId = i;
				return true;
			}
		}
	}
	return false;
}

bool GridCreatingConditionTriangleAbstractPolygon::isPolygonSelectable(const QVector2D& pos)
{
	QPolygonF pol = polygon();
	return pol.containsPoint(QPointF(pos.x(), pos.y()), Qt::OddEvenFill);
}

const QPolygonF GridCreatingConditionTriangleAbstractPolygon::polygon(QPointF offset) const
{
	QPolygonF ret;
	vtkIdList* idlist = m_vtkPolygon->GetPointIds();
	vtkPoints* points = m_vtkPolygon->GetPoints();
	if (points->GetNumberOfPoints() == 0) {return ret;}
	int vCount = idlist->GetNumberOfIds();
	QPointF lastP, newP;
	for (int i = 0; i < vCount; ++i) {
		vtkIdType id = idlist->GetId(i);
		double* p = points->GetPoint(id);
		newP = QPointF(*p - offset.x(), *(p + 1) - offset.y());
		if (i == 0 || lastP != newP) {
			ret << newP;
		}
		lastP = newP;
	}
	// For QPolygonF, the start vertex and end vertex should be the same point,
	// to define a closed polygon.
	vtkIdType id = idlist->GetId(0);
	double* p = points->GetPoint(id);
	newP = QPointF(*p - offset.x(), *(p + 1) - offset.y());
	if (lastP != newP) {
		ret << newP;
	}
	return ret;
}

void GridCreatingConditionTriangleAbstractPolygon::setPolygon(const QPolygonF& p)
{
	if (! p.isClosed()) {
		throw ErrorMessage(tr("Please specify a closed polygon!"));
	}
	m_vtkPolygon->Initialize();
	vtkPoints* points = m_vtkPolygon->GetPoints();
	points->SetNumberOfPoints(p.count() - 1);
	for (int i = 0; i < p.count() - 1; ++i) {
		QPointF point = p.at(i);
		points->SetPoint(i, point.x(), point.y(), 0);
	}
	points->Modified();
	updateShapeData();
}


void GridCreatingConditionTriangleAbstractPolygon::setupContainers()
{
	m_vtkPolygon = vtkSmartPointer<vtkPolygon>::New();
	m_vtkPolygon->GetPoints()->SetDataTypeToDouble();

	// setup grid.
	m_vtkGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_edgeGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_vertexGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();

	m_selectedVertexId = 0;
	m_selectedEdgeId = 0;
}

void GridCreatingConditionTriangleAbstractPolygon::setupActors()
{
	m_paintActor = vtkSmartPointer<vtkActor>::New();
	m_edgeActor = vtkSmartPointer<vtkActor>::New();
	m_vertexActor = vtkSmartPointer<vtkActor>::New();
	m_paintActor->GetProperty()->SetLighting(false);
	m_edgeActor->GetProperty()->SetLighting(false);
	m_vertexActor->GetProperty()->SetLighting(false);

	// set properties
	m_edgeActor->GetProperty()->SetLineWidth(2);
	m_vertexActor->GetProperty()->SetPointSize(5.0);

	m_paintMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_paintMapper->UseLookupTableScalarRangeOff();

	m_edgeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_edgeMapper->UseLookupTableScalarRangeOff();

	m_vertexMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_vertexMapper->UseLookupTableScalarRangeOff();

	m_paintMapper->SetInputData(m_vtkGrid);
	m_edgeMapper->SetInputData(m_edgeGrid);
	m_vertexMapper->SetInputData(m_vertexGrid);

	m_paintActor->SetMapper(m_paintMapper);
	m_edgeActor->SetMapper(m_edgeMapper);
	m_vertexActor->SetMapper(m_vertexMapper);
}

void GridCreatingConditionTriangleAbstractPolygon::updateShapeData()
{
	vtkPoints* points = m_vtkPolygon->GetPoints();
	// update idlist.
	vtkIdList* idlist = m_vtkPolygon->GetPointIds();
	idlist->Reset();
	idlist->SetNumberOfIds(points->GetNumberOfPoints());
	for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
		idlist->SetId(i, i);
	}
	idlist->Modified();

	// set the polygon into grid.
	m_vtkGrid->Reset();
	if (m_vtkPolygon->GetPoints()->GetNumberOfPoints() > 0) {
		// triangulate the polygon, and add the triangle cells into the grid.
		vtkSmartPointer<vtkIdList> triIds = vtkSmartPointer<vtkIdList>::New();
		m_vtkPolygon->Triangulate(triIds);
		vtkIdType triFirst = 0;
		while (triFirst < triIds->GetNumberOfIds()) {
			vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
			for (int i = 0; i < 3; ++i) {
				tri->GetPointIds()->SetId(i, triIds->GetId(triFirst + i));
			}
			m_vtkGrid->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
			triFirst += 3;
		}
		m_vtkGrid->SetPoints(m_vtkPolygon->GetPoints());
		m_vtkGrid->Modified();
	}
	// Edge line representation.
	m_edgeGrid->Reset();
	int edgeCount = m_vtkPolygon->GetNumberOfEdges();
	m_edgeGrid->Allocate(edgeCount);
	for (int i = 0; i < edgeCount; ++i) {
		vtkCell* nextCell = m_vtkPolygon->GetEdge(i);
		m_edgeGrid->InsertNextCell(nextCell->GetCellType(), nextCell->GetPointIds());
	}
	m_edgeGrid->SetPoints(m_vtkPolygon->GetPoints());
	m_edgeGrid->BuildLinks();
	m_edgeGrid->Modified();

	// Points representation.
	m_vertexGrid->Reset();
	vtkIdType vertexId = 0;
	int vertexCount = m_vtkPolygon->GetNumberOfPoints();
	m_vertexGrid->Allocate(vertexCount);
	for (int i = 0; i < vertexCount; ++i) {
		vtkVertex* nextVertex = vtkVertex::New();
		nextVertex->GetPointIds()->SetId(0, vertexId);
		m_vertexGrid->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		vertexId += 1;
	}
	m_vertexGrid->SetPoints(m_vtkPolygon->GetPoints());
	m_vertexGrid->Modified();
}

void GridCreatingConditionTriangleAbstractPolygon::setZDepthRange(double min, double max)
{
	m_vertexActor->SetPosition(0, 0, max);
	m_edgeActor->SetPosition(0, 0, max);
	m_paintActor->SetPosition(0, 0, min);
}

void GridCreatingConditionTriangleAbstractPolygon::setActive(bool active)
{
	if (active) {
		m_parent->actorCollection()->AddItem(m_vertexActor);
	} else {
		m_vertexActor->VisibilityOff();
		m_parent->actorCollection()->RemoveItem(m_vertexActor);
	}
	m_parent->updateVisibilityWithoutRendering();
}

QPointF GridCreatingConditionTriangleAbstractPolygon::innerPoint(QPointF offset) const
{
	double v[3];
	QVector2D basePoint, p1, p2, v1, v2, nom;
	QPointF nomP;
	QPolygonF poly = polygon(offset);
	bool found = false;
	int baseId = 0;
	int pnum = m_vtkPolygon->GetNumberOfPoints();

	for (int i = 0; i < pnum && ! found; ++i) {
		m_vtkPolygon->GetPoints()->GetPoint(baseId, v);
		basePoint = QVector2D(v[0] - offset.x(), v[1] - offset.y());
		m_vtkPolygon->GetPoints()->GetPoint((baseId + 1) % pnum, v);
		p1 = QVector2D(v[0] - offset.x(), v[1] - offset.y());
		m_vtkPolygon->GetPoints()->GetPoint((baseId + pnum - 1) % pnum, v);
		p2 = QVector2D(v[0] - offset.x(), v[1] - offset.y());

		v1 = (p1 - basePoint) * 0.01;
		v2 = (p2 - basePoint) * 0.01;

		nom = basePoint + v1 + v2;
		nomP = QPointF(nom.x(), nom.y());
		if (poly.containsPoint(nomP, Qt::OddEvenFill)) {
			return nomP;
		} else {
			nom = basePoint - v1 - v2;
			nomP = QPointF(nom.x(), nom.y());
			if (poly.containsPoint(nomP, Qt::OddEvenFill)) {
				return nomP;
			}
		}
	}
	return QPointF(0, 0);
}
