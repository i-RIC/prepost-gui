#include "gridcreatingconditiontriangle.h"
#include "gridcreatingconditiontriangleabstractpolygon.h"
#include "gridcreatingconditiontriangledivisionline.h"
#include "gridcreatingconditiontrianglegridregionpolygon.h"
#include "gridcreatingconditiontriangleholepolygon.h"
#include "gridcreatingconditiontrianglepolygoncoordinateseditdialog.h"
#include "gridcreatingconditiontrianglepolylinecoordinateseditdialog.h"
#include "gridcreatingconditiontriangleremeshpolygon.h"
#include "gridcreatingconditiontrianglesettingdialog.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/errormessage.h>
#include <misc/iricrootpath.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>
#include <triangle/triangleexecutethread.h>
#include <triangle/triangle.h>

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPolygonF>
#include <QTextStream>
#include <QToolBar>
#include <QUndoCommand>
#include <QVector2D>
#include <QVector>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

GridCreatingConditionTriangle::GridCreatingConditionTriangle(ProjectDataItem* parent, GridCreatingConditionCreator* creator) :
	GridCreatingCondition {parent, creator}
{
	initParams();

	m_gridRegionPolygon = new GridCreatingConditionTriangleGridRegionPolygon(this);
	m_gridRegionPolygon->setActive(true);
	m_selectMode = smPolygon;
	m_selectedPolygon = m_gridRegionPolygon;
	m_selectedLine = nullptr;

	m_defineModeAction = new QAction(QIcon(":/libs/guibase/images/iconPolygonRegion.png"), GridCreatingConditionTriangle::tr("Define Grid Region"), this);
	m_defineModeAction->setCheckable(true);
	m_defineModeAction->setDisabled(true);
	m_refineModeAction = new QAction(QIcon(":/libs/guibase/images/iconPolygonRefine.png"), GridCreatingConditionTriangle::tr("Add &Refinement Region"), this);
	m_refineModeAction->setCheckable(true);
	m_refineModeAction->setDisabled(true);
	connect(m_refineModeAction, SIGNAL(triggered()), this, SLOT(addRefinementPolygon()));
	m_holeModeAction = new QAction(QIcon(":/libs/guibase/images/iconPolygonHole.png"), GridCreatingConditionTriangle::tr("Add &Hole Region"), this);
	m_holeModeAction->setCheckable(true);
	m_holeModeAction->setDisabled(true);
	connect(m_holeModeAction, SIGNAL(triggered()), this, SLOT(addHolePolygon()));
	m_divlineModeAction = new QAction(QIcon(":/libs/guibase/images/iconAddDivLine.png"), GridCreatingConditionTriangle::tr("Add &Break Line"), this);
	m_divlineModeAction->setCheckable(true);
	m_divlineModeAction->setDisabled(true);
	connect(m_divlineModeAction, SIGNAL(triggered()), this, SLOT(addDivisionLine()));

	m_addVertexAction = new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), GridCreatingConditionTriangle::tr("&Add Vertex"), this);
	m_addVertexAction->setCheckable(true);
	connect(m_addVertexAction, SIGNAL(triggered(bool)), this, SLOT(addVertexMode(bool)));
	m_removeVertexAction = new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), GridCreatingConditionTriangle::tr("&Remove Vertex"), this);
	m_removeVertexAction->setCheckable(true);
	connect(m_removeVertexAction, SIGNAL(triggered(bool)), this, SLOT(removeVertexMode(bool)));
	m_coordEditAction = new QAction(GridCreatingConditionTriangle::tr("Edit &Coordinates..."), this);
	connect(m_coordEditAction, SIGNAL(triggered()), this, SLOT(editCoordinates()));

	m_deleteAction = new QAction(GridCreatingConditionTriangle::tr("&Delete Region or Break Line..."), this);
	m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deletePolygon()));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteLine()));

	m_editMaxAreaAction = new QAction(GridCreatingConditionTriangle::tr("Edit &Maximum Area for Cells..."), this);
	m_editMaxAreaAction->setDisabled(true);

	// Set cursors for mouse view change events.
	m_addPixmap = QPixmap(":/libs/guibase/images/cursorAdd.png");
	m_removePixmap = QPixmap(":/libs/guibase/images/cursorRemove.png");
	m_addCursor = QCursor(m_addPixmap, 0, 0);
	m_removeCursor = QCursor(m_removePixmap, 0, 0);

	m_mouseEventMode = meBeforeDefining;

	m_inhibitSelect = false;
	updateActionStatus();
}

GridCreatingConditionTriangle::~GridCreatingConditionTriangle()
{
	delete m_gridRegionPolygon;
	for (auto pol : m_remeshPolygons) {
		delete pol;
	}
	for (auto pol : m_holePolygons) {
		delete pol;
	}
	for (auto line : m_divisionLines) {
		delete line;
	}
	delete m_rightClickingMenu;
}

void GridCreatingConditionTriangle::setupMenu()
{
	m_menu->setTitle(tr("&Polygon"));
	m_menu->addAction(m_refineModeAction);
	m_menu->addAction(m_holeModeAction);
	m_menu->addAction(m_divlineModeAction);
	m_menu->addSeparator();
	m_menu->addAction(m_addVertexAction);
	m_menu->addAction(m_removeVertexAction);
	m_menu->addAction(m_coordEditAction);
	m_menu->addAction(m_editMaxAreaAction);
	m_menu->addSeparator();
	m_menu->addAction(m_deleteAction);
	m_menu->addSeparator();

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(m_refineModeAction);
	m_rightClickingMenu->addAction(m_holeModeAction);
	m_rightClickingMenu->addAction(m_divlineModeAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addVertexAction);
	m_rightClickingMenu->addAction(m_removeVertexAction);
	m_rightClickingMenu->addAction(m_coordEditAction);
	m_rightClickingMenu->addAction(m_editMaxAreaAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_deleteAction);
	m_rightClickingMenu->addSeparator();
	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	m_rightClickingMenu->addAction(p->createAction());
}

bool GridCreatingConditionTriangle::addToolBarButtons(QToolBar* tb)
{
//	tb->addAction(m_defineModeAction);
	tb->addAction(m_refineModeAction);
	tb->addAction(m_holeModeAction);
	tb->addAction(m_divlineModeAction);

	tb->addSeparator();
	tb->addAction(m_addVertexAction);
	tb->addAction(m_removeVertexAction);

	tb->addSeparator();
	tb->addAction(m_deleteAction);
	return true;
}

QColor GridCreatingConditionTriangle::doubleToColor(double /*d*/) const
{
	return Qt::red;
}

void GridCreatingConditionTriangle::setColor(double r, double g, double b)
{
	m_color = QColor((int)(r * 255), (int)(g * 255), (int)(b * 255));
}

void GridCreatingConditionTriangle::setColor(const QColor& color)
{
	setColor(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
}

void GridCreatingConditionTriangle::informSelection(PreProcessorGraphicsViewInterface* v)
{
	switch (m_selectMode) {
	case smPolygon:
		m_selectedPolygon->setActive(true);
		break;
	case smLine:
		m_selectedLine->setActive(true);
		break;
	case smNone:
		// do nothing.
		break;
	}
	updateMouseCursor(v);
}

void GridCreatingConditionTriangle::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	switch (m_selectMode) {
	case smPolygon:
		Q_ASSERT(m_selectedPolygon != nullptr);
		m_selectedPolygon->setActive(false);
		break;
	case smLine:
		Q_ASSERT(m_selectedLine != nullptr);
		m_selectedLine->setActive(false);
		break;
	case smNone:
		// do nothing.
		Q_ASSERT(m_selectedPolygon == nullptr);
		Q_ASSERT(m_selectedLine == nullptr);
		break;
	}
	v->unsetCursor();
}

void GridCreatingConditionTriangle::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

class GridCreatingConditionTriangle::FinishPolygonDefiningCommand : public QUndoCommand
{
public:
	FinishPolygonDefiningCommand(GridCreatingConditionTriangle* polygon) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Finish Defining Polygon")}
	{
		m_polygon = polygon;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void undo() {
		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meDefining;
		m_polygon->m_selectedPolygon = m_targetPolygon;
		m_polygon->m_selectedPolygon->setActive(true);
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->updateActionStatus();
	}
	void redo() {
		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
		m_targetPolygon->finishDefinition();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->updateActionStatus();
	}
private:
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
};

class GridCreatingConditionTriangle::FinishPolyLineDefiningCommand : public QUndoCommand
{
public:
	FinishPolyLineDefiningCommand(GridCreatingConditionTriangle* polygon) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Finish Defining Break Line")}
	{
		m_polygon = polygon;
		m_targetLine = m_polygon->m_selectedLine;
	}
	void undo() {
		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meDefining;
		m_polygon->m_selectMode = GridCreatingConditionTriangle::smLine;
		m_polygon->m_selectedLine = m_targetLine;
		m_polygon->m_selectedLine->setActive(true);
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->updateActionStatus();
	}
	void redo() {
		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
		m_targetLine->finishDefinition();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->updateActionStatus();
	}
private:
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
};

void GridCreatingConditionTriangle::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->key() == Qt::Key_Return) {
		if (m_mouseEventMode == meDefining) {
			if (m_selectMode == smPolygon) {
				definePolygon(false);
			} else if (m_selectMode == smLine) {
				defineLine(false);
			}
		}
	}
}

void GridCreatingConditionTriangle::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionTriangle::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (m_mouseEventMode == meDefining) {
		if (m_selectMode == smPolygon) {
			definePolygon(true);
		} else if (m_selectMode == smLine) {
			defineLine(true);
		}
	}
}

