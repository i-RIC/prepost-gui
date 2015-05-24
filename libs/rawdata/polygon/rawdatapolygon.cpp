#define REAL double
#define VOID void

#include "rawdatapolygon.h"
#include "rawdatapolygonabstractpolygon.h"
#include "rawdatapolygoncoordinateseditdialog.h"
#include "rawdatapolygonholepolygon.h"
#include "rawdatapolygonproxy.h"
#include "rawdatapolygonregionpolygon.h"
#include "rawdatapolygontrianglethread.h"

#include <iriclib_polygon.h>

#include <guibase/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioneditdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/errormessage.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include <misc/zdepthrange.h>
#include <triangle/triangle.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensionscontainer.h>

#include <QAction>
#include <QCoreApplication>
#include <QFile>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPolygonF>
#include <QTextStream>
#include <QToolBar>
#include <QUndoCommand>
#include <QVector>
#include <QVector2D>
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

RawDataPolygon::RawDataPolygon(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
	: RawData(d, creator, condition)
{
	initParams();

	m_shapeUpdating = false;
	m_triangleThread = nullptr;

	m_gridRegionPolygon = new RawDataPolygonRegionPolygon(this);
	m_gridRegionPolygon->setSelected(true);
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		m_gridRegionPolygon->setLookupTable(stcc->vtkDarkObj());
	}
	m_gridRegionPolygon->setMapping(m_mapping);
	m_gridRegionPolygon->setColor(m_color);
	m_selectMode = smPolygon;
	m_selectedPolygon = m_gridRegionPolygon;

	m_editValueAction = new QAction(tr("Edit &Value..."), this);
	connect(m_editValueAction, SIGNAL(triggered()), this, SLOT(editValue()));
	m_copyAction = new QAction(tr("&Copy..."), this);
	connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
	m_addVertexAction = new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), tr("&Add Vertex"), this);
	m_addVertexAction->setCheckable(true);
	connect(m_addVertexAction, SIGNAL(triggered(bool)), this, SLOT(addVertexMode(bool)));
	m_removeVertexAction = new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), tr("&Remove Vertex"), this);
	m_removeVertexAction->setCheckable(true);
	connect(m_removeVertexAction, SIGNAL(triggered(bool)), this, SLOT(removeVertexMode(bool)));
	m_coordEditAction = new QAction(tr("Edit &Coordinates..."), this);
	connect(m_coordEditAction, SIGNAL(triggered()), this, SLOT(editCoordinates()));
	m_holeModeAction = new QAction(QIcon(":/libs/guibase/images/iconPolygonHole.png"), tr("Add &Hole Region"), this);
	m_holeModeAction->setCheckable(true);
	m_holeModeAction->setDisabled(true);
	connect(m_holeModeAction, SIGNAL(triggered()), this, SLOT(addHolePolygon()));

	m_deleteAction = new QAction(tr("&Delete Hole Region..."), this);
	m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deletePolygon()));
	m_editColorSettingAction = new QAction(tr("Color &Setting..."), this);
	connect(m_editColorSettingAction, SIGNAL(triggered()), this, SLOT(editColorSetting()));

	// Set cursors for mouse view change events.
	m_addPixmap = QPixmap(":/libs/guibase/images/cursorAdd.png");
	m_removePixmap = QPixmap(":/libs/guibase/images/cursorRemove.png");
	m_movePointPixmap = QPixmap(":/libs/guibase/images/cursorOpenHandPoint.png");
	m_addCursor = QCursor(m_addPixmap, 0, 0);
	m_removeCursor = QCursor(m_removePixmap, 0, 0);
	m_movePointCursor = QCursor(m_movePointPixmap);

	m_mouseEventMode = meBeforeDefining;

	m_grid = vtkSmartPointer<vtkUnstructuredGrid>::New();

	m_scalarValues = vtkSmartPointer<vtkDoubleArray>::New();
	m_scalarValues->SetName("polygonvalue");
	m_grid->GetPointData()->AddArray(m_scalarValues);
	m_grid->GetPointData()->SetActiveScalars("polygonvalue");

	m_paintMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	if (stcc != nullptr) {
		m_paintMapper->SetLookupTable(stcc->vtkObj());
	}
	m_paintMapper->SetUseLookupTableScalarRange(true);
	m_paintMapper->SetInputData(m_grid);

	m_paintActor = vtkSmartPointer<vtkActor>::New();
	m_paintActor->GetProperty()->SetLighting(false);
	m_paintActor->SetMapper(m_paintMapper);

	actorCollection()->AddItem(m_paintActor);
	renderer()->AddActor(m_paintActor);

	m_inhibitSelect = false;
	m_bcSettingMode = false;
	updateActionStatus();
}

RawDataPolygon::~RawDataPolygon()
{
	delete m_gridRegionPolygon;
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		delete m_holePolygons[i];
	}
	delete m_rightClickingMenu;

	vtkRenderer* r = renderer();
	r->RemoveActor(m_paintActor);
}

void RawDataPolygon::setupMenu()
{
	m_menu->setTitle(tr("&Polygon"));
	m_menu->addAction(m_editNameAction);
	m_menu->addAction(m_editValueAction);
	m_menu->addSeparator();
	m_menu->addAction(m_copyAction);
	m_menu->addSeparator();
	m_menu->addAction(m_addVertexAction);
	m_menu->addAction(m_removeVertexAction);
	m_menu->addAction(m_coordEditAction);
	m_menu->addSeparator();
	m_menu->addAction(m_holeModeAction);
	m_menu->addAction(m_deleteAction);
	m_menu->addSeparator();
	m_menu->addAction(m_editColorSettingAction);
	m_menu->addSeparator();
	m_menu->addAction(deleteAction());

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(m_editValueAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_copyAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addVertexAction);
	m_rightClickingMenu->addAction(m_removeVertexAction);
	m_rightClickingMenu->addAction(m_coordEditAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_holeModeAction);
	m_rightClickingMenu->addAction(m_deleteAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_editColorSettingAction);
}

