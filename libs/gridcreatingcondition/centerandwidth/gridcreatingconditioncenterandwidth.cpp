#include "gridcreatingconditioncenterandwidth.h"
#include "gridcreatingconditioncenterandwidthdialog.h"
#include <misc/zdepthrange.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridrelatedconditionrealnodecontainer.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <misc/iricundostack.h>
#include <guicore/misc/qundocommandhelper.h>
#include "gridcreatingconditioncenterandwidthcoordinateseditdialog.h"
#include <misc/informationdialog.h>

#include <vtkProperty.h>
#include <vtkVertex.h>
#include <vtkLine.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>
#include <vtkActor2DCollection.h>
#include <vtkRenderWindow.h>
#include <vtkCardinalSpline.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkCollectionIterator.h>
#include <vtkTextProperty.h>

#include <QMenu>
#include <QFile>
#include <QMouseEvent>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QToolBar>
#include <QUndoCommand>
#define LABEL "label"

GridCreatingConditionCenterAndWidth::GridCreatingConditionCenterAndWidth(ProjectDataItem* parent, GridCreatingConditionCreator* creator)
	: GridCreatingCondition(parent, creator)
{
	m_vtkPolyLine = vtkSmartPointer<vtkPolyLine>::New();
	// setup grid.
	m_edgeGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_vertexGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_labelArray = vtkSmartPointer<vtkStringArray>::New();
	m_labelArray->SetName(LABEL);

	m_addVertexAction = new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), tr("&Add Vertex"), this);
	m_addVertexAction->setCheckable(true);
	connect(m_addVertexAction, SIGNAL(triggered(bool)), this, SLOT(addVertexMode(bool)));
	m_removeVertexAction = new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), tr("&Remove Vertex"), this);
	m_removeVertexAction->setCheckable(true);
	connect(m_removeVertexAction, SIGNAL(triggered(bool)), this, SLOT(removeVertexMode(bool)));

	m_coordEditAction = new QAction(tr("Edit C&oordinates..."), this);
	connect(m_coordEditAction, SIGNAL(triggered()), this, SLOT(editCoordinates()));

	m_reverseCenterLineAction = new QAction(tr("R&everse Center Line Direction"), this);
	connect(m_reverseCenterLineAction, SIGNAL(triggered()), this, SLOT(reverseCenterLineDirection()));

	m_xSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_ySpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_zSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_spline = vtkSmartPointer<vtkParametricSpline>::New();
	m_spline->SetXSpline(m_xSpline);
	m_spline->SetYSpline(m_ySpline);
	m_spline->SetZSpline(m_zSpline);
	m_splinePoints = vtkPoints::New();
	m_splinePoints->SetDataTypeToDouble();

	// Set cursors for mouse view change events.
	m_addPixmap = QPixmap(":/libs/guibase/images/cursorAdd.png");
	m_removePixmap = QPixmap(":/libs/guibase/images/cursorRemove.png");
	m_addCursor = QCursor(m_addPixmap, 0, 0);
	m_removeCursor = QCursor(m_removePixmap, 0, 0);

	m_mouseEventMode = meBeforeDefining;

	m_selectedVertexId = 0;
	m_selectedEdgeId = 0;

	m_iMax = 11;
	m_jMax = 11;
	m_width = 10.0;

	m_isAccepted = false;
	m_isGridCreated = false;

	updateActionStatus();
	graphicsView()->ResetCameraClippingRange();
}

GridCreatingConditionCenterAndWidth::~GridCreatingConditionCenterAndWidth()
{
	renderer()->RemoveActor(m_tmpActor);
	renderer()->RemoveActor(m_vertexActor);
	renderer()->RemoveActor(m_edgeActor);
	renderer()->RemoveActor2D(m_labelActor);

	delete m_rightClickingMenu;
}

const QVector<QPointF> GridCreatingConditionCenterAndWidth::polyLine()
{
	QVector<QPointF> ret;
	vtkIdList* idlist = m_vtkPolyLine->GetPointIds();
	vtkPoints* points = m_vtkPolyLine->GetPoints();
	int vCount = idlist->GetNumberOfIds();
	for (int i = 0; i < vCount; ++i){
		vtkIdType id = idlist->GetId(i);
		double* p = points->GetPoint(id);
		ret << QPointF(*p, *(p + 1));
	}
	return ret;
}

void GridCreatingConditionCenterAndWidth::setPolyLine(const QVector<QPointF> &polyline)
{
	m_vtkPolyLine = vtkSmartPointer<vtkPolyLine>::New();
	vtkPoints* points = m_vtkPolyLine->GetPoints();
	points->SetNumberOfPoints(polyline.count());
	for (int i = 0; i < polyline.count(); ++i){
		QPointF point = polyline.at(i);
		points->SetPoint(i, point.x(), point.y(), 0);
	}
	points->Modified();
	updateShapeData();
}