class GridCreatingConditionTriangle::DefinePolygonNewPointCommand : public QUndoCommand
{
public:
	DefinePolygonNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Add New Polygon Point")}
	{
		m_keyDown = keyDown;
		double dx = point.x();
		double dy = point.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		m_newPoint = QVector2D(dx, dy);
		m_polygon = pol;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void redo() {
		vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
		if (m_keyDown) {
			// add new point.
			pol->GetPoints()->InsertNextPoint(m_newPoint.x(), m_newPoint.y(), 0);
			pol->GetPoints()->Modified();
		} else {
			// modify the last point.
			vtkIdType lastId = pol->GetNumberOfPoints() - 1;
			pol->GetPoints()->SetPoint(lastId, m_newPoint.x(), m_newPoint.y(), 0);
			pol->GetPoints()->Modified();
		}
		pol->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
		if (m_keyDown) {
			// decrease the number of points. i. e. remove the last point.
			vtkIdType numOfPoints = pol->GetPoints()->GetNumberOfPoints();
			pol->GetPoints()->SetNumberOfPoints(numOfPoints - 1);
			pol->GetPoints()->Modified();
		} else {
			// this does not happen. no implementation needed.
		}
		pol->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("DefinePolygonNewPoint");
	}
	bool mergeWith(const QUndoCommand* other) {
		const DefinePolygonNewPointCommand* comm = dynamic_cast<const DefinePolygonNewPointCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (comm->m_polygon != m_polygon) {return false;}
		if (comm->m_targetPolygon != m_targetPolygon) {return false;}
		m_newPoint = comm->m_newPoint;
		return true;
	}
private:
	bool m_keyDown;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
	QVector2D m_newPoint;
};

class GridCreatingConditionTriangle::MovePolygonCommand : public QUndoCommand
{
public:
	MovePolygonCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Move Polygon")}
	{
		m_keyDown = keyDown;
		double dx = from.x();
		double dy = from.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		QVector2D fromVec(dx, dy);
		dx = to.x();
		dy = to.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		QVector2D toVec(dx, dy);
		m_offset = toVec - fromVec;
		m_polygon = pol;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void redo() {
		vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
		vtkPoints* points = pol->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			double p[3];
			points->GetPoint(i, p);
			p[0] += m_offset.x();
			p[1] += m_offset.y();
			points->SetPoint(i, p);
		}
		points->Modified();
		pol->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
		vtkPoints* points = pol->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			double p[3];
			points->GetPoint(i, p);
			p[0] -= m_offset.x();
			p[1] -= m_offset.y();
			points->SetPoint(i, p);
		}
		points->Modified();
		pol->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("MovePolygon");
	}
	bool mergeWith(const QUndoCommand* other) {
		const MovePolygonCommand* comm = dynamic_cast<const MovePolygonCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (comm->m_polygon != m_polygon) {return false;}
		if (comm->m_targetPolygon != m_targetPolygon) {return false;}
		m_offset += comm->m_offset;
		return true;
	}
private:
	bool m_keyDown;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
	QVector2D m_offset;
};