bool RawDataPolygon::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(m_holeModeAction);

	tb->addSeparator();
	tb->addAction(m_addVertexAction);
	tb->addAction(m_removeVertexAction);

	tb->addSeparator();
	tb->addAction(m_deleteAction);
	return true;
}

QColor RawDataPolygon::doubleToColor(double /*d*/)
{
	return Qt::red;
}

void RawDataPolygon::setColor(double r, double g, double b)
{
	m_color = QColor((int)(r * 255), (int)(g * 255), (int)(b * 255));

	m_gridRegionPolygon->setColor(m_color);
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* hp = m_holePolygons.at(i);
		hp->setColor(m_color);
	}
	m_paintActor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
	m_paintActor->GetProperty()->SetColor(r, g, b);
}

void RawDataPolygon::setColor(const QColor& color)
{
	setColor(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
}

void RawDataPolygon::informSelection(PreProcessorGraphicsViewInterface* v)
{
	m_gridRegionPolygon->setActive(true);
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* p = m_holePolygons.at(i);
		p->setActive(true);
	}
	switch (m_selectMode) {
	case smPolygon:
		m_selectedPolygon->setSelected(true);
		break;
	case smNone:
		// do nothing.
		break;
	}
	updateMouseCursor(v);
}

void RawDataPolygon::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	m_gridRegionPolygon->setActive(false);
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* p = m_holePolygons.at(i);
		p->setActive(false);
	}
	switch (m_selectMode) {
	case smPolygon:
		m_selectedPolygon->setSelected(false);
		break;
	case smNone:
		// do nothing.
		break;
	}
	v->unsetCursor();
}

void RawDataPolygon::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

class RawDataPolygonPolygonFinishDefiningCommand : public QUndoCommand
{
public:
	RawDataPolygonPolygonFinishDefiningCommand(RawDataPolygon* polygon)
		: QUndoCommand(RawDataPolygon::tr("Finish Defining Polygon")) {
		m_polygon = polygon;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void undo() {
		m_polygon->m_mouseEventMode = RawDataPolygon::meDefining;
		m_polygon->m_selectedPolygon = m_targetPolygon;
		m_polygon->m_selectedPolygon->setSelected(true);
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->updateActionStatus();
	}
	void redo() {
		m_polygon->m_mouseEventMode = RawDataPolygon::meNormal;
		m_targetPolygon->finishDefinition();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->updateActionStatus();
	}
private:
	RawDataPolygon* m_polygon;
	RawDataPolygonAbstractPolygon* m_targetPolygon;
};

void RawDataPolygon::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->key() == Qt::Key_Return) {
		if (m_mouseEventMode == meDefining) {
			if (m_selectMode == smPolygon) {
				definePolygon(false);
			}
		}
	}
}

void RawDataPolygon::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void RawDataPolygon::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (m_mouseEventMode == meDefining) {
		if (m_selectMode == smPolygon) {
			definePolygon(true);
		}
	}
}

class RawDataPolygonPolygonDefineNewPointCommand : public QUndoCommand
{
public:
	RawDataPolygonPolygonDefineNewPointCommand(bool keyDown, const QPoint& point, RawDataPolygon* pol)
		: QUndoCommand(RawDataPolygon::tr("Add New Polygon Point")) {
		m_keyDown = keyDown;
		double dx = point.x();
		double dy = point.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		m_newPoint = QVector2D(dx, dy);
		m_polygon = pol;
		m_oldMapped = m_polygon->m_mapped;
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
		m_polygon->m_mapped = false;
		m_polygon->m_shapeUpdating = true;
		m_targetPolygon->updateShapeData();
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
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
		m_polygon->m_mapped = m_oldMapped;
		m_polygon->m_shapeUpdating = true;
		m_targetPolygon->updateShapeData();
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
	}
	int id() const {
		return iRIC::generateCommandId("RawDataPolygonPolygonDefineNewPoint");
	}
	bool mergeWith(const QUndoCommand* other) {
		const RawDataPolygonPolygonDefineNewPointCommand* comm = dynamic_cast<const RawDataPolygonPolygonDefineNewPointCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (comm->m_polygon != m_polygon) {return false;}
		if (comm->m_targetPolygon != m_targetPolygon) {return false;}
		m_newPoint = comm->m_newPoint;
		return true;
	}
private:
	bool m_keyDown;
	RawDataPolygon* m_polygon;
	RawDataPolygonAbstractPolygon* m_targetPolygon;
	QVector2D m_newPoint;
	bool m_oldMapped;
};

class RawDataPolygonPolygonMoveCommand : public QUndoCommand
{
public:
	RawDataPolygonPolygonMoveCommand(bool keyDown, const QPoint& from, const QPoint& to, RawDataPolygon* pol)
		: QUndoCommand(RawDataPolygon::tr("Move Polygon")) {
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
		m_oldMapped = pol->m_mapped;
		m_polygon = pol;
	}
	void redo() {
		m_polygon->m_mapped = false;
		m_polygon->m_shapeUpdating = true;
		movePolygon(m_polygon->m_gridRegionPolygon, m_offset);
		for (int i = 0; i < m_polygon->m_holePolygons.count(); ++i) {
			RawDataPolygonHolePolygon* hp = m_polygon->m_holePolygons.at(i);
			movePolygon(hp, m_offset);
		}
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
	}
	void undo() {
		m_polygon->m_mapped = m_oldMapped;
		m_polygon->m_shapeUpdating = true;
		movePolygon(m_polygon->m_gridRegionPolygon, - m_offset);
		for (int i = 0; i < m_polygon->m_holePolygons.count(); ++i) {
			RawDataPolygonHolePolygon* hp = m_polygon->m_holePolygons.at(i);
			movePolygon(hp, - m_offset);
		}
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
	}
	void movePolygon(RawDataPolygonAbstractPolygon* polygon, const QVector2D& offset) {
		vtkPolygon* pol = polygon->getVtkPolygon();
		vtkPoints* points = pol->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			double p[3];
			points->GetPoint(i, p);
			p[0] += offset.x();
			p[1] += offset.y();
			points->SetPoint(i, p);
		}
		points->Modified();
		pol->Modified();
		polygon->updateShapeData();
	}
	int id() const {
		return iRIC::generateCommandId("RawDataPolygonPolygonMove");
	}
	bool mergeWith(const QUndoCommand* other) {
		const RawDataPolygonPolygonMoveCommand* comm = dynamic_cast<const RawDataPolygonPolygonMoveCommand*>(other);
		if (comm == nullptr) {return false;}
		if (comm->m_keyDown) {return false;}
		if (comm->m_polygon != m_polygon) {return false;}
		m_offset += comm->m_offset;
		return true;
	}