void GridCreatingConditionCenterAndWidth::updateShapeData()
{
	vtkIdList* idlist = m_vtkPolyLine->GetPointIds();
	vtkPoints* points = m_vtkPolyLine->GetPoints();

	vtkIdType pointCount = points->GetNumberOfPoints();
	idlist->SetNumberOfIds(pointCount);

	// update idlist.
	for (vtkIdType i = 0; i < pointCount; ++i){
		idlist->SetId(i, i);
	}

	// edge grid is constructed.
	m_edgeGrid->Reset();
	vtkIdType edgeId = 0;
	int edgeCount = m_vtkPolyLine->GetNumberOfPoints() - 1;
	m_edgeGrid->Allocate(edgeCount);
	vtkLine* nextEdge = vtkLine::New();
	for (int i = 0; i < edgeCount; ++i){
		nextEdge->GetPointIds()->SetId(0, edgeId);
		nextEdge->GetPointIds()->SetId(1, edgeId + 1);
		m_edgeGrid->InsertNextCell(nextEdge->GetCellType(), nextEdge->GetPointIds());
		edgeId += 1;
	}
	nextEdge->Delete();
	m_edgeGrid->SetPoints(m_vtkPolyLine->GetPoints());
	m_edgeGrid->BuildLinks();
	m_edgeGrid->Modified();

	// labels are constructed;
	m_labelArray->Reset();
	int vertexCount = m_vtkPolyLine->GetNumberOfPoints();
	if (vertexCount > 0){
		m_labelArray->InsertNextValue("  Upstream");
	}
	for (int i = 1; i < vertexCount; ++i){
		if (i == vertexCount - 1){
			m_labelArray->InsertNextValue("  Downstream");
		} else {
			m_labelArray->InsertNextValue("");
		}
	}
	// points are constructed.
	m_vertexGrid->Reset();
	vtkIdType vertexId = 0;
	m_vertexGrid->Allocate(vertexCount);
	vtkVertex* nextVertex = vtkVertex::New();
	for (int i = 0; i < vertexCount; ++i){
		nextVertex->GetPointIds()->SetId(0, vertexId);
		m_vertexGrid->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		vertexId += 1;
	}
	nextVertex->Delete();
	m_vertexGrid->SetPoints(m_vtkPolyLine->GetPoints());
	m_vertexGrid->GetPointData()->AddArray(m_labelArray);
	m_vertexGrid->Modified();
}

bool GridCreatingConditionCenterAndWidth::create(QWidget* parent)
{
	if (m_mouseEventMode == meBeforeDefining || m_mouseEventMode == meDefining){
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid creating condition definition not finished yet."));
		return false;
	}
	if (polyLine().count() < 2){
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid center line is not defined yet. Please click in the main region to define the grid center line."));
		return false;
	}

	createSpline(m_vtkPolyLine->GetPoints(), m_iMax);
	showDialog(parent);
	if (! m_isAccepted){return false;}

	Grid* grid = createGrid();
	if (grid == 0){return false;}
	m_isGridCreated = true;

	emit gridCreated(grid);
	return true;
}

Grid* GridCreatingConditionCenterAndWidth::createGrid()
{
	if (m_iMax * m_jMax > MAXGRIDSIZE){
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The maximum number of grid nodes is %1.").arg(MAXGRIDSIZE));
		return 0;
	}
	createSpline(m_vtkPolyLine->GetPoints(), m_iMax - 1);

	Structured2DGrid* grid = new Structured2DGrid(0);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridRelatedConditions(grid);

	grid->setDimensions(m_iMax, m_jMax);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate(m_iMax * m_jMax);

	for (int j = 0; j < m_jMax; j++){
		for (int i = 0; i < m_iMax; i++){
			double p1[3], p2[3];
			if (i == 0){
				m_splinePoints->GetPoint(i, p1);
				m_splinePoints->GetPoint(i + 1, p2);
			}else if (i == m_iMax - 1){
				m_splinePoints->GetPoint(i, p1);
				m_splinePoints->GetPoint(i - 1, p2);
			}else{
				m_splinePoints->GetPoint(i - 1, p1);
				m_splinePoints->GetPoint(i + 1, p2);
			}
			double dx = p2[0] - p1[0];
			double dy = p2[1] - p1[1];
			double s = sqrt(dx * dx + dy * dy);
			dx = (m_width / (m_jMax - 1)) * dx / s;
			dy = (m_width / (m_jMax - 1)) * dy / s;
			int center = (m_jMax - 1) / 2;
			if (i == 0){
				points->InsertPoint(m_iMax * j + i, p1[0] + ((- 1) * center + j) * (- dy), p1[1] + ((- 1) * center + j) * dx, 0.0);
			}else if (i == m_iMax - 1){
				points->InsertPoint(m_iMax * j + i, p1[0] + (center - j) * (- dy), p1[1] + (center - j) * dx, 0.0);
			}else{
				m_splinePoints->GetPoint(i, p1);
				points->InsertPoint(m_iMax * j + i, p1[0] + ((- 1) * center + j) * (- dy), p1[1] + ((- 1) * center + j) * dx, 0.0);
			}
		}
	}
	grid->vtkGrid()->SetPoints(points);
	points->Delete();

	// allocate memory for all grid related conditions.
	QList<GridRelatedConditionContainer*>::iterator it;
	QList<GridRelatedConditionContainer*>& clist = grid->gridRelatedConditions();
	for (it = clist.begin(); it != clist.end(); ++it){
		(*it)->allocate();
	}
	grid->setModified();
	return grid;
}

void GridCreatingConditionCenterAndWidth::showDialog(QWidget *parent)
{
	GridCreatingConditionCenterAndWidthDialog* dialog = new GridCreatingConditionCenterAndWidthDialog(parent);
	connect(dialog, SIGNAL(applied(QDialog*)), this, SLOT(handleDialogApplied(QDialog*)));
	dialog->setIMax(m_iMax);
	dialog->setJMax(m_jMax);
	dialog->setWidth(m_width);
	dialog->setLength(m_length);
	m_tmpIMax = m_iMax;
	m_tmpJMax = m_jMax;
	m_tmpWidth = m_width;
	dialog->setStreamWiseLabel();
	dialog->setCrossStreamLabel();

	int result = dialog->exec();
	if (result == QDialog::Accepted){
		handleDialogAccepted(dialog);
		m_isAccepted = true;
	}else{
		handleDialogRejected(dialog);
		m_isAccepted = false;
	}
	delete dialog;
}

