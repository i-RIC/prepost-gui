#include "gridcreatingconditioncompoundchannel.h"
#include "gridcreatingconditioncompoundchannelabstractline.h"

#include <guibase/vtktool/vtkpointsutil.h>
#include <misc/errormessage.h>

#include <QPointF>

#include <vtkActor2DCollection.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkVertex.h>

#define LABEL "label"

GridCreatingConditionCompoundChannelAbstractLine::GridCreatingConditionCompoundChannelAbstractLine(GridCreatingConditionCompoundChannel* parent)
{
	m_parent = parent;

	setupContainers();
	setupActors();
	updateShapeData();
	m_vertexActor->VisibilityOff();

	vtkRenderer* r = m_parent->renderer();
	r->AddActor(m_edgeActor);
	r->AddActor(m_vertexActor);
	r->AddActor2D(m_labelActor);
	vtkActorCollection* col = m_parent->actorCollection();
	col->AddItem(m_edgeActor);
	vtkActor2DCollection* col2 = m_parent->actor2DCollection();
	col2->AddItem(m_labelActor);
}

GridCreatingConditionCompoundChannelAbstractLine::~GridCreatingConditionCompoundChannelAbstractLine()
{
	vtkRenderer* r = m_parent->renderer();
	r->RemoveActor(m_edgeActor);
	r->RemoveActor(m_vertexActor);
	r->RemoveActor2D(m_labelActor);
	vtkActorCollection* col = m_parent->actorCollection();
	col->RemoveItem(m_edgeActor);
	col->RemoveItem(m_vertexActor);
	vtkActor2DCollection* col2 = m_parent->actor2DCollection();
	col2->RemoveItem(m_labelActor);
}

bool GridCreatingConditionCompoundChannelAbstractLine::isVertexSelectable(const QVector2D& pos, double limitdist)
{
	m_selectedVertexId = m_edgeGrid->FindPoint(pos.x(), pos.y(), 0.0);
	double point[3];
	m_vtkPolyLine->GetPoints()->GetPoint(m_selectedVertexId, point);
	QVector2D vertexPos(point[0], point[1]);
	return ((vertexPos - pos).length() < limitdist);
}
bool GridCreatingConditionCompoundChannelAbstractLine::isEdgeSelectable(const QVector2D& pos, double limitdist)
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

const QVector<QPointF> GridCreatingConditionCompoundChannelAbstractLine::polyLine(QPointF offset) const
{
	QVector<QPointF> ret;
	vtkIdList* idlist = m_vtkPolyLine->GetPointIds();
	vtkPoints* points = m_vtkPolyLine->GetPoints();
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
	return ret;
}

void GridCreatingConditionCompoundChannelAbstractLine::setPolyLine(const QVector<QPointF>& polyline)
{
	m_vtkPolyLine->Initialize();
	vtkPoints* points = m_vtkPolyLine->GetPoints();
	points->SetNumberOfPoints(polyline.count());
	for (int i = 0; i < polyline.count(); ++i) {
		QPointF point = polyline.at(i);
		points->SetPoint(i, point.x(), point.y(), 0);
	}
	points->Modified();
	updateShapeData();
}


void GridCreatingConditionCompoundChannelAbstractLine::setupContainers()
{
	m_vtkPolyLine = vtkSmartPointer<vtkPolyLine>::New();
	m_vtkPolyLine->GetPoints()->SetDataTypeToDouble();

	// setup grid.
	m_vtkGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_edgeGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_vertexGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_labelArray = vtkSmartPointer<vtkStringArray>::New();
	m_labelArray->SetName(LABEL);

	m_selectedVertexId = 0;
	m_selectedEdgeId = 0;
}

void GridCreatingConditionCompoundChannelAbstractLine::setupActors()
{
	m_edgeActor = vtkSmartPointer<vtkActor>::New();
	m_vertexActor = vtkSmartPointer<vtkActor>::New();
	m_labelActor = vtkSmartPointer<vtkActor2D>::New();

	m_edgeActor->GetProperty()->SetLighting(false);
	m_vertexActor->GetProperty()->SetLighting(false);

	// set properties
	m_edgeActor->GetProperty()->SetLineWidth(2);
	m_vertexActor->GetProperty()->SetPointSize(5.0);
	m_labelActor->GetProperty()->SetColor(0, 0, 0);

	m_edgeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_edgeMapper->UseLookupTableScalarRangeOff();

	m_vertexMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_vertexMapper->UseLookupTableScalarRangeOff();

	m_labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	m_labelMapper->SetLabelModeToLabelFieldData();
	m_labelMapper->SetFieldDataName(LABEL);
	vtkTextProperty* prop = m_labelMapper->GetLabelTextProperty();
	prop->SetColor(0, 0, 0);
	prop->SetFontSize(FONTSIZE);
	prop->SetFontFamilyToArial();
	prop->BoldOn();
	prop->ItalicOff();
	prop->ShadowOff();
	prop->SetJustificationToLeft();
	prop->SetVerticalJustificationToCentered();

	m_edgeMapper->SetInputData(m_edgeGrid);
	m_vertexMapper->SetInputData(m_vertexGrid);
	m_labelMapper->SetInputData(m_vertexGrid);

	m_edgeActor->SetMapper(m_edgeMapper);
	m_vertexActor->SetMapper(m_vertexMapper);
	m_labelActor->SetMapper(m_labelMapper);
}