private:
	bool m_keyDown;
	RawDataPolygon* m_polygon;
	QVector2D m_offset;
	bool m_oldMapped;
};

class RawDataPolygonPolygonMoveVertexCommand : public QUndoCommand
{
public:
	RawDataPolygonPolygonMoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, RawDataPolygon* pol)
		: QUndoCommand(RawDataPolygon::tr("Move Polygon Vertex")) {
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
		m_oldMapped = pol->m_mapped;
		m_polygon = pol;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void redo() {
		m_polygon->m_mapped = false;
		m_polygon->m_shapeUpdating = true;
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
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
	}
	void undo() {
		m_polygon->m_mapped = m_oldMapped;
		m_polygon->m_shapeUpdating = true;
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
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
	}
	int id() const {
		return iRIC::generateCommandId("RawDataPolygonPolygonMoveVertex");
	}
	bool mergeWith(const QUndoCommand* other) {
		const RawDataPolygonPolygonMoveVertexCommand* comm = dynamic_cast<const RawDataPolygonPolygonMoveVertexCommand*>(other);
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
	RawDataPolygon* m_polygon;
	RawDataPolygonAbstractPolygon* m_targetPolygon;
	QVector2D m_offset;
	bool m_oldMapped;
};

class RawDataPolygonPolygonAddVertexCommand : public QUndoCommand
{
public:
	RawDataPolygonPolygonAddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, RawDataPolygon* pol)
		: QUndoCommand(RawDataPolygon::tr("Insert Polygon Vertex")) {
		m_keyDown = keyDown;
		m_vertexId = (edgeId + 1) % (pol->m_selectedPolygon->getVtkPolygon()->GetNumberOfPoints() + 1);
		double dx = point.x();
		double dy = point.y();
		pol->graphicsView()->viewportToWorld(dx, dy);
		m_vertexPosition = QVector2D(dx, dy);
		m_oldMapped = pol->m_mapped;
		m_polygon = pol;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void redo() {
		m_polygon->m_mapped = false;
		m_polygon->m_shapeUpdating = true;
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
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
	}
	void undo() {
		m_polygon->m_mapped = m_oldMapped;
		if (m_keyDown) {
			m_polygon->m_shapeUpdating = true;
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
			m_polygon->m_shapeUpdating = false;
			m_polygon->renderGraphicsView();
			m_polygon->updateGrid();
		} else {
			// this never happens.
		}
	}
	int id() const {
		return iRIC::generateCommandId("RawDataPolygonPolygonAddVertex");
	}
	bool mergeWith(const QUndoCommand* other) {
		const RawDataPolygonPolygonAddVertexCommand* comm = dynamic_cast<const RawDataPolygonPolygonAddVertexCommand*>(other);
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
	RawDataPolygon* m_polygon;
	RawDataPolygonAbstractPolygon* m_targetPolygon;
	bool m_oldMapped;
};

void RawDataPolygon::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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
			iRICUndoStack::instance().push(new RawDataPolygonPolygonDefineNewPointCommand(false, QPoint(event->x(), event->y()), this));
		}
		break;
	case meTranslate:
		// execute translation.
		if (m_selectMode == smPolygon) {
			iRICUndoStack::instance().push(new RawDataPolygonPolygonMoveCommand(false, m_currentPoint, QPoint(event->x(), event->y()), this));
		}
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meMoveVertex:
		if (m_selectMode == smPolygon) {
			iRICUndoStack::instance().push(new RawDataPolygonPolygonMoveVertexCommand(false, m_currentPoint, QPoint(event->x(), event->y()), m_selectedPolygon->selectedVertexId(), this));
		}
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meAddVertex:
		if (m_selectMode == smPolygon) {
			iRICUndoStack::instance().push(new RawDataPolygonPolygonAddVertexCommand(false, m_selectedPolygon->selectedEdgeId(), QPoint(event->x(), event->y()), this));
		}
		break;
	case meTranslateDialog:
		break;
	case meEditVerticesDialog:
		break;
	}
}

class RawDataPolygonPolygonRemoveVertexCommand : public QUndoCommand
{
public:
	RawDataPolygonPolygonRemoveVertexCommand(vtkIdType vertexId, RawDataPolygon* pol)
		: QUndoCommand(RawDataPolygon::tr("Remove Polygon Vertex")) {
		m_vertexId = vertexId;
		double p[3];
		pol->m_selectedPolygon->getVtkPolygon()->GetPoints()->GetPoint(m_vertexId, p);
		m_vertexPosition = QVector2D(p[0], p[1]);
		m_oldMapped = pol->m_mapped;
		m_polygon = pol;
		m_targetPolygon = m_polygon->m_selectedPolygon;
	}
	void redo() {
		m_polygon->m_mapped = false;
		m_polygon->m_shapeUpdating = true;
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
		m_polygon->m_mouseEventMode = RawDataPolygon::meNormal;
		m_targetPolygon->getVtkPolygon()->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
	}
	void undo() {
		m_polygon->m_mapped = m_oldMapped;
		m_polygon->m_shapeUpdating = false;
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
		m_polygon->m_shapeUpdating = false;
		m_polygon->renderGraphicsView();
		m_polygon->updateGrid();
	}
private:
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	RawDataPolygon* m_polygon;
	RawDataPolygonAbstractPolygon* m_targetPolygon;
	bool m_oldMapped;
};