void GridCreatingConditionCenterAndWidth::handleDialogApplied(QDialog *d)
{
	GridCreatingConditionCenterAndWidthDialog* dialog = dynamic_cast<GridCreatingConditionCenterAndWidthDialog*>(d);
	setIMax(dialog->iMax());
	setJMax(dialog->jMax());
	setWidth(dialog->width());
	createSpline(m_vertexGrid->GetPoints(), m_iMax - 1);

	Grid* g = createGrid();
	if (g == 0){return;}
	m_tmpMapper->SetInputData(g->vtkGrid());
	m_tmpActor->VisibilityOn();
	renderGraphicsView();
	m_tmpActor->VisibilityOff();
}

void GridCreatingConditionCenterAndWidth::handleDialogAccepted(QDialog *d)
{
	GridCreatingConditionCenterAndWidthDialog* dialog = dynamic_cast<GridCreatingConditionCenterAndWidthDialog*>(d);
	setIMax(dialog->iMax());
	setJMax(dialog->jMax());
	setWidth(dialog->width());
	createSpline(m_vertexGrid->GetPoints(), m_iMax - 1);
}

void GridCreatingConditionCenterAndWidth::handleDialogRejected(QDialog* /*d*/)
{
	setIMax(m_tmpIMax);
	setJMax(m_tmpJMax);
	setWidth(m_tmpWidth);
	m_splinePoints->Initialize();
	renderer()->GetRenderWindow()->Render();
}

bool GridCreatingConditionCenterAndWidth::ready() const
{
	return true;
}

void GridCreatingConditionCenterAndWidth::setupActors()
{
	if (actorCollection()->GetNumberOfItems() > 0){
		vtkCollectionIterator* it = actorCollection()->NewIterator();
		it->GoToFirstItem();
		while (! it->IsDoneWithTraversal()){
			vtkActor* actor = vtkActor::SafeDownCast(it->GetCurrentObject());
			renderer()->RemoveActor(actor);
			it->GoToNextItem();
		}
		actorCollection()->RemoveAllItems();
	}

	m_edgeActor = vtkSmartPointer<vtkActor>::New();
	m_vertexActor = vtkSmartPointer<vtkActor>::New();
	setActorProperties(m_edgeActor->GetProperty());
	setActorProperties(m_vertexActor->GetProperty());
	m_edgeActor->GetProperty()->SetLineWidth(2);
	m_vertexActor->GetProperty()->SetPointSize(5.0);
	m_labelActor = vtkSmartPointer<vtkActor2D>::New();

	renderer()->AddActor(m_edgeActor);
	renderer()->AddActor(m_vertexActor);
	renderer()->AddActor2D(m_labelActor);

	m_edgeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_vertexMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_edgeActor->SetMapper(m_edgeMapper);
	m_vertexActor->SetMapper(m_vertexMapper);

	m_edgeMapper->SetInputData(m_edgeGrid);
	m_vertexMapper->SetInputData(m_vertexGrid);

	m_tmpActor = vtkSmartPointer<vtkActor>::New();
	setActorProperties(m_tmpActor->GetProperty());
	m_tmpActor->GetProperty()->SetRepresentationToWireframe();

	m_tmpMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_tmpActor->SetMapper(m_tmpMapper);
	renderer()->AddActor(m_tmpActor);
	m_tmpActor->VisibilityOff();

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
	m_labelActor->SetMapper(m_labelMapper);
	m_labelMapper->SetInputData(m_vertexGrid);

	actorCollection()->AddItem(m_edgeActor);
	actorCollection()->AddItem(m_vertexActor);
	actor2DCollection()->AddItem(m_labelActor);

	updateShapeData();
}

void GridCreatingConditionCenterAndWidth::setupMenu()
{

	m_menu->addAction(m_addVertexAction);
	m_menu->addAction(m_removeVertexAction);
	m_menu->addAction(m_coordEditAction);
	m_menu->addAction(m_reverseCenterLineAction);

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(m_conditionDataItem->createAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addVertexAction);
	m_rightClickingMenu->addAction(m_removeVertexAction);
	m_rightClickingMenu->addAction(m_coordEditAction);
	m_rightClickingMenu->addAction(m_reverseCenterLineAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_conditionDataItem->clearAction());
}

void GridCreatingConditionCenterAndWidth::setActorProperties(vtkProperty *prop)
{
	prop->SetLighting(false);
	prop->SetColor(0, 0, 0);
}

void GridCreatingConditionCenterAndWidth::informSelection(PreProcessorGraphicsViewInterface* v)
{
	m_edgeActor->GetProperty()->SetLineWidth(selectedEdgeWidth);
	m_vertexActor->GetProperty()->SetPointSize(5.0);
	updateMouseCursor(v);
}

void GridCreatingConditionCenterAndWidth::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	m_edgeActor->GetProperty()->SetLineWidth(normalEdgeWidth);
	m_vertexActor->GetProperty()->SetPointSize(1.0);
	v->unsetCursor();
}

void GridCreatingConditionCenterAndWidth::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GridCreatingConditionCenterAndWidth::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->key() == Qt::Key_Return){
		if (m_mouseEventMode == meDefining){
			definePolyLine();
		}
	}
}

void GridCreatingConditionCenterAndWidth::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionCenterAndWidth::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (m_mouseEventMode == meDefining){
		definePolyLine();
	}
}