class GridCreatingConditionTriangle::MovePolygonVertexCommand : public QUndoCommand
{
public:
	MovePolygonVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Move Polygon Vertex")}
	{
		m_keyDown = keyDown;
		m_vertexId = vertexId;
		double dx = from.x();
		double dy = from.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		QVector2D fromVec(dx, dy);
		dx = to.x();
		dy = to.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		QVector2D toVec(dx, dy);
		m_offset = toVec - fromVec;
		m_polygon = pol;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void redo() {
		vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
		vtkPoints* points = pol->GetPoints();
		double p[3];
		points->GetPoint(m_vertexId, p);
		p[0] += m_offset.x();
		p[1] += m_offset.y();
		points->SetPoint(m_vertexId, p);

		points->Modified();
		pol->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
		vtkPoints* points = pol->GetPoints();
		double p[3];
		points->GetPoint(m_vertexId, p);
		p[0] -= m_offset.x();
		p[1] -= m_offset.y();
		points->SetPoint(m_vertexId, p);

		points->Modified();
		pol->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("MovePolygonVertex");
	}
	bool mergeWith(const QUndoCommand* other) {
		const MovePolygonVertexCommand* comm = dynamic_cast<const MovePolygonVertexCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (comm->m_polygon != m_polygon) {return false;}
		if (comm->m_targetPolygon != m_targetPolygon) {return false;}
		if (comm->m_vertexId != m_vertexId) {return false;}
		m_offset += comm->m_offset;
		return true;
	}

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
	QVector2D m_offset;
};

class GridCreatingConditionTriangle::AddPolygonVertexCommand : public QUndoCommand
{
public:
	AddPolygonVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionTriangle* pol) :
		QUndoCommand(GridCreatingConditionTriangle::tr("Insert Polygon Vertex"))
	{
		m_keyDown = keyDown;
		m_vertexId = (edgeId + 1) % (pol->m_selectedPolygon->getVtkPolygon()->GetNumberOfPoints() + 1);
		double dx = point.x();
		double dy = point.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		m_vertexPosition = QVector2D(dx, dy);
		m_polygon = pol;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void redo() {
		if (m_keyDown) {
			// add vertex.
			vtkPoints* points = m_targetPolygon->getVtkPolygon()->GetPoints();
			QVector<QVector2D> positions;
			positions.reserve(points->GetNumberOfPoints());
			double p[3];
			for (vtkIdType i = 0; i < m_vertexId; ++i) {
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			positions.append(m_vertexPosition);
			for (vtkIdType i = m_vertexId; i < points->GetNumberOfPoints(); ++i) {
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			points->SetNumberOfPoints(positions.count());
			for (vtkIdType i = 0; i < positions.count(); ++i) {
				QVector2D v = positions.at(i);
				points->SetPoint(i, v.x(), v.y(), 0);
			}
			points->Modified();
		} else {
			// just modify the vertex position
			vtkPoints* points = m_targetPolygon->getVtkPolygon()->GetPoints();
			points->SetPoint(m_vertexId, m_vertexPosition.x(), m_vertexPosition.y(), 0);
			points->Modified();
		}
		m_targetPolygon->getVtkPolygon()->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		if (m_keyDown) {
			// remove vertex.
			vtkPoints* points = m_targetPolygon->getVtkPolygon()->GetPoints();
			QVector<QVector2D> positions;
			positions.reserve(points->GetNumberOfPoints());
			double p[3];
			for (vtkIdType i = 0; i < m_vertexId; ++i) {
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			// skip vertex in m_vertexId[
			for (vtkIdType i = m_vertexId + 1; i < points->GetNumberOfPoints(); ++i) {
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			points->SetNumberOfPoints(positions.count());
			for (vtkIdType i = 0; i < positions.count(); ++i) {
				QVector2D v = positions.at(i);
				points->SetPoint(i, v.x(), v.y(), 0);
			}
			points->Modified();
			m_targetPolygon->getVtkPolygon()->Modified();
			m_targetPolygon->updateShapeData();
			m_polygon->renderGraphicsView();
		} else {
			// this never happens.
		}
	}
	int id() const {
		return iRIC::generateCommandId("AddPolygonVertex");
	}
	bool mergeWith(const QUndoCommand* other) {
		const AddPolygonVertexCommand* comm = dynamic_cast<const AddPolygonVertexCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (m_polygon != comm->m_polygon) {return false;}
		if (m_vertexId != comm->m_vertexId) {return false;}
		m_vertexPosition = comm->m_vertexPosition;
		return true;
	}
private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
};


class GridCreatingConditionTriangle::DefinePolyLineNewPointCommand : public QUndoCommand
{
public:
	DefinePolyLineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionTriangle* cond) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Add New Break Line Point")}
	{
		m_keyDown = keyDown;
		double dx = point.x();
		double dy = point.y();
		cond->graphicsView()->viewportToWorld(dx, dy);
		m_newPoint = QVector2D(dx, dy);
		m_condition = cond;
		m_targetLine = m_condition->m_selectedLine;
	}
	void redo() {
		vtkPolyLine* line = m_targetLine->getVtkLine();
		if (m_keyDown) {
			// add new point.
			vtkIdType numOfPoints = line->GetPoints()->GetNumberOfPoints();
			if (numOfPoints == 0) {
				m_condition->m_mouseEventMode = GridCreatingConditionTriangle::meDefining;
			}
			line->GetPoints()->InsertNextPoint(m_newPoint.x(), m_newPoint.y(), 0);
			line->GetPoints()->Modified();
		} else {
			// modify the last point.
			vtkIdType lastId = line->GetNumberOfPoints() - 1;
			line->GetPoints()->SetPoint(lastId, m_newPoint.x(), m_newPoint.y(), 0);
			line->GetPoints()->Modified();
		}
		line->Modified();
		m_targetLine->updateShapeData();
		m_condition->renderGraphicsView();
	}
	void undo() {
		vtkPolyLine* line = m_targetLine->getVtkLine();
		if (m_keyDown) {
			// decrease the number of points. i. e. remove the last point.
			vtkIdType numOfPoints = line->GetPoints()->GetNumberOfPoints();
			if (numOfPoints == 1) {
				m_condition->m_mouseEventMode = GridCreatingConditionTriangle::meBeforeDefining;
			}
			line->GetPoints()->SetNumberOfPoints(numOfPoints - 1);
			line->GetPoints()->Modified();
		} else {
			// this does not happen. no implementation needed.
		}
		line->Modified();
		m_targetLine->updateShapeData();
		m_condition->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("GridCreatingConditionTrianglePolyLineDefineNewPoint");
	}
	bool mergeWith(const QUndoCommand* other) {
		const DefinePolyLineNewPointCommand* comm = dynamic_cast<const DefinePolyLineNewPointCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (comm->m_condition != m_condition) {return false;}
		m_newPoint = comm->m_newPoint;
		return true;
	}
private:
	bool m_keyDown;
	GridCreatingConditionTriangle* m_condition;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
	QVector2D m_newPoint;
};

class GridCreatingConditionTriangle::MovePolyLineCommand : public QUndoCommand
{
public:
	MovePolyLineCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Move Break Line")}
	{
		m_keyDown = keyDown;
		double dx = from.x();
		double dy = from.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		QVector2D fromVec(dx, dy);
		dx = to.x();
		dy = to.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		QVector2D toVec(dx, dy);
		m_offset = toVec - fromVec;
		m_polygon = pol;
		m_targetLine = m_polygon->m_selectedLine;
	}
	void redo() {
		vtkPolyLine* pol = m_targetLine->getVtkLine();
		vtkPoints* points = pol->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			double p[3];
			points->GetPoint(i, p);
			p[0] += m_offset.x();
			p[1] += m_offset.y();
			points->SetPoint(i, p);
		}
		points->Modified();
		pol->Modified();
		m_targetLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		vtkPolyLine* pol = m_targetLine->getVtkLine();
		vtkPoints* points = pol->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			double p[3];
			points->GetPoint(i, p);
			p[0] -= m_offset.x();
			p[1] -= m_offset.y();
			points->SetPoint(i, p);
		}
		points->Modified();
		pol->Modified();
		m_targetLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("TrianglePolyLineMove");
	}
	bool mergeWith(const QUndoCommand* other) {
		const MovePolyLineCommand* comm = dynamic_cast<const MovePolyLineCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (comm->m_polygon != m_polygon) {return false;}
		if (comm->m_targetLine != m_targetLine) {return false;}
		m_offset += comm->m_offset;
		return true;
	}
private:
	bool m_keyDown;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
	QVector2D m_offset;
};

class GridCreatingConditionTriangle::MovePolyLineVertexCommand : public QUndoCommand
{
public:
	MovePolyLineVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Move Break Line Vertex")}
	{
		m_keyDown = keyDown;
		m_vertexId = vertexId;
		double dx = from.x();
		double dy = from.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		QVector2D fromVec(dx, dy);
		dx = to.x();
		dy = to.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		QVector2D toVec(dx, dy);
		m_offset = toVec - fromVec;
		m_polygon = pol;
		m_targetLine = m_polygon->m_selectedLine;
	}
	void redo() {
		vtkPolyLine* pol = m_targetLine->getVtkLine();
		vtkPoints* points = pol->GetPoints();
		double p[3];
		points->GetPoint(m_vertexId, p);
		p[0] += m_offset.x();
		p[1] += m_offset.y();
		points->SetPoint(m_vertexId, p);

		points->Modified();
		pol->Modified();
		m_targetLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		vtkPolyLine* pol = m_targetLine->getVtkLine();
		vtkPoints* points = pol->GetPoints();
		double p[3];
		points->GetPoint(m_vertexId, p);
		p[0] -= m_offset.x();
		p[1] -= m_offset.y();
		points->SetPoint(m_vertexId, p);

		points->Modified();
		pol->Modified();
		m_targetLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("PolyLineMoveVertex");
	}
	bool mergeWith(const QUndoCommand* other) {
		const MovePolyLineVertexCommand* comm = dynamic_cast<const MovePolyLineVertexCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (comm->m_polygon != m_polygon) {return false;}
		if (comm->m_targetLine != m_targetLine) {return false;}
		if (comm->m_vertexId != m_vertexId) {return false;}
		m_offset += comm->m_offset;
		return true;
	}
private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
	QVector2D m_offset;
};

class GridCreatingConditionTriangle::AddPolyLineVertexCommand : public QUndoCommand
{
public:
	AddPolyLineVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Insert Break Line Vertex")}
	{
		m_keyDown = keyDown;
		m_vertexId = edgeId + 1;
		double dx = point.x();
		double dy = point.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		m_vertexPosition = QVector2D(dx, dy);
		m_polygon = pol;
		m_targetLine = m_polygon->m_selectedLine;
	}
	void redo() {
		if (m_keyDown) {
			// add vertex.
			vtkPoints* points = m_targetLine->getVtkLine()->GetPoints();
			QVector<QVector2D> positions;
			positions.reserve(points->GetNumberOfPoints());
			double p[3];
			for (vtkIdType i = 0; i < m_vertexId; ++i) {
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			positions.append(m_vertexPosition);
			for (vtkIdType i = m_vertexId; i < points->GetNumberOfPoints(); ++i) {
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			points->SetNumberOfPoints(positions.count());
			for (vtkIdType i = 0; i < positions.count(); ++i) {
				QVector2D v = positions.at(i);
				points->SetPoint(i, v.x(), v.y(), 0);
			}
			points->Modified();
		} else {
			// just modify the vertex position
			vtkPoints* points = m_targetLine->getVtkLine()->GetPoints();
			points->SetPoint(m_vertexId, m_vertexPosition.x(), m_vertexPosition.y(), 0);
			points->Modified();
		}
		m_targetLine->getVtkLine()->Modified();
		m_targetLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		if (m_keyDown) {
			// remove vertex.
			vtkPoints* points = m_targetLine->getVtkLine()->GetPoints();
			QVector<QVector2D> positions;
			positions.reserve(points->GetNumberOfPoints());
			double p[3];
			for (vtkIdType i = 0; i < m_vertexId; ++i) {
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			// skip vertex in m_vertexId[
			for (vtkIdType i = m_vertexId + 1; i < points->GetNumberOfPoints(); ++i) {
				points->GetPoint(i, p);
				positions.append(QVector2D(p[0], p[1]));
			}
			points->SetNumberOfPoints(positions.count());
			for (vtkIdType i = 0; i < positions.count(); ++i) {
				QVector2D v = positions.at(i);
				points->SetPoint(i, v.x(), v.y(), 0);
			}
			points->Modified();
			m_targetLine->getVtkLine()->Modified();
			m_targetLine->updateShapeData();
			m_polygon->renderGraphicsView();
		} else {
			// this never happens.
		}
	}
	int id() const {
		return iRIC::generateCommandId("PolyLineAddVertex");
	}
	bool mergeWith(const QUndoCommand* other) {
		const AddPolyLineVertexCommand* comm = dynamic_cast<const AddPolyLineVertexCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (m_polygon != comm->m_polygon) {return false;}
		if (m_targetLine != comm->m_targetLine) {return false;}
		if (m_vertexId != comm->m_vertexId) {return false;}
		m_vertexPosition = comm->m_vertexPosition;
		return true;
	}
private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
};

void GridCreatingConditionTriangle::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
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
		if (m_selectMode == smPolygon) {
			iRICUndoStack::instance().push(new DefinePolygonNewPointCommand(false, QPoint(event->x(), event->y()), this));
		} else if (m_selectMode == smLine) {
			iRICUndoStack::instance().push(new DefinePolyLineNewPointCommand(false, QPoint(event->x(), event->y()), this));
		}
		break;
	case meTranslate:
		// execute translation.
		if (m_selectMode == smPolygon) {
			iRICUndoStack::instance().push(new MovePolygonCommand(false, m_currentPoint, QPoint(event->x(), event->y()), this));
		} else if (m_selectMode == smLine) {
			iRICUndoStack::instance().push(new MovePolyLineCommand(false, m_currentPoint, QPoint(event->x(), event->y()), this));
		}
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meMoveVertex:
		if (m_selectMode == smPolygon) {
			iRICUndoStack::instance().push(new MovePolygonVertexCommand(false, m_currentPoint, QPoint(event->x(), event->y()), m_selectedPolygon->selectedVertexId(), this));
		} else if (m_selectMode == smLine) {
			iRICUndoStack::instance().push(new MovePolyLineVertexCommand(false, m_currentPoint, QPoint(event->x(), event->y()), m_selectedLine->selectedVertexId(), this));
		}
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meAddVertex:
		if (m_selectMode == smPolygon) {
			iRICUndoStack::instance().push(new AddPolygonVertexCommand(false, m_selectedPolygon->selectedEdgeId(), QPoint(event->x(), event->y()), this));
		} else if (m_selectMode == smLine) {
			iRICUndoStack::instance().push(new AddPolyLineVertexCommand(false, m_selectedLine->selectedEdgeId(), QPoint(event->x(), event->y()), this));
		}
		break;
	case meTranslateDialog:
		break;
	case meEditVerticesDialog:
		break;
	}
}

class GridCreatingConditionTriangle::RemovePolygonVertexCommand : public QUndoCommand
{
public:
	RemovePolygonVertexCommand(vtkIdType vertexId, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Remove Polygon Vertex")}
	{
		m_vertexId = vertexId;
		double p[3];
		pol->m_selectedPolygon->getVtkPolygon()->GetPoints()->GetPoint(m_vertexId, p);
		m_vertexPosition = QVector2D(p[0], p[1]);
		m_polygon = pol;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void redo() {
		vtkPoints* points = m_targetPolygon->getVtkPolygon()->GetPoints();
		QVector<QVector2D> positions;
		positions.reserve(points->GetNumberOfPoints());
		double p[3];
		for (vtkIdType i = 0; i < m_vertexId; ++i) {
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		// skip vertex in m_vertexId[
		for (vtkIdType i = m_vertexId + 1; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		points->SetNumberOfPoints(positions.count());
		for (vtkIdType i = 0; i < positions.count(); ++i) {
			QVector2D v = positions.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
		m_targetPolygon->getVtkPolygon()->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		vtkPoints* points = m_targetPolygon->getVtkPolygon()->GetPoints();
		QVector<QVector2D> positions;
		positions.reserve(points->GetNumberOfPoints());
		double p[3];
		for (vtkIdType i = 0; i < m_vertexId; ++i) {
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		positions.append(m_vertexPosition);
		for (vtkIdType i = m_vertexId; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		points->SetNumberOfPoints(positions.count());
		for (vtkIdType i = 0; i < positions.count(); ++i) {
			QVector2D v = positions.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_targetPolygon->getVtkPolygon()->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	}
private:
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
};

class GridCreatingConditionTriangle::RemovePolyLineVertexCommand : public QUndoCommand
{
public:
	RemovePolyLineVertexCommand(vtkIdType vertexId, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Remove Break Line Vertex")}
	{
		m_vertexId = vertexId;
		double p[3];
		pol->m_selectedLine->getVtkLine()->GetPoints()->GetPoint(m_vertexId, p);
		m_vertexPosition = QVector2D(p[0], p[1]);
		m_polygon = pol;
		m_targetLine = m_polygon->m_selectedLine;
	}
	void redo() {
		vtkPoints* points = m_targetLine->getVtkLine()->GetPoints();
		QVector<QVector2D> positions;
		positions.reserve(points->GetNumberOfPoints());
		double p[3];
		for (vtkIdType i = 0; i < m_vertexId; ++i) {
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		// skip vertex in m_vertexId[
		for (vtkIdType i = m_vertexId + 1; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		points->SetNumberOfPoints(positions.count());
		for (vtkIdType i = 0; i < positions.count(); ++i) {
			QVector2D v = positions.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
		m_targetLine->getVtkLine()->Modified();
		m_targetLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	void undo() {
		vtkPoints* points = m_targetLine->getVtkLine()->GetPoints();
		QVector<QVector2D> positions;
		positions.reserve(points->GetNumberOfPoints());
		double p[3];
		for (vtkIdType i = 0; i < m_vertexId; ++i) {
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		positions.append(m_vertexPosition);
		for (vtkIdType i = m_vertexId; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			positions.append(QVector2D(p[0], p[1]));
		}
		points->SetNumberOfPoints(positions.count());
		for (vtkIdType i = 0; i < positions.count(); ++i) {
			QVector2D v = positions.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_targetLine->getVtkLine()->Modified();
		m_targetLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}

private:
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
};

void GridCreatingConditionTriangle::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {

		// left click
		double worldX = static_cast<double>(event->x());
		double worldY = static_cast<double>(event->y());
		v->viewportToWorld(worldX, worldY);

		switch (m_mouseEventMode) {
		case meNormal:
			// new selected polygon.
			if (selectObject(event->pos())) {
				// selection changed.
				updateMouseEventMode();
				updateMouseCursor(v);
				updateActionStatus();
				renderGraphicsView();
			}
			break;
		case meBeforeDefining:
			// enter defining mode.
			m_mouseEventMode = meDefining;
			if (m_selectMode == smPolygon) {
				iRICUndoStack::instance().push(new DefinePolygonNewPointCommand(true, QPoint(event->x(), event->y()), this));
			} else if (m_selectMode == smLine) {
				iRICUndoStack::instance().push(new DefinePolyLineNewPointCommand(true, QPoint(event->x(), event->y()), this));
			}
		case meDefining:
			if (m_selectMode == smPolygon) {
				iRICUndoStack::instance().push(new DefinePolygonNewPointCommand(true, QPoint(event->x(), event->y()), this));
			} else if (m_selectMode == smLine) {
				iRICUndoStack::instance().push(new DefinePolyLineNewPointCommand(true, QPoint(event->x(), event->y()), this));
			}
			break;
		case meTranslatePrepare:
			if (selectObject(event->pos())) {
				// selection changed.
				updateMouseEventMode();
				updateMouseCursor(v);
				updateActionStatus();
				renderGraphicsView();
			} else {
				// start translating
				m_mouseEventMode = meTranslate;
				m_currentPoint = QPoint(event->x(), event->y());
				updateMouseCursor(v);
				// push the first translation command.
				if (m_selectMode == smPolygon) {
					iRICUndoStack::instance().push(new MovePolygonCommand(true, m_currentPoint, m_currentPoint, this));
				} else if (m_selectMode == smLine) {
					iRICUndoStack::instance().push(new MovePolyLineCommand(true, m_currentPoint, m_currentPoint, this));
				}
			}
			break;
		case meMoveVertexPrepare:
			m_mouseEventMode = meMoveVertex;
			m_currentPoint = QPoint(event->x(), event->y());
			// push the first move command.
			if (m_selectMode == smPolygon) {
				iRICUndoStack::instance().push(new MovePolygonVertexCommand(true, m_currentPoint, m_currentPoint, m_selectedPolygon->selectedVertexId(), this));
			} else if (m_selectMode == smLine) {
				iRICUndoStack::instance().push(new MovePolyLineVertexCommand(true, m_currentPoint, m_currentPoint, m_selectedLine->selectedVertexId(), this));
			}
			break;
		case meAddVertexPrepare:
			m_mouseEventMode = meAddVertex;
			if (m_selectMode == smPolygon) {
				iRICUndoStack::instance().push(new AddPolygonVertexCommand(true, m_selectedPolygon->selectedEdgeId(), QPoint(event->x(), event->y()), this));
			} else if (m_selectMode == smLine) {
				iRICUndoStack::instance().push(new AddPolyLineVertexCommand(true, m_selectedLine->selectedEdgeId(), QPoint(event->x(), event->y()), this));
			}
			break;
		case meAddVertexNotPossible:
			// do nothing.
			break;
		case meRemoveVertexPrepare:
			if (m_selectMode == smPolygon) {
				if (m_selectedPolygon->polygon().count() == 1) {
					// you are going to remove the last point.
					deletePolygon(true);
				} else {
					iRICUndoStack::instance().push(new RemovePolygonVertexCommand(m_selectedPolygon->selectedVertexId(), this));
				}
			} else if (m_selectMode == smLine) {
				if (m_selectedLine->polyLine().count() == 1) {
					// you are going to remove the last point.
					deleteLine(true);
				} else {
					iRICUndoStack::instance().push(new RemovePolyLineVertexCommand(m_selectedLine->selectedVertexId(), this));
				}
			}
			m_inhibitSelect = true;
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
		case meTranslateDialog:
			break;
		case meEditVerticesDialog:
			break;
		}
		updateMouseCursor(v);
		updateActionStatus();
	} else if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GridCreatingConditionTriangle::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (m_mouseEventMode) {
		case meNormal:
		case meTranslatePrepare:
		case meMoveVertexPrepare:
		case meAddVertexPrepare:
		case meAddVertexNotPossible:
		case meRemoveVertexPrepare:
		case meRemoveVertexNotPossible:
		case meTranslate:
		case meMoveVertex:
		case meAddVertex:
			m_currentPoint = QPoint(event->x(), event->y());
			updateMouseEventMode();
			updateMouseCursor(v);
			updateActionStatus();
			break;
		case meDefining:
			// do nothing no mode change.
			updateMouseCursor(v);
			break;
		default:
			break;
		}
		m_inhibitSelect = false;
	} else if (event->button() == Qt::RightButton) {
		if (m_mouseEventMode == meEditVerticesDialog) {return;}
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionTriangle::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meNormal:
	case meAddVertexNotPossible:
	case meRemoveVertexNotPossible:
	case meTranslateDialog:
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

void GridCreatingConditionTriangle::addCustomMenuItems(QMenu* /*menu*/)
{}

void GridCreatingConditionTriangle::definePolygon(bool doubleClick)
{
	int minCount = 4;
	if (doubleClick) {
		minCount = 3;
	}
	if (m_selectedPolygon == nullptr) {return;}
	if (m_selectedPolygon->polygon().count() <= minCount) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Polygon must have three vertices at least."));
		return;
	}
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.undo();
	stack.beginMacro(tr("Finish Defining Polygon"));
	// finish defining the polygon.
	stack.push(new FinishPolygonDefiningCommand(this));
	stack.endMacro();
}

void GridCreatingConditionTriangle::defineLine(bool doubleClick)
{
	int minCount = 2;
	if (doubleClick) {
		minCount = 1;
	}
	if (m_selectedLine == nullptr) {return;}
	if (m_selectedLine->polyLine().count() <= minCount) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Break line must have two vertices at least."));
		return;
	}
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.undo();
	stack.beginMacro(tr("Finish Defining Break Line"));
	// finish defining the line.
	stack.push(new FinishPolyLineDefiningCommand(this));
	stack.endMacro();
}

void GridCreatingConditionTriangle::addVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meAddVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GridCreatingConditionTriangle::removeVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meRemoveVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GridCreatingConditionTriangle::doLoadFromProjectMainFile(const QDomNode& /*node*/) {}

void GridCreatingConditionTriangle::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) {}

void GridCreatingConditionTriangle::loadExternalData(const QString& filename)
{
	clear();

	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);
	QPolygonF poly;
	auto offset = this->offset();
	s >> m_angleConstraint >> m_angle;
	s >> m_areaConstraint >> m_area;
	s >> poly;
	if (poly.size() > 0) {
		if (!offset.isNull()) poly.translate(-offset.x(), -offset.y());
		m_mouseEventMode = meNormal;
		m_gridRegionPolygon->setPolygon(poly);
		Q_ASSERT(m_selectedPolygon == m_gridRegionPolygon);
		m_selectMode = smPolygon;
		informDeselection(graphicsView());
	} else {
		m_mouseEventMode = meBeforeDefining;
	}
	int divLines;
	int remeshPolygons;
	int holePolygons;
	s >> divLines;
	for (int i = 0; i < divLines; ++i) {
		QVector<QPointF> line;
		s >> line;
		if (!offset.isNull()) for (QPointF &pt : line) pt += QPointF(-offset.x(), -offset.y());
		GridCreatingConditionTriangleDivisionLine* tmpLine = new GridCreatingConditionTriangleDivisionLine(this);
		tmpLine->setZDepthRange(m_depthRange.min(), m_depthRange.max());
		tmpLine->setPolyLine(line);
		m_divisionLines.append(tmpLine);
	}
	s >> remeshPolygons;
	for (int i = 0; i < remeshPolygons; ++i) {
		QPolygonF pol;
		double area;
		s >> pol;
		s >> area;
		if (!offset.isNull()) pol.translate(-offset.x(), -offset.y());
		GridCreatingConditionTriangleRemeshPolygon* tmpPol = new GridCreatingConditionTriangleRemeshPolygon(this);
		tmpPol->setZDepthRange(m_depthRange.min(), m_depthRange.max());
		tmpPol->setPolygon(pol);
		tmpPol->setCellSize(area);
		m_remeshPolygons.append(tmpPol);
	}
	s >> holePolygons;
	for (int i = 0; i < holePolygons; ++i) {
		QPolygonF pol;
		s >> pol;
		if (!offset.isNull()) pol.translate(-offset.x(), -offset.y());
		GridCreatingConditionTriangleHolePolygon* tmpPol = new GridCreatingConditionTriangleHolePolygon(this);
		tmpPol->setZDepthRange(m_depthRange.min(), m_depthRange.max());
		tmpPol->setPolygon(pol);
		m_holePolygons.append(tmpPol);
	}
	deselectAll();
	updateActionStatus();
	f.close();
}

void GridCreatingConditionTriangle::saveExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);
	auto offset = this->offset();
	auto voffset = QPointF(-offset.x(), -offset.y());
	s << m_angleConstraint << m_angle;
	s << m_areaConstraint << m_area;
	s << m_gridRegionPolygon->polygon(voffset);
	int divLines = m_divisionLines.count();
	s << divLines;
	for (GridCreatingConditionTriangleDivisionLine* line : m_divisionLines) {
		s << line->polyLine(voffset);
	}
	int remeshPolygons = m_remeshPolygons.count();
	s << remeshPolygons;
	for (GridCreatingConditionTriangleRemeshPolygon* pol : m_remeshPolygons) {
		s << pol->polygon(voffset);
		s << pol->cellSize();
	}
	int holePolygons = m_holePolygons.count();
	s << holePolygons;
	for (GridCreatingConditionTriangleHolePolygon* pol : m_holePolygons) {
		s << pol->polygon(voffset);
	}
	f.close();
}

void GridCreatingConditionTriangle::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GridCreatingConditionTriangle::assignActorZValues(const ZDepthRange& range)
{
	m_depthRange = range;
	m_gridRegionPolygon->setZDepthRange(range.min(), range.max());
	for (GridCreatingConditionTriangleDivisionLine* line : m_divisionLines) {
		line->setZDepthRange(range.min(), range.max());
	}
	for (GridCreatingConditionTriangleRemeshPolygon* pol : m_remeshPolygons) {
		pol->setZDepthRange(range.min(), range.max());
	}
	for (GridCreatingConditionTriangleHolePolygon* pol : m_holePolygons) {
		pol->setZDepthRange(range.min(), range.max());
	}
}

void GridCreatingConditionTriangle::updateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QVector2D worldPos(dx, dy);
	switch (m_mouseEventMode) {
	case meAddVertexNotPossible:
	case meAddVertexPrepare:
		if (m_selectMode == smNone) {return;}
		if (m_selectMode == smPolygon) {
			if (m_selectedPolygon->isEdgeSelectable(worldPos, graphicsView()->stdRadius(5))) {
				m_mouseEventMode = meAddVertexPrepare;
			} else {
				m_mouseEventMode = meAddVertexNotPossible;
			}
		}
		if (m_selectMode == smLine) {
			if (m_selectedLine->isEdgeSelectable(worldPos, graphicsView()->stdRadius(5))) {
				m_mouseEventMode = meAddVertexPrepare;
			} else {
				m_mouseEventMode = meAddVertexNotPossible;
			}
		}
		break;
	case meRemoveVertexNotPossible:
	case meRemoveVertexPrepare:
		if (m_selectMode == smNone) {return;}
		if (m_selectMode == smPolygon) {
			if (m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(5))) {
				m_mouseEventMode = meRemoveVertexPrepare;
			} else {
				m_mouseEventMode = meRemoveVertexNotPossible;
			}
		}
		if (m_selectMode == smLine) {
			if (m_selectedLine->isVertexSelectable(worldPos, graphicsView()->stdRadius(5))) {
				m_mouseEventMode = meRemoveVertexPrepare;
			} else {
				m_mouseEventMode = meRemoveVertexNotPossible;
			}
		}
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		if (m_selectMode == smNone) {return;}
		if (m_selectMode == smPolygon) {
			if (m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(5))) {
				m_mouseEventMode = meMoveVertexPrepare;
			} else if (m_selectedPolygon->isPolygonSelectable(worldPos)) {
				m_mouseEventMode = meTranslatePrepare;
			} else {
				m_mouseEventMode = meNormal;
			}
		}
		if (m_selectMode == smLine) {
			if (m_selectedLine->isVertexSelectable(worldPos, graphicsView()->stdRadius(5))) {
				m_mouseEventMode = meMoveVertexPrepare;
			} else if (m_selectedLine->isEdgeSelectable(worldPos, graphicsView()->stdRadius(5))) {
				m_mouseEventMode = meTranslatePrepare;
			} else {
				m_mouseEventMode = meNormal;
			}
		}
		break;
	case meBeforeDefining:
	case meDefining:
		// do nothing
		break;
	case meTranslateDialog:
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionTriangle::updateActionStatus()
{
	switch (m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);

		m_defineModeAction->setChecked(false);
		m_refineModeAction->setDisabled(true);
		m_refineModeAction->setChecked(false);
		m_holeModeAction->setDisabled(true);
		m_holeModeAction->setChecked(false);
		m_divlineModeAction->setDisabled(true);
		m_divlineModeAction->setChecked(false);
		m_deleteAction->setDisabled(true);
		if (dynamic_cast<GridCreatingConditionTriangleGridRegionPolygon*>(m_selectedPolygon) != nullptr) {
			m_defineModeAction->setChecked(true);
		} else if (dynamic_cast<GridCreatingConditionTriangleRemeshPolygon*>(m_selectedPolygon) != nullptr) {
			m_refineModeAction->setChecked(true);
		} else if (dynamic_cast<GridCreatingConditionTriangleHolePolygon*>(m_selectedPolygon) != nullptr) {
			m_holeModeAction->setChecked(true);
		} else if (dynamic_cast<GridCreatingConditionTriangleDivisionLine*>(m_selectedLine) != nullptr) {
			m_divlineModeAction->setChecked(true);
		}
		break;
	case meTranslate:
	case meMoveVertex:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setDisabled(true);

		m_refineModeAction->setDisabled(true);
		m_holeModeAction->setDisabled(true);
		m_divlineModeAction->setDisabled(true);
		m_deleteAction->setDisabled(true);
		break;

		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setChecked(false);

		if (m_selectMode != smNone) {
			m_addVertexAction->setEnabled(true);
			if (m_selectMode == smPolygon) {
				m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
			} else if (m_selectMode == smLine) {
				m_removeVertexAction->setEnabled(activePolylineHasThreeVertices());
			}
			m_coordEditAction->setEnabled(true);
		} else {
			m_addVertexAction->setDisabled(true);
			m_removeVertexAction->setDisabled(true);
			m_coordEditAction->setDisabled(true);
		}

		m_defineModeAction->setEnabled(false);
		m_defineModeAction->setChecked(false);
		m_refineModeAction->setEnabled(true);
		m_refineModeAction->setChecked(false);
		m_holeModeAction->setEnabled(true);
		m_holeModeAction->setChecked(false);
		m_divlineModeAction->setEnabled(true);
		m_divlineModeAction->setChecked(false);
		if (m_selectedPolygon != nullptr) {
			m_addVertexAction->setEnabled(true);
			m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
			m_coordEditAction->setEnabled(true);
			m_deleteAction->setEnabled(true);
			m_editMaxAreaAction->setDisabled(true);
			if (dynamic_cast<GridCreatingConditionTriangleRemeshPolygon*>(m_selectedPolygon) != nullptr) {
				m_editMaxAreaAction->setEnabled(true);
			}
		} else if (m_selectedLine != nullptr) {
			m_addVertexAction->setEnabled(true);
			m_removeVertexAction->setEnabled(activePolylineHasThreeVertices());
			m_coordEditAction->setEnabled(true);
			m_deleteAction->setEnabled(true);
			m_editMaxAreaAction->setDisabled(true);
		} else {
			m_addVertexAction->setDisabled(true);
			m_removeVertexAction->setDisabled(true);
			m_coordEditAction->setDisabled(true);
			m_deleteAction->setDisabled(true);
			m_editMaxAreaAction->setDisabled(true);
		}
		break;
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meAddVertex:
		m_addVertexAction->setChecked(true);
		m_removeVertexAction->setChecked(false);

		if (m_selectMode != smNone) {
			m_addVertexAction->setEnabled(true);
			if (m_selectMode == smPolygon) {
				m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
			} else if (m_selectMode == smLine) {
				m_removeVertexAction->setEnabled(activePolylineHasThreeVertices());
			}
			m_coordEditAction->setEnabled(true);
		} else {
			m_addVertexAction->setDisabled(true);
			m_removeVertexAction->setDisabled(true);
			m_coordEditAction->setDisabled(true);
		}
		m_defineModeAction->setChecked(false);
		m_refineModeAction->setDisabled(true);
		m_refineModeAction->setChecked(false);
		m_holeModeAction->setDisabled(true);
		m_holeModeAction->setChecked(false);
		m_divlineModeAction->setDisabled(true);
		m_divlineModeAction->setChecked(false);
		m_deleteAction->setEnabled(true);

		break;
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(true);
		m_coordEditAction->setEnabled(false);

		m_defineModeAction->setChecked(false);
		m_refineModeAction->setDisabled(true);
		m_refineModeAction->setChecked(false);
		m_holeModeAction->setDisabled(true);
		m_holeModeAction->setChecked(false);
		m_divlineModeAction->setDisabled(true);
		m_divlineModeAction->setChecked(false);
		m_deleteAction->setEnabled(true);
		break;
	case meTranslateDialog:
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionTriangle::editCoordinates()
{
	/*
		m_mouseEventMode = meEditVerticesDialog;
		GridCreatingConditionTriangleCoordinatesEditDialog* dialog = new GridCreatingConditionTriangleCoordinatesEditDialog(this, preProcessorWindow());
		dialog->show();
		iricMainWindow()->enterModelessDialogMode();
		connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
		connect(dialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	*/

	m_mouseEventMode = meEditVerticesDialog;
	if (m_selectedPolygon != nullptr) {
		GridCreatingConditionTrianglePolygonCoordinatesEditDialog* dialog = new GridCreatingConditionTrianglePolygonCoordinatesEditDialog(this, preProcessorWindow());
		dialog->show();
		iricMainWindow()->enterModelessDialogMode();
		connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
		connect(dialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	}
	if (m_selectedLine != nullptr) {
		GridCreatingConditionTrianglePolyLineCoordinatesEditDialog* dialog = new GridCreatingConditionTrianglePolyLineCoordinatesEditDialog(this, preProcessorWindow());
		dialog->show();
		iricMainWindow()->enterModelessDialogMode();
		connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
		connect(dialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	}
}

void GridCreatingConditionTriangle::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

bool GridCreatingConditionTriangle::create(QWidget* parent)
{
	if (! checkCondition()) {return false;}
	if (m_area == 0) {
		// specify default value.
		QRectF rect = m_gridRegionPolygon->polygon().boundingRect();
		m_area = iRIC::roundedValue(rect.width() * rect.height() / 100, 1);
	}
	GridCreatingConditionTriangleSettingDialog dialog(parent);
	dialog.setAngleConstraint(m_angleConstraint);
	dialog.setAngle(m_angle);
	dialog.setAreaConstraint(m_areaConstraint);
	dialog.setArea(m_area);
	if (QDialog::Rejected == dialog.exec()) {
		return false;
	}
	m_angleConstraint = dialog.angleConstraint();
	m_angle = dialog.angle();
	m_areaConstraint = dialog.areaConstraint();
	m_area = dialog.area();

	Grid* grid = createGrid();
	if (grid == nullptr) {return false;}
	emit gridCreated(grid);
	return true;
}

void GridCreatingConditionTriangle::clear()
{
	initParams();
	for (int i = 0; i < m_remeshPolygons.count(); ++i) {
		delete m_remeshPolygons[i];
	}
	m_remeshPolygons.clear();
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		delete m_holePolygons[i];
	}
	m_holePolygons.clear();

	delete m_gridRegionPolygon;
	m_gridRegionPolygon = new GridCreatingConditionTriangleGridRegionPolygon(this);
	m_gridRegionPolygon->setActive(true);
	m_gridRegionPolygon->setZDepthRange(m_depthRange.min(), m_depthRange.max());
	m_mouseEventMode = meBeforeDefining;
	m_selectedPolygon = m_gridRegionPolygon;
	updateMouseCursor(graphicsView());
	updateActionStatus();
	renderGraphicsView();
}

void GridCreatingConditionTriangle::initParams()
{
	m_angleConstraint = false;
	m_angle = 20;
	m_areaConstraint = false;
	m_area = 0;
}

Grid* GridCreatingConditionTriangle::createGrid()
{
	Unstructured2DGrid* grid = new Unstructured2DGrid(nullptr);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);
	QPolygonF pol = m_gridRegionPolygon->polygon();
	QVector<QPointF> line;
	int pointCount = 0;
	int segmentCount = 0;
	int regionCount = 1;
	pointCount += (pol.count() - 1);
	segmentCount += (pol.count() - 1);
	QPointF offset = pol.boundingRect().topLeft();
	for (int i = 0; i < m_remeshPolygons.count(); ++i) {
		pol = m_remeshPolygons[i]->polygon();
		pointCount += (pol.count() - 1);
		segmentCount += (pol.count() - 1);
		++regionCount;
	}
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		pol = m_holePolygons[i]->polygon();
		pointCount += (pol.count() - 1);
		segmentCount += (pol.count() - 1);
	}
	for (int i = 0; i < m_divisionLines.count(); ++i) {
		line = m_divisionLines[i]->polyLine();
		pointCount += line.count();
		segmentCount += (line.count() - 1);
	}
	triangulateio in, out;

	in.pointlist = new double[pointCount * 2];
	in.pointattributelist = NULL;
	in.pointmarkerlist = NULL;
	in.numberofpoints = pointCount;
	in.numberofpointattributes = 0;

	in.trianglelist = NULL;
	in.triangleattributelist = NULL;
	in.trianglearealist = NULL;
	in.neighborlist = NULL;
	in.numberoftriangles = 0;
	in.numberofcorners = 0;
	in.numberoftriangleattributes = 0;

	in.segmentlist = new int[segmentCount * 2];
	in.segmentmarkerlist = NULL;
	in.numberofsegments = segmentCount;

	in.holelist = new double[m_holePolygons.count() * 2];
	in.numberofholes = m_holePolygons.count();

	in.regionlist = new double[regionCount * 4];
	in.numberofregions = regionCount;

	in.edgelist = NULL;
	in.edgemarkerlist = NULL;
	in.normlist = NULL;
	in.numberofedges = 0;

	pol = m_gridRegionPolygon->polygon(offset);
	for (int i = 0; i < pol.count() - 1; ++i) {
		*(in.pointlist + i * 2)     = pol.at(i).x();
		*(in.pointlist + i * 2 + 1) = pol.at(i).y();
		*(in.segmentlist + i * 2) = i + 1;
		*(in.segmentlist + i * 2 + 1) = (i + 1) % (pol.count() - 1) + 1;
	}
	QPointF innerP = m_gridRegionPolygon->innerPoint(offset);
	*(in.regionlist)     = innerP.x();
	*(in.regionlist + 1) = innerP.y();
	*(in.regionlist + 2) = 0;
	if (m_areaConstraint) {
		*(in.regionlist + 3) = m_area;
	} else {
		// no area constraint.
		QRectF rect = pol.boundingRect();
		*(in.regionlist + 3) = rect.width() * rect.height();
	}
	int pOffset = 2 * (pol.count() - 1);
	int sOffset = (pol.count() - 1);
	int aOffset = 1;
	for (int i = 0; i < m_remeshPolygons.count(); ++i) {
		GridCreatingConditionTriangleRemeshPolygon* rpol = m_remeshPolygons[i];
		pol = rpol->polygon(offset);
		for (int j = 0; j < pol.count() - 1; ++j) {
			*(in.pointlist + j * 2 + pOffset)     = pol.at(j).x();
			*(in.pointlist + j * 2 + 1 + pOffset) = pol.at(j).y();
			*(in.segmentlist + j * 2 + sOffset * 2) = j + sOffset + 1;
			*(in.segmentlist + j * 2 + 1 + sOffset * 2) = (j + 1) % (pol.count() - 1) + sOffset + 1;
		}
		innerP = rpol->innerPoint(offset);
		*(in.regionlist + 4 * aOffset) = innerP.x();
		*(in.regionlist + 4 * aOffset + 1) = innerP.y();
		*(in.regionlist + 4 * aOffset + 2) = 0;
		*(in.regionlist + 4 * aOffset + 3) = rpol->cellSize();
		pOffset += 2 * (pol.count() - 1);
		sOffset += (pol.count() - 1);
		++aOffset;
	}
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		GridCreatingConditionTriangleHolePolygon* hpol = m_holePolygons[i];
		pol = hpol->polygon(offset);
		for (int j = 0; j < pol.count() - 1; ++j) {
			*(in.pointlist + j * 2 + pOffset)     = pol.at(j).x();
			*(in.pointlist + j * 2 + 1 + pOffset) = pol.at(j).y();
			*(in.segmentlist + j * 2 + sOffset * 2) = j + sOffset + 1;
			*(in.segmentlist + j * 2 + 1 + sOffset * 2) = (j + 1) % (pol.count() - 1) + sOffset + 1;
		}
		innerP = hpol->innerPoint(offset);
		*(in.holelist + i * 2) = innerP.x();
		*(in.holelist + i * 2 + 1) = innerP.y();
		pOffset += 2 * (pol.count() - 1);
		sOffset += (pol.count() - 1);
	}
	for (int i = 0; i < m_divisionLines.count(); ++i) {
		GridCreatingConditionTriangleDivisionLine* line = m_divisionLines[i];
		QVector<QPointF> l = line->polyLine(offset);
		for (int j = 0; j < l.count(); ++j) {
			*(in.pointlist + j * 2 + pOffset)     = l.at(j).x();
			*(in.pointlist + j * 2 + 1 + pOffset) = l.at(j).y();
			if (j != 0) {
				*(in.segmentlist + (j - 1) * 2 + sOffset * 2) = j + pOffset / 2;
				*(in.segmentlist + (j - 1) * 2 + 1 + sOffset * 2) = j + 1 + pOffset / 2;
			}
		}
		pOffset += 2 * l.count();
		sOffset += (l.count() - 1);
	}

	out.pointlist = NULL;
	out.pointattributelist = NULL;
	out.pointmarkerlist = NULL;
	out.trianglelist = NULL;
	out.triangleattributelist = NULL;
	out.trianglearealist = NULL;
	out.neighborlist = NULL;
	out.segmentlist = NULL;
	out.segmentmarkerlist = NULL;
	out.holelist = NULL;
	out.regionlist = NULL;
	out.edgelist = NULL;
	out.edgemarkerlist = NULL;
	out.normlist = NULL;

	QString argstr;
	argstr.append("p");
	if (m_angleConstraint) {
		argstr.append(QString("q%1").arg(m_angle));
	}
	if (m_areaConstraint || m_remeshPolygons.count() > 0) {
		argstr.append("a");
	}
	argstr.append("D");
	char* arg = new char[argstr.length() + 1];
	strcpy(arg, iRIC::toStr(argstr).c_str());

	TriangleExecuteThread* thread = new TriangleExecuteThread(this);

	thread->setArgs(arg);
	thread->setIOs(&in, &out);

	PreProcessorGridAndGridCreatingConditionDataItemInterface* di = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(m_conditionDataItem->parent());
	std::string zoneName = di->zoneName();

	QString triExe = iRICRootPath::get().append("/triangle.exe");
	QString workFolder = projectData()->workDirectory();
	QString polyFileName = QString("%1.poly").arg(zoneName.c_str());
	QString fileArgs = "-ve";
	fileArgs.append(argstr);

	thread->setFileOutputSetting(triExe, fileArgs, polyFileName, workFolder);

	// file output.
	QFile f(QString("%1/%2").arg(workFolder).arg(polyFileName));
	f.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text);
	QTextStream o(&f);
	o.setRealNumberPrecision(20);
	o.setRealNumberNotation(QTextStream::ScientificNotation);
	o << in.numberofpoints << " 2 0 0\n";
	for (int i = 0; i < in.numberofpoints; ++i) {
		o << i + 1 << " " << *(in.pointlist + i * 2) << " " << *(in.pointlist + i * 2 + 1) << "\n";
	}
	o << in.numberofsegments << " 0\n";
	for (int i = 0; i < in.numberofsegments; ++i) {
		o << i + 1 << " " << *(in.segmentlist + i * 2) << " " << *(in.segmentlist + i * 2 + 1) << "\n";
	}
	o << in.numberofholes << "\n";
	for (int i = 0; i < in.numberofholes; ++i) {
		o << i + 1 << " " << *(in.holelist + i * 2) << " " << *(in.holelist + i * 2 + 1) << "\n";
	}
	o << in.numberofregions << "\n";
	for (int i = 0; i < in.numberofregions; ++i) {
		o << i + 1 << " " << *(in.regionlist + i * 4) << " " << *(in.regionlist + i * 4 + 1) << " " << *(in.regionlist + i * 4 + 2) << " " << *(in.regionlist + i * 4 + 3) << "\n";
	}
	f.close();

	// start execution!
	thread->start();
	// wait for finish
	bool finished = thread->wait(2000);
	m_canceled = false;

	if (! finished) {
		int prog = 10;
		// Not finished yet. Show wait dialog.
		WaitDialog* waitDialog = new WaitDialog(preProcessorWindow());
		waitDialog->showProgressBar();
		waitDialog->setRange(0, 100);
		waitDialog->setUnknownLimitMode(300);
		waitDialog->setProgress(prog);
		waitDialog->setMessage(tr("Generating grid..."));
		connect(waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
		waitDialog->show();
		while (! finished && ! m_canceled) {
			qApp->processEvents();
			finished = thread->wait(200);
			waitDialog->setProgress(prog);
			++prog;
		}
		waitDialog->hide();
		delete waitDialog;
		if (m_canceled) {
			// not finished, but canceled;
			thread->terminate();
			thread->wait();
		}
	}
	delete thread;

	// free memory
	delete arg;
	delete in.pointlist;
	delete in.segmentlist;
	delete in.regionlist;
	delete in.holelist;

	if (m_canceled) {
		if (out.pointlist != NULL) {
			trifree(out.pointlist);
		}
		if (out.trianglelist != NULL) {
			trifree(out.trianglelist);
		}
		return nullptr;
	}
	// copy the result to VTK containers.
	vtkPoints* points = vtkPoints::New();
	points->Allocate(out.numberofpoints);
	points->SetDataTypeToDouble();
	for (int i = 0; i < out.numberofpoints; ++i) {
		double v[3];
		v[0] = *(out.pointlist + i * 2) + offset.x();
		v[1] = *(out.pointlist + i * 2 + 1) + offset.y();
		v[2] = 0;
		points->InsertNextPoint(v);
	}
	grid->vtkGrid()->SetPoints(points);

	grid->vtkGrid()->Allocate(out.numberoftriangles);
	for (int i = 0; i < out.numberoftriangles; ++i) {
		vtkIdType id1, id2, id3;
		vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
		id1 = *(out.trianglelist + i * 3) - 1;
		id2 = *(out.trianglelist + i * 3 + 1) - 1;
		id3 = *(out.trianglelist + i * 3 + 2) - 1;
		tri->GetPointIds()->SetId(0, id1);
		tri->GetPointIds()->SetId(1, id2);
		tri->GetPointIds()->SetId(2, id3);
		grid->vtkGrid()->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	}

	// allocate memory for all grid related conditions.
	QList<GridAttributeContainer*>& clist = grid->gridAttributes();
	for (auto it = clist.begin(); it != clist.end(); ++it) {
		(*it)->allocate();
	}
	grid->setModified();
	grid->vtkGrid()->BuildLinks();
	if (out.pointlist != NULL) {
		trifree(out.pointlist);
	}
	if (out.trianglelist != NULL) {
		trifree(out.trianglelist);
	}

	return grid;
}

class GridCreatingConditionTriangle::AddRemeshPolygonCommand : public QUndoCommand
{
public:
	AddRemeshPolygonCommand(GridCreatingConditionTriangleRemeshPolygon* newPoly, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Add New Remesh Polygon")}
	{
		m_polygon = pol;
		m_targetPolygon = newPoly;
		m_undoed = false;
	}
	virtual ~AddRemeshPolygonCommand() {
		if (m_undoed) {
//			delete m_targetPolygon;
		}
	}
	void redo() {
		m_polygon->deselectAll();

		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meBeforeDefining;
		m_polygon->m_selectMode = GridCreatingConditionTriangle::smPolygon;
		m_targetPolygon->setActive(true);
		m_polygon->m_selectedPolygon = m_targetPolygon;
		m_polygon->m_remeshPolygons.append(m_targetPolygon);
		m_polygon->updateActionStatus();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->renderGraphicsView();
		m_undoed = false;
	}
	void undo() {
		m_polygon->deselectAll();

		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
		m_polygon->m_remeshPolygons.removeOne(m_targetPolygon);
		m_polygon->updateActionStatus();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->renderGraphicsView();
		m_undoed = true;
	}
private:
	bool m_undoed;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleRemeshPolygon* m_targetPolygon;
};

void GridCreatingConditionTriangle::addRefinementPolygon()
{
	GridCreatingConditionTriangleRemeshPolygon* pol = new GridCreatingConditionTriangleRemeshPolygon(this);
	pol->setZDepthRange(m_depthRange.min(), m_depthRange.max());
	pol->setActive(true);
	if (m_selectedPolygon != nullptr) {
		m_selectedPolygon->setActive(false);
	}
	m_selectedPolygon = pol;
	m_editMaxAreaAction->disconnect();
	connect(m_editMaxAreaAction, SIGNAL(triggered()), pol, SLOT(editGridSize()));
	iRICUndoStack::instance().push(new AddRemeshPolygonCommand(pol, this));
	InformationDialog::information(preProcessorWindow(), GridCreatingConditionTriangle::tr("Information"), GridCreatingConditionTriangle::tr("Please define refinement region. Refinement region can be defined as polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gctriangle_addrefinementpolygon");
}

class GridCreatingConditionTriangle::AddHolePolygonCommand : public QUndoCommand
{
public:
	AddHolePolygonCommand(GridCreatingConditionTriangleHolePolygon* newPoly, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Add New Hole Polygon")}
	{
		m_polygon = pol;
		m_targetPolygon = newPoly;
		m_undoed = false;
	}
	virtual ~AddHolePolygonCommand() {
		if (m_undoed) {
//			delete m_targetPolygon;
		}
	}
	void redo() {
		m_polygon->deselectAll();

		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meBeforeDefining;
		m_polygon->m_selectMode = GridCreatingConditionTriangle::smPolygon;
		m_targetPolygon->setActive(true);
		m_polygon->m_selectedPolygon = m_targetPolygon;
		m_polygon->m_holePolygons.append(m_targetPolygon);
		m_polygon->updateActionStatus();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->renderGraphicsView();
		m_undoed = false;
	}
	void undo() {
		m_polygon->deselectAll();

		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
		m_polygon->m_holePolygons.removeOne(m_targetPolygon);
		m_polygon->updateActionStatus();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->renderGraphicsView();
		m_undoed = true;
	}
private:
	bool m_undoed;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleHolePolygon* m_targetPolygon;
};

void GridCreatingConditionTriangle::addHolePolygon()
{
	GridCreatingConditionTriangleHolePolygon* pol = new GridCreatingConditionTriangleHolePolygon(this);
	pol->setZDepthRange(m_depthRange.min(), m_depthRange.max());
	pol->setActive(true);
	if (m_selectedPolygon != nullptr) {
		m_selectedPolygon->setActive(false);
	}
	if (m_selectedLine != nullptr) {
		m_selectedLine->setActive(false);
		m_selectedLine = nullptr;
	}
	m_selectMode = smPolygon;
	m_selectedPolygon = pol;
	iRICUndoStack::instance().push(new AddHolePolygonCommand(pol, this));
	InformationDialog::information(preProcessorWindow(), GridCreatingConditionTriangle::tr("Information"), GridCreatingConditionTriangle::tr("Please define hole region. Hole region can be defined as polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gctriangle_addholepolygon");
}

class GridCreatingConditionTriangle::AddDivisionLineCommand : public QUndoCommand
{
public:
	AddDivisionLineCommand(GridCreatingConditionTriangleDivisionLine* newLine, GridCreatingConditionTriangle* pol) :
		QUndoCommand {GridCreatingConditionTriangle::tr("Add New Break Line")}
	{
		m_polygon = pol;
		m_targetLine = newLine;
		m_undoed = false;
	}
	virtual ~AddDivisionLineCommand() {
		if (m_undoed) {
//			delete m_targetLine;
		}
	}
	void redo() {
		m_polygon->deselectAll();

		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meBeforeDefining;
		m_polygon->m_selectMode = GridCreatingConditionTriangle::smLine;
		m_targetLine->setActive(true);
		m_polygon->m_selectedLine = m_targetLine;
		m_polygon->m_divisionLines.append(m_targetLine);
		m_polygon->updateActionStatus();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->renderGraphicsView();
		m_undoed = false;
	}
	void undo() {
		m_polygon->deselectAll();

		m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
		m_polygon->m_divisionLines.removeOne(m_targetLine);
		m_polygon->updateActionStatus();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->renderGraphicsView();
		m_undoed = true;
	}
private:
	bool m_undoed;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleDivisionLine* m_targetLine;
};

void GridCreatingConditionTriangle::addDivisionLine()
{
	GridCreatingConditionTriangleDivisionLine* l = new GridCreatingConditionTriangleDivisionLine(this);
	l->setZDepthRange((m_depthRange.min() + m_depthRange.max()) * 0.5, m_depthRange.max());
	l->setActive(true);
	if (m_selectedPolygon != nullptr) {
		m_selectedPolygon->setActive(false);
		m_selectedPolygon = nullptr;
	}
	if (m_selectedLine != nullptr) {
		m_selectedLine->setActive(false);
	}
	m_selectMode = smLine;
	m_selectedLine = l;
	iRICUndoStack::instance().push(new AddDivisionLineCommand(l, this));
	InformationDialog::information(preProcessorWindow(), GridCreatingConditionTriangle::tr("Information"), GridCreatingConditionTriangle::tr("Please define break line. Break line can be defined as polygonal line by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gctriangle_adddivisionline");
}

void GridCreatingConditionTriangle::deletePolygon(bool force)
{
	if (m_selectedPolygon == nullptr) {return;}
	if (! force) {
		int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to remove this polygon?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	if (dynamic_cast<GridCreatingConditionTriangleGridRegionPolygon*>(m_selectedPolygon) != nullptr) {
		delete m_gridRegionPolygon;
		m_gridRegionPolygon = new GridCreatingConditionTriangleGridRegionPolygon(this);
		m_gridRegionPolygon->setActive(true);
		m_gridRegionPolygon->setZDepthRange(m_depthRange.min(), m_depthRange.max());
		m_selectedPolygon = m_gridRegionPolygon;
		m_mouseEventMode = meBeforeDefining;
	} else if (dynamic_cast<GridCreatingConditionTriangleRemeshPolygon*>(m_selectedPolygon) != nullptr) {
		GridCreatingConditionTriangleRemeshPolygon* tmpPoly = dynamic_cast<GridCreatingConditionTriangleRemeshPolygon*>(m_selectedPolygon);
		m_remeshPolygons.removeOne(tmpPoly);
		delete m_selectedPolygon;
		m_selectedPolygon = nullptr;
		m_selectMode = smNone;
		m_mouseEventMode = meNormal;
	} else if (dynamic_cast<GridCreatingConditionTriangleHolePolygon*>(m_selectedPolygon) != nullptr) {
		GridCreatingConditionTriangleHolePolygon* tmpPoly = dynamic_cast<GridCreatingConditionTriangleHolePolygon*>(m_selectedPolygon);
		m_holePolygons.removeOne(tmpPoly);
		delete m_selectedPolygon;
		m_selectedPolygon = nullptr;
		m_selectMode = smNone;
		m_mouseEventMode = meNormal;
	}
	// This operation is not undoable.
	iRICUndoStack::instance().clear();

	updateMouseCursor(graphicsView());
	updateActionStatus();
	renderGraphicsView();
}

void GridCreatingConditionTriangle::deleteLine(bool force)
{
	if (m_selectedLine == nullptr) {return;}
	if (! force) {
		int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to remove this line?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	GridCreatingConditionTriangleDivisionLine* tmpLine = dynamic_cast<GridCreatingConditionTriangleDivisionLine*>(m_selectedLine);
	m_divisionLines.removeOne(tmpLine);
	delete m_selectedLine;
	m_selectedLine = nullptr;
	m_selectMode = smNone;
	m_mouseEventMode = meNormal;

	// This operation is not undoable.
	iRICUndoStack::instance().clear();

	updateMouseCursor(graphicsView());
	updateActionStatus();
	renderGraphicsView();
}

bool GridCreatingConditionTriangle::selectObject(QPoint point)
{
	SelectMode oldSelectMode = m_selectMode;
	GridCreatingConditionTriangleAbstractPolygon* oldSelectedPolygon = m_selectedPolygon;
	GridCreatingConditionTriangleAbstractLine* oldSelectedLine = m_selectedLine;
	deselectAll();

	double dx = point.x();
	double dy = point.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF p(dx, dy);
	QVector2D pv(dx, dy);

	double selectlimit = graphicsView()->stdRadius(5);

	// check whether the division line can be selected.
	bool selected = false;
	GridCreatingConditionTriangleAbstractLine* newSelLine = nullptr;
	for (int i = m_divisionLines.count() - 1; i >= 0 && (! selected); --i) {
		GridCreatingConditionTriangleAbstractLine* l = m_divisionLines[i];
		if (l->isEdgeSelectable(pv, selectlimit)) {
			newSelLine = l;
			selected = true;
		} else if (l->isVertexSelectable(pv, selectlimit)) {
			newSelLine = l;
			selected = true;
		}
	}
	if (newSelLine != nullptr) {
		m_selectMode = smLine;
		m_selectedLine = newSelLine;
		m_selectedLine->setActive(true);
		m_selectedPolygon = nullptr;
	}

	// find polygon that contains this point.
	GridCreatingConditionTriangleAbstractPolygon* newSelPol = nullptr;
	for (int i = m_remeshPolygons.count() - 1; i >= 0 && (! selected); --i) {
		GridCreatingConditionTriangleAbstractPolygon* pol = m_remeshPolygons[i];
		QPolygonF polF = pol->polygon();
		if (polF.count() <= 3) {
			// it contains only two points.
			if (pol->isEdgeSelectable(pv, selectlimit)) {
				newSelPol = pol;
				selected = true;
				GridCreatingConditionTriangleRemeshPolygon* rpol = m_remeshPolygons[i];
				connect(m_editMaxAreaAction, SIGNAL(triggered()), rpol, SLOT(editGridSize()));
			} else if (pol->isVertexSelectable(pv, selectlimit)) {
				newSelPol = pol;
				selected = true;
				GridCreatingConditionTriangleRemeshPolygon* rpol = m_remeshPolygons[i];
				connect(m_editMaxAreaAction, SIGNAL(triggered()), rpol, SLOT(editGridSize()));
			}
		} else {
			if (polF.containsPoint(p, Qt::OddEvenFill)) {
				newSelPol = pol;
				selected = true;
				GridCreatingConditionTriangleRemeshPolygon* rpol = m_remeshPolygons[i];
				connect(m_editMaxAreaAction, SIGNAL(triggered()), rpol, SLOT(editGridSize()));
			}
		}
	}
	for (int i = m_holePolygons.count() - 1; i >= 0 && (! selected); --i) {
		GridCreatingConditionTriangleAbstractPolygon* pol = m_holePolygons[i];
		QPolygonF polF = pol->polygon();
		if (polF.count() <= 3) {
			if (pol->isEdgeSelectable(pv, selectlimit)) {
				newSelPol = pol;
				selected = true;
			} else if (pol->isVertexSelectable(pv, selectlimit)) {
				newSelPol = pol;
				selected = true;
			}
		} else {
			if (pol->polygon().containsPoint(p, Qt::OddEvenFill)) {
				newSelPol = pol;
				selected = true;
			}
		}
	}
	if (! selected) {
		QPolygonF polF = m_gridRegionPolygon->polygon();
		if (polF.count() <= 3) {
			if (m_gridRegionPolygon->isEdgeSelectable(pv, selectlimit)) {
				newSelPol = m_gridRegionPolygon;
				selected = true;
			} else if (m_gridRegionPolygon->isVertexSelectable(pv, selectlimit)) {
				newSelPol = m_gridRegionPolygon;
				selected = true;
			}
		} else {
			if (m_gridRegionPolygon->polygon().containsPoint(p, Qt::OddEvenFill)) {
				newSelPol = m_gridRegionPolygon;
				selected = true;
			}
		}
	}
	if (newSelPol != nullptr) {
		m_selectMode = smPolygon;
		m_selectedPolygon = newSelPol;
		m_selectedPolygon->setActive(true);
		m_selectedLine = nullptr;
	}
	if (! selected) {
		m_selectMode = smNone;
		m_selectedLine = nullptr;
		m_selectedPolygon = nullptr;
	}

	if (m_selectMode != oldSelectMode) {return true;}
	if (m_selectMode == smPolygon) {
		return m_selectedPolygon != oldSelectedPolygon;
	}
	if (m_selectMode == smLine) {
		return m_selectedLine != oldSelectedLine;
	}
	return false;
}

void GridCreatingConditionTriangle::deselectAll()
{
	m_editMaxAreaAction->disconnect();
	if (m_selectMode == smPolygon) {
		if (m_selectedPolygon != nullptr) {
			m_selectedPolygon->setActive(false);
			m_selectedPolygon = nullptr;
		}
	} else if (m_selectMode == smLine) {
		if (m_selectedLine != nullptr) {
			m_selectedLine->setActive(false);
			m_selectedLine = nullptr;
		}
	}
	m_selectMode = smNone;
	Q_ASSERT(m_selectedPolygon == nullptr);
	Q_ASSERT(m_selectedLine == nullptr);
}

bool GridCreatingConditionTriangle::checkCondition()
{
	if (m_gridRegionPolygon->polygon().count() < 3) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid region polygon have to consists of more than three vertices."));
		return false;
	}
	if (iRIC::hasIntersection(m_gridRegionPolygon->polygon())) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid region polygon shape is invalid."));
		return false;
	}
	QPolygonF gridPol = m_gridRegionPolygon->polygon();
	QList<QPolygonF> polygons;
	for (int i = 0; i < m_remeshPolygons.count(); ++i) {
		GridCreatingConditionTriangleRemeshPolygon* rpol = m_remeshPolygons[i];
		if (rpol->polygon().count() < 3) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Remesh polygon have to consists of more than three vertices"));
			return false;
		}
		if (iRIC::hasIntersection(rpol->polygon())) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Remesh polygon shape is invalid."));
			return false;
		}
		if (gridPol.intersected(rpol->polygon()) != rpol->polygon()) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Remesh polygon have to be inside grid region."));
			return false;
		}
		polygons.append(rpol->polygon());
	}
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		GridCreatingConditionTriangleHolePolygon* hpol = m_holePolygons[i];
		if (hpol->polygon().count() < 3) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon have to consists of more than three vertices"));
			return false;
		}
		if (iRIC::hasIntersection(hpol->polygon())) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon shape is invalid."));
			return false;
		}
		if (gridPol.intersected(hpol->polygon()) != hpol->polygon()) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon have to be inside grid region."));
			return false;
		}
		polygons.append(hpol->polygon());
	}
	for (int i = 0; i < polygons.count(); ++i) {
		for (int j = i + 1; j < polygons.count(); ++j) {
			QPolygonF pol1 = polygons[i];
			QPolygonF pol2 = polygons[j];
			if (! pol1.intersected(pol2).isEmpty()) {
				// intersects!
				QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Remesh polygons and hole polygons can not have intersections."));
				return false;
			}
		}
	}
	for (int i = 0; i < m_divisionLines.count(); ++i) {
		GridCreatingConditionTriangleDivisionLine* line = m_divisionLines[i];
		QVector<QPointF> l = line->polyLine();
		if (l.count() < 2) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Break line have to consists of more more than two vertices"));
			return false;
		}
		for (int j = 0; j < l.count(); ++j) {
			if (! gridPol.containsPoint(l[j], Qt::OddEvenFill)) {
				QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Break line have to be inside grid region."));
				return false;
			}
		}
		for (int j = 0; j < l.count() - 1; ++j) {
			QLineF tmpline(l[j], l[j + 1]);
			for (int k = 0; k < polygons.count(); ++k) {
				QPolygonF pol = polygons[k];
				for (int m = 0; m < pol.count() - 1; ++m) {
					QLineF tmpline2(pol[m], pol[m + 1]);
					if (QLineF::BoundedIntersection == tmpline.intersect(tmpline2, 0)) {
						QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Break line have to have no intersection with any region edges."));
						return false;
					}
				}
			}
			for (int k = i + 1; k < m_divisionLines.count(); ++ k) {
				QVector<QPointF> l2 = m_divisionLines[k]->polyLine();
				for (int m = 0; m < l2.count() - 1; ++m) {
					QLineF tmpline2(l2[m], l2[m + 1]);
					if (QLineF::BoundedIntersection == tmpline.intersect(tmpline2, 0)) {
						QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Break line have to have no intersection with other break lines."));
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool GridCreatingConditionTriangle::activePolygonHasFourVertices()
{
	if (m_selectedPolygon == nullptr) {return false;}
	QPolygonF pol = m_selectedPolygon->polygon();
	return pol.count() > 4;
}

bool GridCreatingConditionTriangle::activePolylineHasThreeVertices()
{
	if (m_selectedLine == nullptr) {return false;}
	QVector<QPointF> line = m_selectedLine->polyLine();
	return line.count() >= 3;
}

void GridCreatingConditionTriangle::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GridCreatingConditionTriangle::tr("Information"), GridCreatingConditionTriangle::tr("Please define grid creating region. Grid creating region can be defined as polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gctriangle_gridregion");
}

void GridCreatingConditionTriangle::doApplyOffset(double x, double y)
{
	QPolygonF region = this->m_gridRegionPolygon->polygon(QPointF(x, y));
	this->m_gridRegionPolygon->setPolygon(region);

	for (int i = 0; i < this->m_remeshPolygons.count(); ++i) {
		QPolygonF hole = this->m_remeshPolygons[i]->polygon(QPointF(x, y));
		this->m_remeshPolygons[i]->setPolygon(hole);
	}

	for (int i = 0; i < this->m_holePolygons.count(); ++i) {
		QPolygonF hole = this->m_holePolygons[i]->polygon(QPointF(x, y));
		this->m_holePolygons[i]->setPolygon(hole);
	}

	for (int i = 0; i < this->m_divisionLines.count(); ++i) {
		QVector<QPointF> polyline = this->m_divisionLines[i]->polyLine(QPointF(x, y));
		this->m_divisionLines[i]->setPolyLine(polyline);
	}
	this->setModified();
}