void RawDataPolygon::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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
				iRICUndoStack::instance().push(new RawDataPolygonPolygonDefineNewPointCommand(true, QPoint(event->x(), event->y()), this));
			}
		case meDefining:
			if (m_selectMode == smPolygon) {
				iRICUndoStack::instance().push(new RawDataPolygonPolygonDefineNewPointCommand(true, QPoint(event->x(), event->y()), this));
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
					iRICUndoStack::instance().push(new RawDataPolygonPolygonMoveCommand(true, m_currentPoint, m_currentPoint, this));
				}
			}
			break;
		case meMoveVertexPrepare:
			m_mouseEventMode = meMoveVertex;
			m_currentPoint = QPoint(event->x(), event->y());
			// push the first move command.
			if (m_selectMode == smPolygon) {
				iRICUndoStack::instance().push(new RawDataPolygonPolygonMoveVertexCommand(true, m_currentPoint, m_currentPoint, m_selectedPolygon->selectedVertexId(), this));
			}
			break;
		case meAddVertexPrepare:
			m_mouseEventMode = meAddVertex;
			if (m_selectMode == smPolygon) {
				iRICUndoStack::instance().push(new RawDataPolygonPolygonAddVertexCommand(true, m_selectedPolygon->selectedEdgeId(), QPoint(event->x(), event->y()), this));
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
					iRICUndoStack::instance().push(new RawDataPolygonPolygonRemoveVertexCommand(m_selectedPolygon->selectedVertexId(), this));
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

void RawDataPolygon::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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
		if (isNear(m_dragStartPoint, QPoint(event->x(), event->y()))) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void RawDataPolygon::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
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
		v->setCursor(Qt::OpenHandCursor);
		break;
	case meMoveVertexPrepare:
		v->setCursor(m_movePointCursor);
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

void RawDataPolygon::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
	menu->addAction(m_editValueAction);
	menu->addSeparator();
	menu->addAction(m_copyAction);
}

void RawDataPolygon::definePolygon(bool doubleClick, bool noEditVal)
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
	stack.push(new RawDataPolygonPolygonFinishDefiningCommand(this));
	stack.endMacro();
	if (m_selectedPolygon == m_gridRegionPolygon && (! noEditVal)) {
		editValue();
	}
}

void RawDataPolygon::addVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meAddVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void RawDataPolygon::removeVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meRemoveVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void RawDataPolygon::doLoadFromProjectMainFile(const QDomNode& node)
{
	RawData::doLoadFromProjectMainFile(node);
	m_opacityPercent = loadOpacityPercent(node);
	m_paintActor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
	QString mapping = node.toElement().attribute("mapping", "arbitrary");
	if (mapping == "arbitrary") {
		m_mapping = RawDataPolygonColorSettingDialog::Arbitrary;
	} else {
		m_mapping = RawDataPolygonColorSettingDialog::Value;
	}
	m_color = loadColorAttribute("color", node, Qt::red);
}

void RawDataPolygon::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	RawData::doSaveToProjectMainFile(writer);
	writeOpacityPercent(m_opacityPercent, writer);
	QString mapping;
	if (m_mapping == RawDataPolygonColorSettingDialog::Arbitrary) {
		mapping = "arbitrary";
	} else {
		mapping = "value";
	}
	writer.writeAttribute("mapping", mapping);
	writeColorAttribute("color", m_color, writer);
}

void RawDataPolygon::loadExternalData(const QString& filename)
{
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		m_gridRegionPolygon->setLookupTable(stcc->vtkDarkObj());
	}
	m_gridRegionPolygon->setMapping(m_mapping);
	m_gridRegionPolygon->setColor(m_color);

	if (projectData()->version().build() >= 3607) {
		iRICLib::Polygon* pol = new iRICLib::Polygon();
		GridRelatedConditionDimensionsContainer* dims = dimensions();
		bool noDim = true;
		if (dims != nullptr) {
			noDim = dims->containers().size() == 0;
		}

		pol->load(iRIC::toStr(filename).c_str(), noDim);
		m_variantValues.clear();
		for (unsigned int i = 0; i < pol->values.size(); ++i) {
			m_variantValues.append(pol->values[i]);
		}
		QPolygonF qpol;
		iRICLib::InternalPolygon* regionPolygon = pol->polygon;
		for (int i = 0; i < regionPolygon->pointCount; ++i) {
			qpol << QPointF(*(regionPolygon->x + i), *(regionPolygon->y + i));
		}
		m_gridRegionPolygon->setPolygon(qpol);

		for (unsigned int i = 0; i < pol->holes.size(); ++i) {
			qpol.clear();
			iRICLib::InternalPolygon* holePolygon = pol->holes.at(i);
			for (int j = 0; j < holePolygon->pointCount; ++j) {
				qpol << QPointF(*(holePolygon->x + j), *(holePolygon->y + j));
			}
			RawDataPolygonHolePolygon* tmpPol = new RawDataPolygonHolePolygon(this);
			setupHolePolygon(tmpPol);
			tmpPol->setPolygon(qpol);
			tmpPol->setActive(false);
			tmpPol->setSelected(false);
			m_holePolygons.append(tmpPol);
		}
		delete pol;
	} else {
		QPolygonF poly;
		QFile f(filename);
		f.open(QIODevice::ReadOnly);
		QDataStream s(&f);
		if (projectData()->version().build() >= 2870) {
			s >> poly;
		} else {
			while (! s.atEnd()) {
				double a, b;
				s >> a >> b;
				poly << QPointF(a, b);
			}
			poly << poly.at(0);
		}
		m_gridRegionPolygon->setPolygon(poly);

		// for newer than 2870, holes are supported.
		if (projectData()->version().build() >= 2870) {
			int holePolygons;
			s >> holePolygons;
			for (int i = 0; i < holePolygons; ++i) {
				QPolygonF pol;
				s >> pol;
				RawDataPolygonHolePolygon* tmpPol = new RawDataPolygonHolePolygon(this);
				setupHolePolygon(tmpPol);
				tmpPol->setPolygon(pol);
				tmpPol->setActive(false);
				tmpPol->setSelected(false);
				m_holePolygons.append(tmpPol);
			}
		}
		f.close();
	}

	if (m_gridRegionPolygon->polygon().size() > 0) {
		m_mouseEventMode = meNormal;
		informDeselection(graphicsView());
	} else {
		m_mouseEventMode = meBeforeDefining;
		m_selectMode = smPolygon;
		m_selectedPolygon = m_gridRegionPolygon;
	}
	deselectAll();
	updateGrid(true);
	updateActionStatus();
}