void GridCreatingConditionCenterAndWidth::createSpline(vtkPoints* points, int division)
{
	if (m_mouseEventMode == meBeforeDefining || m_mouseEventMode == meDefining){return;}
	if (! points->GetNumberOfPoints()) return;

	m_spline->SetPoints(points);
	m_spline->Modified();
	m_splinePoints->Initialize();

	//calculate the length of this spline
	int d = division * 5;
	// It might not work well on a too large grid
	double pre[3], dx, dy;
	double* length = new double[d + 1];
	length[0] = 0.0;

	double u[3], Pt[3], Du[9];
	u[0] = 0.0;
	m_spline->Evaluate(u, Pt, Du);
	m_splinePoints->InsertPoint(0, Pt);
	for (int i = 1; i <= d; i++){
		u[0] = i * (1.0 / d);
		m_spline->Evaluate(u, Pt, Du);
		m_splinePoints->InsertPoint(i, Pt);

		m_splinePoints->GetPoint(i - 1, pre);
		dx = Pt[0] - pre[0];
		dy = Pt[1] - pre[1];
		length[i] = length[i - 1] + sqrt(dx * dx + dy * dy);
	}
	m_length = length[d];

	// diviede the spline at equal intervals
	m_splinePoints->Initialize();
	u[0] = 0.0;
	m_spline->Evaluate(u, Pt, Du);
	m_splinePoints->InsertPoint(0, Pt);
	vtkIdType c = 1;
	double cumulativeLength = m_length / division;
	for (int i = 0; i < d; i++){
		if (length[i] > cumulativeLength){
			u[0] = (i - 1) * (1.0 / d) + (1.0 / d) * ((cumulativeLength - length[i - 1]) / (length[i] - length[i - 1]));
			m_spline->Evaluate(u, Pt, Du);
			m_splinePoints->InsertPoint(c, Pt);
			c++;
			if (c == division){
				u[0] = 1.0;
				m_spline->Evaluate(u, Pt, Du);
				m_splinePoints->InsertPoint(c, Pt);
				break;
			}
			cumulativeLength = c * m_length / division;
		}
	}
	delete length;
}


class GridCreatingConditionCenterAndWidthDefineNewPointCommand : public QUndoCommand
{
public:
	GridCreatingConditionCenterAndWidthDefineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionCenterAndWidth* cond)
		: QUndoCommand(GridCreatingConditionCenterAndWidth::tr("Add New Center Line Point"))
	{
		m_keyDown = keyDown;
		double dx = point.x();
		double dy = point.y();
		cond->graphicsView()->viewportToWorld(dx, dy);
		m_newPoint = QVector2D(dx, dy);
		m_condition = cond;
	}
	void redo()
	{
		vtkPolyLine* line = m_condition->m_vtkPolyLine;
		if (m_keyDown){
			// add new point.
			vtkIdType numOfPoints = line->GetPoints()->GetNumberOfPoints();
			if (numOfPoints == 0){
				m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meDefining;
			}
			line->GetPoints()->InsertNextPoint(m_newPoint.x(), m_newPoint.y(), 0);
			line->GetPoints()->Modified();
		}else{
			// modify the last point.
			vtkIdType lastId = line->GetNumberOfPoints() - 1;
			line->GetPoints()->SetPoint(lastId, m_newPoint.x(), m_newPoint.y(), 0);
			line->GetPoints()->Modified();
		}
		line->Modified();
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
	void undo()
	{
		vtkPolyLine* line = m_condition->m_vtkPolyLine;
		if (m_keyDown){
			// decrease the number of points. i. e. remove the last point.
			vtkIdType numOfPoints = line->GetPoints()->GetNumberOfPoints();
			if (numOfPoints == 1){
				m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meBeforeDefining;
			}
			line->GetPoints()->SetNumberOfPoints(numOfPoints - 1);
			line->GetPoints()->Modified();
		}else{
			// this does not happen. no implementation needed.
		}
		line->Modified();
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthDefineNewPoint");
	}
	bool mergeWith(const QUndoCommand *other){
		const GridCreatingConditionCenterAndWidthDefineNewPointCommand* comm = dynamic_cast<const GridCreatingConditionCenterAndWidthDefineNewPointCommand*>(other);
		if (comm == 0){return false;}
		if (comm->m_keyDown){return false;}
		if (comm->m_condition != m_condition){return false;}
		m_newPoint = comm->m_newPoint;
		return true;
	}
private:
	bool m_keyDown;
	GridCreatingConditionCenterAndWidth* m_condition;
	QVector2D m_newPoint;
};

class GridCreatingConditionCenterAndWidthMoveCommand : public QUndoCommand
{
public:
	GridCreatingConditionCenterAndWidthMoveCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionCenterAndWidth* cond)
		: QUndoCommand(GridCreatingConditionCenterAndWidth::tr("Move Center Line"))
	{
		m_keyDown = keyDown;
		double dx = from.x();
		double dy = from.y();
		cond->graphicsView()->viewportToWorld(dx, dy);
		QVector2D fromVec(dx, dy);
		dx = to.x();
		dy = to.y();
		cond->graphicsView()->viewportToWorld(dx, dy);
		QVector2D toVec(dx, dy);
		m_offset = toVec - fromVec;
		m_condition = cond;
	}
	void redo()
	{
		vtkPolyLine* line =  m_condition->m_vtkPolyLine;
		vtkPoints* points = line->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i){
			double p[3];
			points->GetPoint(i, p);
			p[0] += m_offset.x();
			p[1] += m_offset.y();
			points->SetPoint(i, p);
		}
		points->Modified();
		line->Modified();
		m_condition->updateShapeData();
		if (m_condition->m_isGridCreated){
			m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
			emit m_condition->tmpGridCreated(m_condition->createGrid());
		}
		m_condition->renderGraphicsView();
	}
	void undo()
	{
		vtkPolyLine* line =  m_condition->m_vtkPolyLine;
		vtkPoints* points = line->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i){
			double p[3];
			points->GetPoint(i, p);
			p[0] -= m_offset.x();
			p[1] -= m_offset.y();
			points->SetPoint(i, p);
		}
		points->Modified();
		line->Modified();
		m_condition->updateShapeData();
		if (m_condition->m_isGridCreated){
			m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
			emit m_condition->tmpGridCreated(m_condition->createGrid());
		}
		m_condition->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthMove");
	}
	bool mergeWith(const QUndoCommand *other){
		const GridCreatingConditionCenterAndWidthMoveCommand* comm = dynamic_cast<const GridCreatingConditionCenterAndWidthMoveCommand*>(other);
		if (comm == 0){return false;}
		if (comm->m_keyDown){return false;}
		if (comm->m_condition != m_condition){return false;}
		m_offset += comm->m_offset;
		return true;
	}