void GridCreatingConditionCompoundChannelAbstractLine::updateShapeData()
{
	vtkIdList* idlist = m_vtkPolyLine->GetPointIds();
	vtkPoints* points = m_vtkPolyLine->GetPoints();

	vtkIdType pointCount = points->GetNumberOfPoints();
	idlist->SetNumberOfIds(pointCount);

	// update idlist.
	for (vtkIdType i = 0; i < pointCount; ++i) {
		idlist->SetId(i, i);
	}

	// edge grid is constructed.
	m_edgeGrid->Reset();
	vtkIdType edgeId = 0;
	int edgeCount = m_vtkPolyLine->GetNumberOfPoints() - 1;
	m_edgeGrid->Allocate(edgeCount);
	for (int i = 0; i < edgeCount; ++i) {
		vtkLine* nextEdge = vtkLine::New();
		nextEdge->GetPointIds()->SetId(0, edgeId);
		nextEdge->GetPointIds()->SetId(1, edgeId + 1);
		m_edgeGrid->InsertNextCell(nextEdge->GetCellType(), nextEdge->GetPointIds());
		edgeId += 1;
	}
	m_edgeGrid->SetPoints(m_vtkPolyLine->GetPoints());
	m_edgeGrid->BuildLinks();
	m_edgeGrid->Modified();

	// labels are constructed;
	m_labelArray->Reset();
	int vertexCount = m_vtkPolyLine->GetNumberOfPoints();
	if (vertexCount > 0) {
		m_labelArray->InsertNextValue("  Upstream");
	}
	for (int i = 1; i < vertexCount; ++i) {
		if (i == vertexCount - 1) {
			m_labelArray->InsertNextValue("  Downstream");
		} else {
			m_labelArray->InsertNextValue("");
		}
	}
	// points are constructed.
	m_vertexGrid->Reset();
	vtkIdType vertexId = 0;
	m_vertexGrid->Allocate(vertexCount);
	for (int i = 0; i < vertexCount; ++i) {
		vtkVertex* nextVertex = vtkVertex::New();
		nextVertex->GetPointIds()->SetId(0, vertexId);
		m_vertexGrid->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		vertexId += 1;
	}
	m_vertexGrid->SetPoints(m_vtkPolyLine->GetPoints());
	m_vertexGrid->GetPointData()->AddArray(m_labelArray);
	m_vertexGrid->Modified();
}

void GridCreatingConditionCompoundChannelAbstractLine::setZDepthRange(double /*min*/, double max)
{
	m_vertexActor->SetPosition(0, 0, max);
	m_edgeActor->SetPosition(0, 0, max);
}

QPointF GridCreatingConditionCompoundChannelAbstractLine::vertex(int index) const
{
	double p[3];
	getVtkLine()->GetPoints()->GetPoint(index, p);
	return QPointF(p[0], p[1]);
}

void GridCreatingConditionCompoundChannelAbstractLine::insertVertex(int index, const QPointF& vertex)
{
	auto coords = vtkPointsUtil::getCoordinates(getVtkLine());
	coords.insert(coords.begin() + index, vertex);
	vtkPointsUtil::setCoordinates(getVtkLine(), coords);
	updateShapeData();
}

void GridCreatingConditionCompoundChannelAbstractLine::setVertex(int index, const QPointF& vertex)
{
	auto line = getVtkLine();
	line->GetPoints()->SetPoint(index, vertex.x(), vertex.y(), 0);
	line->GetPoints()->Modified();
	line->Modified();
	updateShapeData();
}

void GridCreatingConditionCompoundChannelAbstractLine::removeVertex(int index)
{
	auto coords = vtkPointsUtil::getCoordinates(getVtkLine());
	coords.erase(coords.begin() + index);
	vtkPointsUtil::setCoordinates(getVtkLine(), coords);
	updateShapeData();
}

void GridCreatingConditionCompoundChannelAbstractLine::setActive(bool active)
{
	if (active) {
		m_vertexActor->VisibilityOn();
		m_parent->actorCollection()->AddItem(m_vertexActor);
	} else {
		m_vertexActor->VisibilityOff();
		m_parent->actorCollection()->RemoveItem(m_vertexActor);
	}
}

void GridCreatingConditionCompoundChannelAbstractLine::reverseDirection()
{
	QVector<QPointF> points = polyLine();
	QVector<QPointF> revPoints;
	for (int i = points.count() - 1; i >= 0; --i) {
		revPoints.append(points.at(i));
	}
	setPolyLine(revPoints);
}