void RawDataPolygon::saveExternalData(const QString& filename)
{
	iRICLib::Polygon* pol = new iRICLib::Polygon();
	pol->values.clear();
	for (int i = 0; i < m_variantValues.size(); ++i) {
		pol->values.push_back(m_variantValues.at(i).toDouble());
	}
	iRICLib::InternalPolygon* regionPolygon = new iRICLib::InternalPolygon();
	QPolygonF qpol = polygon();
	regionPolygon->pointCount = qpol.count();
	regionPolygon->x = new double[regionPolygon->pointCount];
	regionPolygon->y = new double[regionPolygon->pointCount];
	for (int i = 0; i < regionPolygon->pointCount; ++i) {
		*(regionPolygon->x + i) = qpol.at(i).x();
		*(regionPolygon->y + i) = qpol.at(i).y();
	}
	pol->polygon = regionPolygon;
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		iRICLib::InternalPolygon* holePolygon = new iRICLib::InternalPolygon();
		QPolygonF hqpol = m_holePolygons[i]->polygon();
		holePolygon->pointCount = hqpol.count();
		holePolygon->x = new double[regionPolygon->pointCount];
		holePolygon->y = new double[regionPolygon->pointCount];
		for (int j = 0; j < holePolygon->pointCount; ++j) {
			*(holePolygon->x + j) = hqpol.at(j).x();
			*(holePolygon->y + j) = hqpol.at(j).y();
		}
		pol->holes.push_back(holePolygon);
	}
	GridRelatedConditionDimensionsContainer* dims = dimensions();
	bool noDim = true;
	if (dims != nullptr) {
		noDim = dims->containers().size() == 0;
	}
	pol->save(iRIC::toStr(filename).c_str(), noDim);
	delete pol;
}

void RawDataPolygon::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void RawDataPolygon::assignActorZValues(const ZDepthRange& range)
{
	m_depthRange = range;
	m_gridRegionPolygon->setZDepthRange(range.min(), range.max());
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* p = m_holePolygons[i];
		p->setZDepthRange(range.min(), range.max());
	}
	m_paintActor->SetPosition(0, 0, range.min());
}

void RawDataPolygon::updateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QVector2D worldPos(dx, dy);
	bool shapeUpdating = m_shapeUpdating;
	shapeUpdating = shapeUpdating || (m_triangleThread != nullptr && m_triangleThread->isOutputting());
	switch (m_mouseEventMode) {
	case meAddVertexNotPossible:
	case meAddVertexPrepare:
		if (m_selectMode == smNone) {return;}
		if (m_selectMode == smPolygon) {
			if (shapeUpdating) {
				m_mouseEventMode = meAddVertexNotPossible;
			} else if (m_selectedPolygon->isEdgeSelectable(worldPos, graphicsView()->stdRadius(5))) {
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
			if (shapeUpdating) {
				m_mouseEventMode = meRemoveVertexNotPossible;
			} else if (m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(5))) {
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
			if (shapeUpdating) {
				m_mouseEventMode = meNormal;
			} else if (m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(5))) {
				m_mouseEventMode = meMoveVertexPrepare;
			} else if (m_selectedPolygon == m_gridRegionPolygon && m_selectedPolygon->isPolygonSelectable(worldPos)) {
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

void RawDataPolygon::updateActionStatus()
{
	switch (m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);

		m_holeModeAction->setDisabled(true);
		m_holeModeAction->setChecked(false);
		m_deleteAction->setDisabled(true);
		if (dynamic_cast<RawDataPolygonRegionPolygon*>(m_selectedPolygon) != nullptr) {
//			m_defineModeAction->setChecked(true);
		} else if (dynamic_cast<RawDataPolygonHolePolygon*>(m_selectedPolygon) != nullptr) {
			m_holeModeAction->setChecked(true);
		}
		break;
	case meTranslate:
	case meMoveVertex:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setDisabled(true);

		m_holeModeAction->setDisabled(true);
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
			}
			m_coordEditAction->setEnabled(true);
		} else {
			m_addVertexAction->setDisabled(true);
			m_removeVertexAction->setDisabled(true);
			m_coordEditAction->setDisabled(true);
		}

		m_holeModeAction->setEnabled(true);
		m_holeModeAction->setChecked(false);
		if (m_selectedPolygon != nullptr) {
			m_addVertexAction->setEnabled(true);
			m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
			m_coordEditAction->setEnabled(true);
			m_deleteAction->setEnabled(m_selectedPolygon != m_gridRegionPolygon);
		} else {
			m_addVertexAction->setDisabled(true);
			m_removeVertexAction->setDisabled(true);
			m_coordEditAction->setDisabled(true);
			m_deleteAction->setDisabled(true);
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
			}
			m_coordEditAction->setEnabled(true);
		} else {
			m_addVertexAction->setDisabled(true);
			m_removeVertexAction->setDisabled(true);
			m_coordEditAction->setDisabled(true);
		}
		m_holeModeAction->setDisabled(true);
		m_holeModeAction->setChecked(false);
		m_deleteAction->setEnabled(m_selectedPolygon != m_gridRegionPolygon);

		break;
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(true);
		m_coordEditAction->setEnabled(false);

		m_holeModeAction->setDisabled(true);
		m_holeModeAction->setChecked(false);
		m_deleteAction->setEnabled(m_selectedPolygon != m_gridRegionPolygon);
		break;
	case meTranslateDialog:
	case meEditVerticesDialog:
		break;
	}
}