private:
	bool m_keyDown;
	GridCreatingConditionCenterAndWidth* m_condition;
	QVector2D m_offset;
};

class GridCreatingConditionCenterAndWidthMoveVertexCommand : public QUndoCommand
{
public:
	GridCreatingConditionCenterAndWidthMoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond)
		: QUndoCommand(GridCreatingConditionCenterAndWidth::tr("Move Center Line Vertex"))
	{
		m_keyDown = keyDown;
		m_vertexId = vertexId;
		double dx = from.x();
		double dy = from.y();
		cond->graphicsView()->viewportToWorld(dx, dy);
		QVector2D fromVec(dx, dy);
		dx = to.x();
		dy = to.y();
		cond->graphicsView()->viewportToWorld(dx, dy);
		QVector2D toVec(dx, dy);
		m_offset = toVec - fromVec;
		m_condition = cond;
	}
	void redo()
	{
		vtkPolyLine* line = m_condition->m_vtkPolyLine;
		vtkPoints* points = line->GetPoints();
		double p[3];
		points->GetPoint(m_vertexId, p);
		p[0] += m_offset.x();
		p[1] += m_offset.y();
		points->SetPoint(m_vertexId, p);

		points->Modified();
		line->Modified();
		m_condition->updateShapeData();
		if (m_condition->m_isGridCreated){
			m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
			emit m_condition->tmpGridCreated(m_condition->createGrid());
		}
		m_condition->renderGraphicsView();
	}
	void undo()
	{
		vtkPolyLine* line = m_condition->m_vtkPolyLine;
		vtkPoints* points = line->GetPoints();
		double p[3];
		points->GetPoint(m_vertexId, p);
		p[0] -= m_offset.x();
		p[1] -= m_offset.y();
		points->SetPoint(m_vertexId, p);

		points->Modified();
		line->Modified();
		m_condition->updateShapeData();
		if (m_condition->m_isGridCreated){
			m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
			emit m_condition->tmpGridCreated(m_condition->createGrid());
		}
		m_condition->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthMoveVertex");
	}
	bool mergeWith(const QUndoCommand *other){
		const GridCreatingConditionCenterAndWidthMoveVertexCommand* comm = dynamic_cast<const GridCreatingConditionCenterAndWidthMoveVertexCommand*>(other);
		if (comm == 0){return false;}
		if (comm->m_keyDown){return false;}
		if (comm->m_condition != m_condition){return false;}
		if (comm->m_vertexId != m_vertexId){return false;}
		m_offset += comm->m_offset;
		return true;
	}
private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	GridCreatingConditionCenterAndWidth* m_condition;
	QVector2D m_offset;
};

class GridCreatingConditionCenterAndWidthAddVertexCommand : public QUndoCommand
{
public:
	GridCreatingConditionCenterAndWidthAddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCenterAndWidth* cond)
		: QUndoCommand(GridCreatingConditionCenterAndWidth::tr("Insert Center Line Vertex"))
	{
		m_keyDown = keyDown;
		m_vertexId = (edgeId + 1) % cond->m_vtkPolyLine->GetNumberOfPoints();
		double dx = point.x();
		double dy = point.y();
		cond->graphicsView()->viewportToWorld(dx, dy);
		m_vertexPosition = QVector2D(dx, dy);
		m_condition = cond;
	}
	void redo()
	{
		if (m_keyDown){
			// add vertex.
			vtkPoints* points = m_condition->m_vtkPolyLine->GetPoints();
			QVector<QVector2D> positions;
			positions.reserve(points->GetNumberOfPoints());
			double p[3];
			for (vtkIdType i = 0; i < m_vertexId; ++i){
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			positions.append(m_vertexPosition);
			for (vtkIdType i = m_vertexId; i < points->GetNumberOfPoints(); ++i){
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			points->SetNumberOfPoints(positions.count());
			for (vtkIdType i = 0; i < positions.count(); ++i){
				QVector2D v = positions.at(i);
				points->SetPoint(i, v.x(), v.y(), 0);
			}
			points->Modified();
		} else {
			// just modify the vertex position
			vtkPoints* points = m_condition->m_vtkPolyLine->GetPoints();
			points->SetPoint(m_vertexId, m_vertexPosition.x(), m_vertexPosition.y(), 0);
			points->Modified();
		}
		m_condition->m_vtkPolyLine->Modified();
		m_condition->updateShapeData();
		if (m_condition->m_isGridCreated){
			m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
			emit m_condition->tmpGridCreated(m_condition->createGrid());
		}
		m_condition->renderGraphicsView();
	}
	void undo()
	{
		if (m_keyDown){
			// remove vertex.
			vtkPoints* points = m_condition->m_vtkPolyLine->GetPoints();
			QVector<QVector2D> positions;
			positions.reserve(points->GetNumberOfPoints());
			double p[3];
			for (vtkIdType i = 0; i < m_vertexId; ++i){
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			// skip vertex in m_vertexId[
			for (vtkIdType i = m_vertexId + 1; i < points->GetNumberOfPoints(); ++i){
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			points->SetNumberOfPoints(positions.count());
			for (vtkIdType i = 0; i < positions.count(); ++i){
				QVector2D v = positions.at(i);
				points->SetPoint(i, v.x(), v.y(), 0);
			}
			points->Modified();
			m_condition->m_vtkPolyLine->Modified();
			m_condition->updateShapeData();
			if (m_condition->m_isGridCreated){
				m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
				emit m_condition->tmpGridCreated(m_condition->createGrid());
			}
			m_condition->renderGraphicsView();
		}else{
			// this never happens.
		}
	}
	int id() const {
		return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthAddVertex");
	}
	bool mergeWith(const QUndoCommand *other)
	{
		const GridCreatingConditionCenterAndWidthAddVertexCommand* comm = dynamic_cast<const GridCreatingConditionCenterAndWidthAddVertexCommand*>(other);
		if (comm == 0){return false;}
		if (comm->m_keyDown){return false;}
		if (m_condition != comm->m_condition){return false;}
		if (m_vertexId != comm->m_vertexId){return false;}
		m_vertexPosition = comm->m_vertexPosition;
		return true;
	}
private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionCenterAndWidth* m_condition;
};

void GridCreatingConditionCenterAndWidth::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode){
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		m_currentPoint = QPoint(event->x(), event->y());
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case meBeforeDefining:
		// do nothing.
		break;
	case meDefining:
		// update the position of the last point.
		iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthDefineNewPointCommand(false, QPoint(event->x(), event->y()), this));
		break;
	case meTranslate:
		// execute translation.
		iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthMoveCommand(false, m_currentPoint, QPoint(event->x(), event->y()), this));
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meMoveVertex:
		iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthMoveVertexCommand(false, m_currentPoint, QPoint(event->x(), event->y()), m_selectedVertexId, this));
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meAddVertex:
		iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthAddVertexCommand(false, m_selectedEdgeId, QPoint(event->x(), event->y()), this));
		break;
	case meEditVerticesDialog:
		break;
	}
}


class GridCreatingConditionCenterAndWidthRemoveVertexCommand : public QUndoCommand
{
public:
	GridCreatingConditionCenterAndWidthRemoveVertexCommand(vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond)
		: QUndoCommand(GridCreatingConditionCenterAndWidth::tr("Remove Center Line Vertex"))
	{
		m_vertexId = vertexId;
		double p[3];
		cond->m_vtkPolyLine->GetPoints()->GetPoint(m_vertexId, p);
		m_vertexPosition = QVector2D(p[0], p[1]);
		m_condition = cond;
	}
	void redo()
	{
		vtkPoints* points = m_condition->m_vtkPolyLine->GetPoints();
		QVector<QVector2D> positions;
		positions.reserve(points->GetNumberOfPoints());
		double p[3];
		for (vtkIdType i = 0; i < m_vertexId; ++i){
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		// skip vertex in m_vertexId
		for (vtkIdType i = m_vertexId + 1; i < points->GetNumberOfPoints(); ++i){
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		points->SetNumberOfPoints(positions.count());
		for (vtkIdType i = 0; i < positions.count(); ++i){
			QVector2D v = positions.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meNormal;
		m_condition->m_vtkPolyLine->Modified();
		m_condition->updateShapeData();
		if (m_condition->m_isGridCreated){
			m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
			emit m_condition->gridCreated(m_condition->createGrid());
		}
		m_condition->renderGraphicsView();
	}
	void undo()
	{
		vtkPoints* points = m_condition->m_vtkPolyLine->GetPoints();
		QVector<QVector2D> positions;
		positions.reserve(points->GetNumberOfPoints());
		double p[3];
		for (vtkIdType i = 0; i < m_vertexId; ++i){
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		positions.append(m_vertexPosition);
		for (vtkIdType i = m_vertexId; i < points->GetNumberOfPoints(); ++i){
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		points->SetNumberOfPoints(positions.count());
		for (vtkIdType i = 0; i < positions.count(); ++i){
			QVector2D v = positions.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_condition->m_vtkPolyLine->Modified();
		m_condition->updateShapeData();
		if (m_condition->m_isGridCreated){
			m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
			emit m_condition->gridCreated(m_condition->createGrid());
		}
		m_condition->renderGraphicsView();
	}
private:
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionCenterAndWidth* m_condition;
};

void GridCreatingConditionCenterAndWidth::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton){
		// left click
		double worldX = static_cast<double>(event->x());
		double worldY = static_cast<double>(event->y());
		v->viewportToWorld(worldX, worldY);

		switch (m_mouseEventMode){
		case meNormal:
			// do nothing.
			break;
		case meBeforeDefining:
			// enter defining mode.
			m_mouseEventMode = meDefining;
			iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthDefineNewPointCommand(true, QPoint(event->x(), event->y()), this));
		case meDefining:
			iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthDefineNewPointCommand(true, QPoint(event->x(), event->y()), this));
			break;
		case meTranslatePrepare:
			m_mouseEventMode = meTranslate;
			m_currentPoint = QPoint(event->x(), event->y());
			// push the first translation command.
			iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthMoveCommand(true, m_currentPoint, m_currentPoint, this));
			break;
		case meMoveVertexPrepare:
			m_mouseEventMode = meMoveVertex;
			m_currentPoint = QPoint(event->x(), event->y());
			// push the first move command.
			iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthMoveVertexCommand(true, m_currentPoint, m_currentPoint, m_selectedVertexId, this));
			break;
		case meAddVertexPrepare:
			m_mouseEventMode = meAddVertex;
			iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthAddVertexCommand(true, m_selectedEdgeId, QPoint(event->x(), event->y()), this));
			break;
		case meAddVertexNotPossible:
			// do nothing.
			break;
		case meRemoveVertexPrepare:
			iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthRemoveVertexCommand(m_selectedVertexId, this));
			break;
		case meRemoveVertexNotPossible:
			// do nothing.
			break;
		case meTranslate:
			// this should not happen.
			break;
		case meMoveVertex:
			// this should not happen.
			break;
		case meAddVertex:
			// this should not happen.
			break;
		case meEditVerticesDialog:
			break;
		}
		updateMouseCursor(v);
		updateActionStatus();
	} else if (event->button() == Qt::RightButton){
		// right click
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GridCreatingConditionCenterAndWidth::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton){
		switch (m_mouseEventMode){
		case meTranslate:
		case meMoveVertex:
		case meAddVertex:
			if (m_isGridCreated) emit gridCreated(createGrid());
		case meNormal:
		case meTranslatePrepare:
		case meMoveVertexPrepare:
		case meAddVertexPrepare:
		case meAddVertexNotPossible:
		case meRemoveVertexPrepare:
		case meRemoveVertexNotPossible:
			m_currentPoint = QPoint(event->x(), event->y());
			updateMouseEventMode();
			updateMouseCursor(v);
			updateActionStatus();
			break;
		case meDefining:
			// do nothing no mode change.
			break;
		case meEditVerticesDialog:
			break;
		}
		updateMouseCursor(v);
	}else if (event->button() == Qt::RightButton){
		if (m_mouseEventMode == meEditVerticesDialog){return;}
		if (isNear(m_dragStartPoint, QPoint(event->x(), event->y()))){
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionCenterAndWidth::deletePolyLine()
{
	if (m_mouseEventMode == meBeforeDefining || m_mouseEventMode == meDefining)
	m_vertexGrid->Initialize();
	m_edgeGrid->Initialize();
	m_vtkPolyLine->GetPointIds()->Initialize();
	m_vtkPolyLine->GetPoints()->Initialize();
	m_vtkPolyLine->Modified();

	m_polyLine.clear();
	ZDepthRange range = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent())->zDepthRange();
	assignActionZValues(range);
	m_mouseEventMode = meBeforeDefining;
	updateMouseCursor(graphicsView());
	m_iMax = 11;
	m_jMax = 11;
	m_width = 10;
	m_length = 0;
	updateShapeData();
	renderer()->GetRenderWindow()->Render();
	// this operation is not undo-able.
	iRICUndoStack::instance().clear();
}

bool GridCreatingConditionCenterAndWidth::isVertexSelectable(const QVector2D& pos)
{
	if (m_selectedVertexId == -1){return false;}
	m_selectedVertexId = m_edgeGrid->FindPoint(pos.x(), pos.y(), 0.0);
	QVector<QPointF> p = polyLine();
	PreProcessorGraphicsViewInterface* v = graphicsView();
	QPointF point = p.at(m_selectedVertexId);
	QVector2D vertexPos = QVector2D(point.x(), point.y());
	double limitdist = v->stdRadius(5);
	return ((vertexPos - pos).length() < limitdist);
}

bool GridCreatingConditionCenterAndWidth::isEdgeSelectable(const QVector2D& pos)
{
	double x[3] = {pos.x(), pos.y(), 0.0};
	int subId;
	double pcoords[3];
	double weights[32];
	double d = graphicsView()->stdRadius(5);
	d = d * d;
	vtkIdType id = m_edgeGrid->FindCell(x, NULL, 0, d, subId, pcoords, weights);
	if (id >= 0){
		m_selectedEdgeId = id;
		return true;
	}
	return false;
}

class GridCreatingConditionCenterAndWidthFinishDefiningCommand : public QUndoCommand
{
public:
	GridCreatingConditionCenterAndWidthFinishDefiningCommand(GridCreatingConditionCenterAndWidth* condition)
		: QUndoCommand(GridCreatingConditionCenterAndWidth::tr("Finish Defining Polyline"))
	{
		m_condition = condition;
	}
	void undo()
	{
		m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meDefining;
		m_condition->updateMouseCursor(m_condition->graphicsView());
		m_condition->updateActionStatus();
	}
	void redo()
	{
		m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meNormal;
		m_condition->updateMouseCursor(m_condition->graphicsView());
		m_condition->updateActionStatus();
		m_condition->create(m_condition->preProcessorWindow());
	}
private:
	GridCreatingConditionCenterAndWidth* m_condition;
};

void GridCreatingConditionCenterAndWidth::definePolyLine()
{
	if (m_vtkPolyLine->GetPoints()->GetNumberOfPoints() < 2){
		// not enough points defined!
		return;
	}
	iRICUndoStack::instance().undo();
	// finish defining the Polyline.
	iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidthFinishDefiningCommand(this));
}