void RawDataPolygon::editCoordinates()
{
	m_mouseEventMode = meEditVerticesDialog;
	if (m_selectedPolygon != nullptr) {
		RawDataPolygonCoordinatesEditDialog* dialog = new RawDataPolygonCoordinatesEditDialog(this, preProcessorWindow());
		dialog->show();
		iricMainWindow()->enterModelessDialogMode();
		connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
		connect(dialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
		connect(dialog, SIGNAL(destroyed()), this, SLOT(updateGrid()));
	}
}

void RawDataPolygon::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

void RawDataPolygon::clear()
{
	initParams();
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		delete m_holePolygons[i];
	}
	m_holePolygons.clear();
	delete m_gridRegionPolygon;
	m_gridRegionPolygon = new RawDataPolygonRegionPolygon(this);
	m_gridRegionPolygon->setSelected(true);
	m_gridRegionPolygon->setZDepthRange(m_depthRange.min(), m_depthRange.max());
	m_mouseEventMode = meBeforeDefining;
	m_selectedPolygon = m_gridRegionPolygon;
	updateMouseCursor(graphicsView());
	updateActionStatus();
	renderGraphicsView();
}

void RawDataPolygon::initParams()
{
	m_variantValues.clear();

	int maxIndex = 1;
	GridRelatedConditionDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		maxIndex = dimensions()->maxIndex();
	}
	for (int i = 0; i <= maxIndex; ++i) {
		m_variantValues.append(0);
	}
	m_opacityPercent = 50;
	m_mapping = RawDataPolygonColorSettingDialog::Value;
}

class RawDataPolygonAddHolePolygonCommand : public QUndoCommand
{
public:
	RawDataPolygonAddHolePolygonCommand(RawDataPolygonHolePolygon* newPoly, RawDataPolygon* pol)
		: QUndoCommand(RawDataPolygon::tr("Add New Hole Polygon")) {
		m_polygon = pol;
		m_targetPolygon = newPoly;
		m_undoed = false;
		m_oldMapped = pol->m_mapped;
	}
	virtual ~RawDataPolygonAddHolePolygonCommand() {
		if (m_undoed) {
//			delete m_targetPolygon;
		}
	}
	void redo() {
		m_polygon->m_mapped = false;
		m_polygon->deselectAll();

		m_polygon->m_mouseEventMode = RawDataPolygon::meBeforeDefining;
		m_polygon->m_selectMode = RawDataPolygon::smPolygon;
		m_targetPolygon->setSelected(true);
		m_polygon->m_selectedPolygon = m_targetPolygon;
		m_polygon->m_holePolygons.append(m_targetPolygon);
		m_polygon->updateActionStatus();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->renderGraphicsView();
		m_undoed = false;
	}
	void undo() {
		m_polygon->m_mapped = m_oldMapped;
		m_polygon->deselectAll();
		m_polygon->m_mouseEventMode = RawDataPolygon::meNormal;
		m_polygon->m_holePolygons.removeOne(m_targetPolygon);
		m_polygon->updateActionStatus();
		m_polygon->updateMouseCursor(m_polygon->graphicsView());
		m_polygon->renderGraphicsView();
		m_undoed = true;
	}
private:
	bool m_undoed;
	RawDataPolygon* m_polygon;
	RawDataPolygonHolePolygon* m_targetPolygon;
	bool m_oldMapped;
};

void RawDataPolygon::addHolePolygon()
{
	RawDataPolygonHolePolygon* pol = new RawDataPolygonHolePolygon(this);
	setupHolePolygon(pol);
	pol->setSelected(true);
	if (m_selectedPolygon != nullptr) {
		m_selectedPolygon->setSelected(false);
	}
	m_selectMode = smPolygon;
	m_selectedPolygon = pol;
	iRICUndoStack::instance().push(new RawDataPolygonAddHolePolygonCommand(pol, this));
	InformationDialog::information(preProcessorWindow(), RawDataPolygon::tr("Information"), RawDataPolygon::tr("Please define hole region. Hole region can be defined as polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gctriangle_addholepolygon");
}

void RawDataPolygon::setupHolePolygon(RawDataPolygonHolePolygon* pol)
{
	pol->setZDepthRange(m_depthRange.min(), m_depthRange.max());
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		pol->setLookupTable(scalarsToColorsContainer()->vtkDarkObj());
	}
	pol->setActive(true);
	pol->setMapping(m_mapping);
	pol->setColor(m_color);
}

void RawDataPolygon::deletePolygon(bool force)
{
	if (m_selectedPolygon == nullptr) {return;}
	if (! force) {
		int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to remove this polygon?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	if (dynamic_cast<RawDataPolygonRegionPolygon*>(m_selectedPolygon) != nullptr) {
		delete m_gridRegionPolygon;
		m_gridRegionPolygon = new RawDataPolygonRegionPolygon(this);
		m_gridRegionPolygon->setActive(true);
		m_gridRegionPolygon->setSelected(true);
		m_gridRegionPolygon->setZDepthRange(m_depthRange.min(), m_depthRange.max());
		m_selectedPolygon = m_gridRegionPolygon;
		m_mouseEventMode = meBeforeDefining;
	} else if (dynamic_cast<RawDataPolygonHolePolygon*>(m_selectedPolygon) != nullptr) {
		RawDataPolygonHolePolygon* tmpPoly = dynamic_cast<RawDataPolygonHolePolygon*>(m_selectedPolygon);
		m_holePolygons.removeOne(tmpPoly);
		delete m_selectedPolygon;
		m_selectedPolygon = nullptr;
		m_selectMode = smNone;
		m_mouseEventMode = meNormal;
	}
	// This operation is not undoable.
	iRICUndoStack::instance().clear();
	m_mapped = false;

	updateMouseCursor(graphicsView());
	updateGrid();
	updateActionStatus();
	renderGraphicsView();
}

bool RawDataPolygon::selectObject(QPoint point)
{
	SelectMode oldSelectMode = m_selectMode;
	RawDataPolygonAbstractPolygon* oldSelectedPolygon = m_selectedPolygon;
	deselectAll();

	double dx = point.x();
	double dy = point.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF p(dx, dy);
	QVector2D pv(dx, dy);

	double selectlimit = graphicsView()->stdRadius(5);

	// find polygon that contains this point.
	RawDataPolygonAbstractPolygon* newSelPol = nullptr;
	bool selected = false;
	for (int i = m_holePolygons.count() - 1; i >= 0 && (! selected); --i) {
		RawDataPolygonAbstractPolygon* pol = m_holePolygons[i];
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
		m_selectedPolygon->setSelected(true);
	}
	if (! selected) {
		m_selectMode = smNone;
		m_selectedPolygon = nullptr;
	}

	if (m_selectMode != oldSelectMode) {return true;}
	if (m_selectMode == smPolygon) {
		return m_selectedPolygon != oldSelectedPolygon;
	}
	return false;
}

void RawDataPolygon::deselectAll()
{
//	m_editMaxAreaAction->disconnect();
	if (m_selectMode == smPolygon) {
		if (m_selectedPolygon != nullptr) {
			m_selectedPolygon->setSelected(false);
			m_selectedPolygon = nullptr;
		}
	}
	m_selectMode = smNone;
}

bool RawDataPolygon::checkCondition()
{
	if (m_gridRegionPolygon->polygon().count() < 4) {
//		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid region polygon have to consists of more than three vertices."));
		return false;
	}
	if (iRIC::hasIntersection(m_gridRegionPolygon->polygon())) {
//		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid region polygon shape is invalid."));
		return false;
	}
	QPolygonF gridPol = m_gridRegionPolygon->polygon();
	QList<QPolygonF> polygons;
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* hpol = m_holePolygons[i];
		if (hpol->polygon().count() < 4) {
//			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon have to consists of more than three vertices"));
			return false;
		}
		if (iRIC::hasIntersection(hpol->polygon())) {
//			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon shape is invalid."));
			return false;
		}
		if (gridPol.intersected(hpol->polygon()) != hpol->polygon()) {
//			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon have to be inside grid region."));
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
//				QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Remesh polygons and hole polygons can not have intersections."));
				return false;
			}
		}
	}
	return true;
}