void GridCreatingConditionCenterAndWidth::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode){
	case meNormal:
	case meAddVertexNotPossible:
	case meRemoveVertexNotPossible:
	case meEditVerticesDialog:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meBeforeDefining:
	case meDefining:
		v->setCursor(Qt::CrossCursor);
		break;
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case meAddVertexPrepare:
		v->setCursor(m_addCursor);
		break;
	case meRemoveVertexPrepare:
		v->setCursor(m_removeCursor);
		break;
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	}
}

void GridCreatingConditionCenterAndWidth::doLoadFromProjectMainFile(const QDomNode& node){
	QDomNode centerNode = iRIC::getChildNode(node, "CenterAndWidth");
	if (! centerNode.isNull()){
		loadCenterAndWidthFromProjectMainFile(centerNode);
	}
}

void GridCreatingConditionCenterAndWidth::doSaveToProjectMainFile(QXmlStreamWriter& writer){
	writer.writeStartElement("CenterAndWidth");
	saveCenterAndWidthToProjectMainFile(writer);
	writer.writeEndElement();
}

void GridCreatingConditionCenterAndWidth::loadCenterAndWidthFromProjectMainFile(const QDomNode & node)
{
	m_iMax = node.toElement().attribute("imax", "11").toInt();
	m_jMax = node.toElement().attribute("jmax", "11").toInt();
	m_width = node.toElement().attribute("width", "10.0").toDouble();
	m_length = node.toElement().attribute("length", "0").toDouble();
}

void GridCreatingConditionCenterAndWidth::saveCenterAndWidthToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("imax", qstr.setNum(m_iMax));
	writer.writeAttribute("jmax", qstr.setNum(m_jMax));
	writer.writeAttribute("width", qstr.setNum(m_width));
	writer.writeAttribute("length", qstr.setNum(m_length));
}

void GridCreatingConditionCenterAndWidth::loadExternalData(const QString& filename){
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);
	QVector<QPointF> v;
	while (! s.atEnd()){
		double a, b;
		s >> a >> b;
		v.append(QPointF(a, b));
	}
	f.close();
	setPolyLine(v);
	if (polyLine().size() > 0){
		m_mouseEventMode = meNormal;
		m_isGridCreated = true;
	}else{
		m_mouseEventMode = meBeforeDefining;
	}
}

void GridCreatingConditionCenterAndWidth::saveExternalData(const QString& filename){
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);
	QVector<QPointF> v = polyLine();
	int vSize = v.size();
	for (int i = 0; i < vSize; i++){
		s << v.at(i).x();
		s << v.at(i).y();
	}
	f.close();
}

void GridCreatingConditionCenterAndWidth::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GridCreatingConditionCenterAndWidth::assignActionZValues(const ZDepthRange& range)
{
	m_vertexActor->SetPosition(0, 0, range.max());
	m_edgeActor->SetPosition(0, 0, range.max());
	m_tmpActor->SetPosition(0, 0, range.max());
}

void GridCreatingConditionCenterAndWidth::setIMax(int i)
{
	m_iMax = i;
}

void GridCreatingConditionCenterAndWidth::setJMax(int j)
{
	m_jMax = j;
}

void GridCreatingConditionCenterAndWidth::setWidth(double w)
{
	m_width = w;
}

bool GridCreatingConditionCenterAndWidth::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(m_addVertexAction);
	tb->addAction(m_removeVertexAction);

	return true;
}

void GridCreatingConditionCenterAndWidth::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

void GridCreatingConditionCenterAndWidth::updateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QVector2D worldPos(dx, dy);
	switch (m_mouseEventMode){
	case meAddVertexNotPossible:
	case meAddVertexPrepare:
		if (isEdgeSelectable(worldPos)){
			m_mouseEventMode = meAddVertexPrepare;
		}else{
			m_mouseEventMode = meAddVertexNotPossible;
		}
		break;
	case meRemoveVertexNotPossible:
	case meRemoveVertexPrepare:
		if (isVertexSelectable(worldPos)){
			m_mouseEventMode = meRemoveVertexPrepare;
		}else{
			m_mouseEventMode = meRemoveVertexNotPossible;
		}
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		if (isVertexSelectable(worldPos)){
			m_mouseEventMode = meMoveVertexPrepare;
		}else if (isEdgeSelectable(worldPos)){
			m_mouseEventMode = meTranslatePrepare;
		}else{
			m_mouseEventMode = meNormal;
		}
		break;
	case meBeforeDefining:
	case meDefining:
		// do nothing
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCenterAndWidth::updateActionStatus()
{
	switch (m_mouseEventMode){
	case meBeforeDefining:
	case meDefining:
	case meTranslate:
	case meMoveVertex:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(m_vtkPolyLine->GetPoints()->GetNumberOfPoints() > 2);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(true);
		break;
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meAddVertex:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(true);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		break;
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(true);
		m_coordEditAction->setEnabled(false);
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCenterAndWidth::addVertexMode(bool on)
{
	if (on){
		m_mouseEventMode = meAddVertexNotPossible;
	}else{
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GridCreatingConditionCenterAndWidth::removeVertexMode(bool on)
{
	if (on){
		m_mouseEventMode = meRemoveVertexNotPossible;
	}else{
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GridCreatingConditionCenterAndWidth::editCoordinates()
{
	m_mouseEventMode = meEditVerticesDialog;
	GridCreatingConditionCenterAndWidthCoordinatesEditDialog* dialog = new GridCreatingConditionCenterAndWidthCoordinatesEditDialog(this, preProcessorWindow());
	dialog->show();
	iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	connect(dialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
}

void GridCreatingConditionCenterAndWidth::clear()
{
	deletePolyLine();
}

void GridCreatingConditionCenterAndWidth::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GridCreatingConditionCenterAndWidth::tr("Information"), GridCreatingConditionCenterAndWidth::tr("Please define grid center line. Grid center line can be defined as polygonal line by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gccenterandwidth_centerline");
}

void GridCreatingConditionCenterAndWidth::reverseCenterLineDirection()
{
	QVector<QPointF> points = polyLine();
	QVector<QPointF> revPoints;
	for (int i = points.count() - 1; i >= 0; --i){
		revPoints.append(points.at(i));
	}
	setPolyLine(revPoints);
	renderGraphicsView();
}

void GridCreatingConditionCenterAndWidth::doApplyOffset(double x, double y)
{
	QVector<QPointF> polyline = polyLine();
	for (int i = 0; i < polyline.size(); ++ i){
		QPointF p = polyline.at(i);
		p.setX(p.x() - x);
		p.setY(p.y() - y);
		polyline[i] = p;
	}
	setPolyLine(polyline);
}