bool RawDataPolygon::activePolygonHasFourVertices()
{
	if (m_selectedPolygon == nullptr) {return false;}
	QPolygonF pol = m_selectedPolygon->polygon();
	return pol.count() > 4;
}

void RawDataPolygon::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), RawDataPolygon::tr("Information"), RawDataPolygon::tr("Please define polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "rawdatapolygoninit");
}

class RawDataPolygonEditValueCommand : public QUndoCommand
{
public:
	RawDataPolygonEditValueCommand(RawDataPolygon* polygon, const QVariant& oldvalue, const QVariant& newvalue)
		: QUndoCommand(QObject::tr("Polygon value change")) {
		m_polygon = polygon;
		m_oldValue = oldvalue;
		m_newValue = newvalue;
		m_oldMapped = polygon->m_mapped;
	}
	void redo() {
		m_polygon->setVariantValue(m_newValue);
		m_polygon->m_mapped = false;
	}
	void undo() {
		m_polygon->setVariantValue(m_oldValue);
		m_polygon->m_mapped = m_oldMapped;
	}
private:
	RawDataPolygon* m_polygon;
	QVariant m_oldValue;
	QVariant m_newValue;
	bool m_oldMapped;
};


const QVariant& RawDataPolygon::variantValue() const
{
	int index = 0;
	GridRelatedConditionDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	return m_variantValues.at(index);
}

void RawDataPolygon::setVariantValue(const QVariant& v)
{
	int index = 0;
	GridRelatedConditionDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	m_variantValues[index] = v;
	updateScalarValues();
	dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent())->informValueRangeChange();
	dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent())->informDataChange();
}

void RawDataPolygon::editValue()
{
	GridRelatedConditionEditDialog* dialog = m_gridRelatedCondition->editDialog(preProcessorWindow());
	PreProcessorRawDataGroupDataItemInterface* i = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setLabel(tr("Please input new value in this polygon.").arg(i->condition()->caption()));
	i->setupEditWidget(dialog->widget());
	dialog->setVariantValue(variantValue());
	if (m_mouseEventMode == meDefining || m_mouseEventMode == meBeforeDefining) {
		dialog->inhibitCancel();
	}
	int ret = dialog->exec();
	if (ret == QDialog::Accepted) {
		iRICUndoStack::instance().push(new RawDataPolygonEditValueCommand(this, variantValue(), dialog->variantValue()));
	}
}

void RawDataPolygon::setPolygon(const QPolygonF& p)
{
	m_gridRegionPolygon->setPolygon(p);
}

void RawDataPolygon::addHolePolygon(const QPolygonF& p)
{
	RawDataPolygonHolePolygon* tmpPol = new RawDataPolygonHolePolygon(this);
	setupHolePolygon(tmpPol);
	tmpPol->setSelected(false);
	tmpPol->setPolygon(p);
	m_holePolygons.append(tmpPol);
}

const QPolygonF RawDataPolygon::polygon() const
{
	return m_gridRegionPolygon->polygon();
}

void RawDataPolygon::updateScalarValues()
{
	vtkPoints* points = m_grid->GetPoints();
	if (points == nullptr) {return;}
	m_scalarValues->Reset();
	double doubleval = variantValue().toDouble();
	for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
		m_scalarValues->InsertNextValue(doubleval);
	}
	m_scalarValues->Modified();
	m_gridRegionPolygon->updateScalarValues();
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* p = m_holePolygons.at(i);
		p->updateScalarValues();
	}
}

void RawDataPolygon::updateGrid(bool noDraw)
{
	if (m_triangleThread != nullptr && m_triangleThread->isOutputting()) {
		// it has already started outputting. Wait until it ends.
		while (m_triangleThread->isOutputting()) {
			m_triangleThread->wait(100);
		}
	}
	if (m_triangleThread == nullptr) {
		m_triangleThread = new RawDataPolygonTriangleThread(this);
		connect(m_triangleThread, SIGNAL(shapeUpdated()), this, SLOT(renderGraphics()));
	}
	m_triangleThread->setNoDraw(noDraw);
	if (! noDraw) {
		m_paintActor->SetVisibility(0);
	}
	if (m_triangleThread->isRunning()) {
		m_triangleThread->cancel();
	}
	m_triangleThread->update();
}

void RawDataPolygon::editColorSetting()
{
	dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent())->showPropertyDialog();
}

void RawDataPolygon::setMapping(RawDataPolygonColorSettingDialog::Mapping m)
{
	m_mapping = m;
	if (m_mapping == RawDataPolygonColorSettingDialog::Arbitrary) {
		m_paintMapper->SetScalarVisibility(false);
		setColor(m_color);
	} else {
		m_paintMapper->SetScalarVisibility(true);
	}
	m_gridRegionPolygon->setMapping(m);
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* p = m_holePolygons.at(i);
		p->setMapping(m);
	}
	updateGrid();
	renderGraphicsView();
}

QDialog* RawDataPolygon::propertyDialog(QWidget* parent)
{
	RawDataPolygonColorSettingDialog* dialog = new RawDataPolygonColorSettingDialog(parent);
	dialog->setMapping(m_mapping);
	dialog->setOpacityPercent(m_opacityPercent);
	if (m_mapping == RawDataPolygonColorSettingDialog::Value) {
		m_color = doubleToColor(variantValue().toDouble());
	}
	dialog->setColor(m_color);

	return dialog;
}

class RawDataPolygonPropertyEditCommand : public QUndoCommand
{
public:
	RawDataPolygonPropertyEditCommand(RawDataPolygon* p, RawDataPolygonColorSettingDialog::Mapping oldm, const QColor& oldc, int oldp, RawDataPolygonColorSettingDialog::Mapping newm, const QColor& newc, int newp)
		: QUndoCommand(QObject::tr("Polygon property edit")) {
		m_polygon = p;
		m_oldMapping = oldm;
		m_oldColor = oldc;
		m_oldOpacityPercent = oldp;
		m_newMapping = newm;
		m_newColor = newc;
		m_newOpacityPercent = newp;
	}
	void undo() {
		m_polygon->m_color = m_oldColor;
		m_polygon->m_opacityPercent = m_oldOpacityPercent;
		m_polygon->setMapping(m_oldMapping);
	}
	void redo() {
		m_polygon->m_color = m_newColor;
		m_polygon->m_opacityPercent = m_newOpacityPercent;
		m_polygon->setMapping(m_newMapping);
	}
private:
	RawDataPolygon* m_polygon;
	RawDataPolygonColorSettingDialog::Mapping m_oldMapping;
	QColor m_oldColor;
	int m_oldOpacityPercent;
	RawDataPolygonColorSettingDialog::Mapping m_newMapping;
	QColor m_newColor;
	int m_newOpacityPercent;
};

void RawDataPolygon::handlePropertyDialogAccepted(QDialog* propDialog)
{
	RawDataPolygonColorSettingDialog* dialog = dynamic_cast<RawDataPolygonColorSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new RawDataPolygonPropertyEditCommand(this, m_mapping, m_color, m_opacityPercent, dialog->mapping(), dialog->color(), dialog->opacityPercent()));
}

bool RawDataPolygon::getValueRange(double* min, double* max)
{
	*min = variantValue().toDouble();
	*max = variantValue().toDouble();
	if (m_selectedPolygon != m_gridRegionPolygon) {return true;}
	switch (m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
		return false;
		break;
	default:
		return true;
	}
}

void RawDataPolygon::renderGraphics()
{
	m_paintActor->SetVisibility(1);
	renderGraphicsView();
}

RawDataProxy* RawDataPolygon::getProxy()
{
	return new RawDataPolygonProxy(this);
}

void RawDataPolygon::copy()
{
	PreProcessorRawDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
	PreProcessorRawDataTopDataItemInterface* tItem = dynamic_cast<PreProcessorRawDataTopDataItemInterface*>(gItem->parent());

	QList<PreProcessorRawDataGroupDataItemInterface*> groups = tItem->groupDataItems();
	QList<PreProcessorRawDataGroupDataItemInterface*> targetGroups;
	QStringList items;
	for (int i = 0; i < groups.count(); ++i) {
		if (groups[i] == gItem) {continue;}
		items.append(groups[i]->standardItem()->text());
		targetGroups.append(groups[i]);
	}
	bool ok;
	QString item = QInputDialog::getItem(preProcessorWindow(), tr("Select Geographic Data"), tr("Please select which geographic data to copy this polygon."), items, 0, false, &ok);
	if (! ok) {
		// canceled.
		return;
	}
	PreProcessorRawDataGroupDataItemInterface* targetGroup = targetGroups[items.indexOf(item)];
	// create a polygon to targetGroup, and set the polygon shape.
	targetGroup->addCopyPolygon(this);
}

void RawDataPolygon::copyShape(RawDataPolygon* polygon)
{
	setPolygon(polygon->polygon());
	for (int i = 0; i < polygon->m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* p = polygon->m_holePolygons.at(i);
		RawDataPolygonHolePolygon* holePol = new RawDataPolygonHolePolygon(this);
		setupHolePolygon(holePol);
		holePol->setPolygon(p->polygon());
		holePol->setActive(false);
		holePol->setSelected(false);
		m_holePolygons.append(holePol);
	}
	updateGrid();
	m_mouseEventMode = meNormal;
	editValue();
	// copy command is not undo-able.
	iRICUndoStack::instance().clear();
}


void RawDataPolygon::doApplyOffset(double x, double y)
{
	applyOffsetToAbstractPolygon(m_gridRegionPolygon, x, y);
	for (auto it = m_holePolygons.begin(); it != m_holePolygons.end(); ++it) {
		applyOffsetToAbstractPolygon(*it, x, y);
	}
	updateGrid(true);
}

void RawDataPolygon::applyOffsetToAbstractPolygon(RawDataPolygonAbstractPolygon* polygon, double x, double y)
{
	QPolygonF pol = polygon->polygon(QPointF(x, y));
	polygon->setPolygon(pol);
}

void RawDataPolygon::handleDimensionCurrentIndexChange(int /*oldIndex*/, int /*newIndex*/)
{
	// @todo implement this!
}

void RawDataPolygon::handleDimensionValuesChange(const QList<QVariant>& /*before*/, const QList<QVariant>& /*after*/)
{
	// @todo implement this!
}

