#define REAL double
#define VOID void

#include "geodatapolygon.h"
#include "geodatapolygonabstractpolygon.h"
#include "geodatapolygonholepolygon.h"
#include "geodatapolygonproxy.h"
#include "geodatapolygonregionpolygon.h"
#include "geodatapolygontrianglethread.h"

#include "private/geodatapolygon_addholepolygoncommand.h"
#include "private/geodatapolygon_addvertexcommand.h"
#include "private/geodatapolygon_coordinateseditor.h"
#include "private/geodatapolygon_editpropertycommand.h"
#include "private/geodatapolygon_editvaluecommand.h"
#include "private/geodatapolygon_finishpolygondefinitioncommand.h"
#include "private/geodatapolygon_movepolygoncommand.h"
#include "private/geodatapolygon_movevertexcommand.h"
#include "private/geodatapolygon_pushnewpointcommand.h"
#include "private/geodatapolygon_removevertexcommand.h"

#include <iriclib_polygon.h>

#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/errormessage.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include <misc/zdepthrange.h>
#include <triangle/triangle.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>

#include <QAction>
#include <QCoreApplication>
#include <QFile>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPolygonF>
#include <QStandardItem>
#include <QTextStream>
#include <QToolBar>
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

GeoDataPolygon::GeoDataPolygon(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData(d, creator, condition)
{
	initParams();

	m_shapeUpdating = false;
	m_triangleThread = nullptr;

	m_gridRegionPolygon = new GeoDataPolygonRegionPolygon(this);
	m_gridRegionPolygon->setSelected(true);
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		m_gridRegionPolygon->setLookupTable(stcc->vtkDarkObj());
	}

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

	m_polyData = vtkSmartPointer<vtkPolyData>::New();

	m_scalarValues = vtkSmartPointer<vtkDoubleArray>::New();
	m_scalarValues->SetName("polygonvalue");
	m_polyData->GetPointData()->AddArray(m_scalarValues);
	m_polyData->GetPointData()->SetActiveScalars("polygonvalue");

	m_paintMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	if (stcc != nullptr) {
		m_paintMapper->SetLookupTable(stcc->vtkObj());
	}
	m_paintMapper->SetUseLookupTableScalarRange(true);
	m_paintMapper->SetInputData(m_polyData);

	m_paintActor = vtkSmartPointer<vtkActor>::New();
	m_paintActor->GetProperty()->SetLighting(false);
	m_paintActor->SetMapper(m_paintMapper);

	actorCollection()->AddItem(m_paintActor);
	renderer()->AddActor(m_paintActor);

	updateActorSettings();
	updateActionStatus();
}

GeoDataPolygon::~GeoDataPolygon()
{
	if (m_triangleThread != nullptr) {
		m_triangleThread->cancelJobs(this);
	}
	delete m_gridRegionPolygon;
	clearHolePolygons();

	delete m_rightClickingMenu;

	vtkRenderer* r = renderer();
	r->RemoveActor(m_paintActor);
}

void GeoDataPolygon::setupMenu()
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

bool GeoDataPolygon::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(m_holeModeAction);

	tb->addSeparator();
	tb->addAction(m_addVertexAction);
	tb->addAction(m_removeVertexAction);

	tb->addSeparator();
	tb->addAction(m_deleteAction);
	return true;
}

QColor GeoDataPolygon::doubleToColor(double /*d*/)
{
	return Qt::red;
}

void GeoDataPolygon::setMapping(GeoDataPolygonColorSettingDialog::Mapping m)
{
	m_setting.mapping = m;
	updateActorSettings();
}

void GeoDataPolygon::setOpacity(int opacity)
{
	m_setting.opacity = opacity;
	updateActorSettings();
}

void GeoDataPolygon::setColor(const QColor& color)
{
	m_setting.color = color;
	updateActorSettings();
}

void GeoDataPolygon::informSelection(PreProcessorGraphicsViewInterface* v)
{
	m_gridRegionPolygon->setActive(true);
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		GeoDataPolygonHolePolygon* p = m_holePolygons.at(i);
		p->setActive(true);
	}

	m_selectMode = smPolygon;
	m_gridRegionPolygon->setSelected(true);
	m_selectedPolygon = m_gridRegionPolygon;

	updateMouseCursor(v);
}

void GeoDataPolygon::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	m_gridRegionPolygon->setActive(false);
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		GeoDataPolygonHolePolygon* p = m_holePolygons.at(i);
		p->setActive(false);
	}
	if (m_selectedPolygon != nullptr) {
		m_selectedPolygon->setSelected(false);
	}
	v->unsetCursor();
}

void GeoDataPolygon::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GeoDataPolygon::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (! iRIC::isEnterKey(event->key())) {return;}
	if (m_mouseEventMode != meDefining) {return;}
	if (m_selectMode != smPolygon) {return;}

	definePolygon(false);
}

void GeoDataPolygon::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GeoDataPolygon::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (m_mouseEventMode == meDefining) {
		if (m_selectMode == smPolygon) {
			definePolygon(true);
		}
	}
}


void GeoDataPolygon::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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
			pushRenderCommand(new PushNewPointCommand(false, QPoint(event->x(), event->y()), this));
		}
		break;
	case meTranslate:
		// execute translation.
		if (m_selectMode == smPolygon) {
			pushRenderCommand(new MovePolygonCommand(false, m_currentPoint, QPoint(event->x(), event->y()), this));
		}
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meMoveVertex:
		if (m_selectMode == smPolygon) {
			pushRenderCommand(new MoveVertexCommand(false, m_currentPoint, QPoint(event->x(), event->y()), m_selectedPolygon->selectedVertexId(), this));
		}
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meAddVertex:
		if (m_selectMode == smPolygon) {
			pushRenderCommand(new AddVertexCommand(false, m_selectedPolygon->selectedEdgeId(), QPoint(event->x(), event->y()), this));
		}
		break;
	case meTranslateDialog:
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GeoDataPolygon::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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
				pushRenderCommand(new PushNewPointCommand(true, QPoint(event->x(), event->y()), this));
			}
			break;
		case meDefining:
			if (m_selectMode == smPolygon) {
				pushRenderCommand(new PushNewPointCommand(true, QPoint(event->x(), event->y()), this));
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
					pushRenderCommand(new MovePolygonCommand(true, m_currentPoint, m_currentPoint, this));
				}
			}
			break;
		case meMoveVertexPrepare:
			m_mouseEventMode = meMoveVertex;
			m_currentPoint = QPoint(event->x(), event->y());
			// push the first move command.
			if (m_selectMode == smPolygon) {
				pushRenderCommand(new MoveVertexCommand(true, m_currentPoint, m_currentPoint, m_selectedPolygon->selectedVertexId(), this));
			}
			break;
		case meAddVertexPrepare:
			m_mouseEventMode = meAddVertex;
			if (m_selectMode == smPolygon) {
				pushRenderCommand(new AddVertexCommand(true, m_selectedPolygon->selectedEdgeId(), QPoint(event->x(), event->y()), this));
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
					m_mouseEventMode = meNormal;
					pushRenderCommand(new RemoveVertexCommand(m_selectedPolygon->selectedVertexId(), this));
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

void GeoDataPolygon::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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

void GeoDataPolygon::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
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

void GeoDataPolygon::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
	menu->addAction(m_editValueAction);
	menu->addSeparator();
	menu->addAction(m_copyAction);
}

void GeoDataPolygon::definePolygon(bool doubleClick, bool noEditVal)
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
	if (m_selectedPolygon == m_gridRegionPolygon && (! noEditVal)) {
		editValue();
	}
}

QColor GeoDataPolygon::color() const
{
	return m_setting.color;
}

void GeoDataPolygon::addVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meAddVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPolygon::removeVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meRemoveVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPolygon::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	m_setting.load(node);
}

void GeoDataPolygon::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	m_setting.save(writer);
}

void GeoDataPolygon::loadExternalData(const QString& filename)
{
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		m_gridRegionPolygon->setLookupTable(stcc->vtkDarkObj());
	}
	if (projectData()->version().build() >= 3607) {
		iRICLib::Polygon* pol = new iRICLib::Polygon();
		GridAttributeDimensionsContainer* dims = dimensions();
		bool noDim = true;
		if (dims != nullptr) {
			noDim = dims->containers().size() == 0;
		}

		pol->load(iRIC::toStr(filename).c_str(), noDim);
		m_variantValues.clear();
		for (unsigned int i = 0; i < pol->values.size(); ++i) {
			m_variantValues.push_back(pol->values[i]);
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
			auto holePol = setupHolePolygon();
			holePol->setPolygon(qpol);
			holePol->setActive(false);
			holePol->setSelected(false);
			m_holePolygons.append(holePol);
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
				auto holePol = setupHolePolygon();
				holePol->setPolygon(pol);
				holePol->setActive(false);
				holePol->setSelected(false);
				m_holePolygons.append(holePol);
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
	updatePolyData(true);
	updateActorSettings();
	updateActionStatus();
}

void GeoDataPolygon::saveExternalData(const QString& filename)
{
	iRICLib::Polygon pol;
	pol.values.clear();
	for (int i = 0; i < m_variantValues.size(); ++i) {
		pol.values.push_back(m_variantValues.at(i).toDouble());
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
	pol.polygon = regionPolygon;
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		iRICLib::InternalPolygon* holePolygon = new iRICLib::InternalPolygon();
		QPolygonF hqpol = m_holePolygons[i]->polygon();
		holePolygon->pointCount = hqpol.count();
		holePolygon->x = new double[holePolygon->pointCount];
		holePolygon->y = new double[holePolygon->pointCount];
		for (int j = 0; j < holePolygon->pointCount; ++j) {
			*(holePolygon->x + j) = hqpol.at(j).x();
			*(holePolygon->y + j) = hqpol.at(j).y();
		}
		pol.holes.push_back(holePolygon);
	}
	GridAttributeDimensionsContainer* dims = dimensions();
	bool noDim = true;
	if (dims != nullptr) {
		noDim = dims->containers().size() == 0;
	}
	pol.save(iRIC::toStr(filename).c_str(), noDim);
}

void GeoDataPolygon::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GeoDataPolygon::assignActorZValues(const ZDepthRange& range)
{
	m_depthRange = range;
	m_gridRegionPolygon->setZDepthRange(range.min(), range.max());
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		GeoDataPolygonHolePolygon* p = m_holePolygons[i];
		p->setZDepthRange(range.min(), range.max());
	}
	m_paintActor->SetPosition(0, 0, range.min());
}

void GeoDataPolygon::updateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QVector2D worldPos(dx, dy);
	bool shapeUpdating = m_shapeUpdating;
	shapeUpdating = shapeUpdating || (m_triangleThread != nullptr && m_triangleThread->isOutputting(this));
	switch (m_mouseEventMode) {
	case meAddVertexNotPossible:
	case meAddVertexPrepare:
		if (m_selectMode == smNone) {return;}
		if (m_selectMode == smPolygon) {
			if (shapeUpdating) {
				m_mouseEventMode = meAddVertexNotPossible;
			} else if (m_selectedPolygon->isEdgeSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
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
			} else if (m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
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
			} else if (m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
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

void GeoDataPolygon::updateActionStatus()
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
		if (dynamic_cast<GeoDataPolygonRegionPolygon*>(m_selectedPolygon) != nullptr) {
//			m_defineModeAction->setChecked(true);
		} else if (dynamic_cast<GeoDataPolygonHolePolygon*>(m_selectedPolygon) != nullptr) {
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

void GeoDataPolygon::editCoordinates()
{
	CoordinatesEditor::edit(this);
}

void GeoDataPolygon::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

void GeoDataPolygon::clear()
{
	initParams();
	clearHolePolygons();
	delete m_gridRegionPolygon;

	m_gridRegionPolygon = new GeoDataPolygonRegionPolygon(this);
	m_gridRegionPolygon->setSelected(true);
	m_gridRegionPolygon->setZDepthRange(m_depthRange.min(), m_depthRange.max());
	m_mouseEventMode = meBeforeDefining;
	m_selectedPolygon = m_gridRegionPolygon;
	updateMouseCursor(graphicsView());
	updateActionStatus();
	renderGraphicsView();
}

bool GeoDataPolygon::ready() const
{
	return true;
}

void GeoDataPolygon::initParams()
{
	m_variantValues.clear();

	int maxIndex = 1;
	GridAttributeDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		maxIndex = dimensions()->maxIndex();
	}
	bool ok;
	double defaultValue = 0;

	auto gridAtt = gridAttribute();
	if (gridAtt != nullptr) {
		defaultValue = gridAtt->variantDefaultValue().toDouble(&ok);
		if (! ok) {defaultValue = 0;}
	}

	for (int i = 0; i <= maxIndex; ++i) {
		m_variantValues.push_back(defaultValue);
	}
}

void GeoDataPolygon::addHolePolygon()
{
	auto holePol = setupHolePolygon();
	if (m_selectedPolygon != nullptr) {
		m_selectedPolygon->setSelected(false);
	}
	pushRenderCommand(new AddHolePolygonCommand(holePol, this));
	InformationDialog::information(preProcessorWindow(), GeoDataPolygon::tr("Information"), GeoDataPolygon::tr("Please define hole region. Hole region can be defined as polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gctriangle_addholepolygon");
}

vtkPolyData* GeoDataPolygon::polyData() const
{
	return m_polyData;
}

GeoDataPolygonHolePolygon* GeoDataPolygon::setupHolePolygon()
{
	GeoDataPolygonHolePolygon* pol = new GeoDataPolygonHolePolygon(this);

	pol->setZDepthRange(m_depthRange.min(), m_depthRange.max());
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		pol->setLookupTable(scalarsToColorsContainer()->vtkDarkObj());
	}
	pol->setActive(true);
	pol->setMapping(m_setting.mapping);
	pol->setColor(m_setting.color);

	return pol;
}

void GeoDataPolygon::deletePolygon(bool force)
{
	if (m_selectedPolygon == nullptr) {return;}
	if (! force) {
		int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to remove this polygon?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	if (dynamic_cast<GeoDataPolygonRegionPolygon*>(m_selectedPolygon) != nullptr) {
		delete m_gridRegionPolygon;
		m_gridRegionPolygon = new GeoDataPolygonRegionPolygon(this);
		m_gridRegionPolygon->setActive(true);
		m_gridRegionPolygon->setSelected(true);
		m_gridRegionPolygon->setZDepthRange(m_depthRange.min(), m_depthRange.max());
		m_selectedPolygon = m_gridRegionPolygon;
		m_mouseEventMode = meBeforeDefining;
	} else if (dynamic_cast<GeoDataPolygonHolePolygon*>(m_selectedPolygon) != nullptr) {
		GeoDataPolygonHolePolygon* tmpPoly = dynamic_cast<GeoDataPolygonHolePolygon*>(m_selectedPolygon);
		m_holePolygons.removeOne(tmpPoly);
		delete m_selectedPolygon;
		m_selectedPolygon = nullptr;
		m_selectMode = smNone;
		m_mouseEventMode = meNormal;
	}
	// This operation is not undoable.
	iRICUndoStack::instance().clear();
	setMapped(false);

	updateMouseCursor(graphicsView());
	updatePolyData();
	updateActionStatus();
	renderGraphicsView();
}

bool GeoDataPolygon::selectObject(QPoint point)
{
	SelectMode oldSelectMode = m_selectMode;
	GeoDataPolygonAbstractPolygon* oldSelectedPolygon = m_selectedPolygon;
	deselectAll();

	double dx = point.x();
	double dy = point.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF p(dx, dy);
	QVector2D pv(dx, dy);

	double selectlimit = graphicsView()->stdRadius(iRIC::nearRadius());

	// find polygon that contains this point.
	GeoDataPolygonAbstractPolygon* newSelPol = nullptr;
	bool selected = false;
	for (int i = m_holePolygons.count() - 1; i >= 0 && (! selected); --i) {
		GeoDataPolygonAbstractPolygon* pol = m_holePolygons[i];
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

void GeoDataPolygon::deselectAll()
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

bool GeoDataPolygon::checkCondition()
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
		GeoDataPolygonHolePolygon* hpol = m_holePolygons[i];
		if (hpol->polygon().count() < 4) {
//			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon have to consists of more than three vertices"));
			return false;
		}
		if (iRIC::hasIntersection(hpol->polygon())) {
//			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon shape is invalid."));
			return false;
		}
//		if (gridPol.intersected(hpol->polygon()) != hpol->polygon()) {
//			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Hole polygon have to be inside grid region."));
//			return false;
//		}
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

bool GeoDataPolygon::activePolygonHasFourVertices()
{
	if (m_selectedPolygon == nullptr) {return false;}
	QPolygonF pol = m_selectedPolygon->polygon();
	return pol.count() > 4;
}

void GeoDataPolygon::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GeoDataPolygon::tr("Information"), GeoDataPolygon::tr("Please define polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "geodatapolygoninit");
}

const QVariant& GeoDataPolygon::variantValue() const
{
	int index = 0;
	GridAttributeDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	return m_variantValues.at(index);
}

void GeoDataPolygon::setVariantValue(const QVariant &v, bool disableInform)
{
	int index = 0;
	GridAttributeDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	m_variantValues[index] = v;
	updateScalarValues();
	if (! disableInform) {
		auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
		p->informValueRangeChange();
		p->informDataChange();
	}
}

void GeoDataPolygon::editValue()
{
	GridAttributeEditDialog* dialog = m_gridAttribute->editDialog(preProcessorWindow());
	PreProcessorGeoDataGroupDataItemInterface* i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setLabel(tr("Please input new value in this polygon.").arg(i->condition()->caption()));
	i->setupEditWidget(dialog->widget());
	dialog->setVariantValue(variantValue());
	if (m_mouseEventMode == meDefining || m_mouseEventMode == meBeforeDefining) {
		dialog->disableCancel();
	}
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {return;}
	pushCommand(new EditValueCommand(dialog->variantValue(), this));
}

void GeoDataPolygon::setPolygon(const QPolygonF& p)
{
	m_gridRegionPolygon->setPolygon(p);
}

void GeoDataPolygon::addHolePolygon(const QPolygonF& p)
{
	auto newPol = setupHolePolygon();
	newPol->setSelected(false);
	newPol->setPolygon(p);
	m_holePolygons.append(newPol);
}

const QPolygonF GeoDataPolygon::polygon() const
{
	return m_gridRegionPolygon->polygon();
}

void GeoDataPolygon::updateScalarValues()
{
	vtkPoints* points = m_polyData->GetPoints();
	if (points == nullptr) {return;}
	m_scalarValues->Reset();
	double doubleval = variantValue().toDouble();
	for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
		m_scalarValues->InsertNextValue(doubleval);
	}
	m_scalarValues->Modified();
	m_gridRegionPolygon->updateScalarValues();
	for (auto hole : m_holePolygons) {
		hole->updateScalarValues();
	}
}

void GeoDataPolygon::updatePolyData(bool noDraw)
{
	if (m_triangleThread != nullptr && m_triangleThread->isOutputting(this)){
		// it has already started outputting. Wait until it ends.
		while (m_triangleThread->isOutputting(this)){
			m_triangleThread->wait(100);
		}
	}
	setupTriangleThread();

	m_triangleThread->addJob(this, noDraw);
	if (! noDraw){
		m_paintActor->SetVisibility(0);
	}
}

void GeoDataPolygon::editColorSetting()
{
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->showPropertyDialog();
}

void GeoDataPolygon::updateActorSettings()
{
	// color
	m_gridRegionPolygon->setColor(m_setting.color);
	for (auto hole : m_holePolygons) {
		hole->setColor(m_setting.color);
	}
	m_paintActor->GetProperty()->SetColor(m_setting.color);

	// opacity
	m_paintActor->GetProperty()->SetOpacity(m_setting.opacity);

	// mapping
	if (m_setting.mapping == GeoDataPolygonColorSettingDialog::Arbitrary) {
		m_paintMapper->SetScalarVisibility(false);
	} else {
		m_paintMapper->SetScalarVisibility(true);
	}
	m_gridRegionPolygon->setMapping(m_setting.mapping);
	for (auto hole : m_holePolygons) {
		hole->setMapping(m_setting.mapping);
	}
}

QDialog* GeoDataPolygon::propertyDialog(QWidget* parent)
{
	GeoDataPolygonColorSettingDialog* dialog = new GeoDataPolygonColorSettingDialog(parent);
	dialog->setSetting(m_setting);

	return dialog;
}

void GeoDataPolygon::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<GeoDataPolygonColorSettingDialog*>(propDialog);
	pushRenderCommand(new EditPropertyCommand(dialog->setting(), this));
}

bool GeoDataPolygon::getValueRange(double* min, double* max)
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

void GeoDataPolygon::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataPolygon::renderGraphics()
{
	int visibility = isVisible() ? 1 : 0;
	m_paintActor->SetVisibility(visibility);
	renderGraphicsView();
}

void GeoDataPolygon::updatePolygon(GeoDataPolygon* polygon, vtkPoints* points, vtkCellArray* ca, bool noDraw)
{
	if (polygon != this) {return;}

	m_triangleThread->lockMutex();

	m_polyData->SetPoints(points);
	points->Delete();

	m_polyData->SetPolys(ca);
	ca->Delete();

	updateScalarValues();

	m_polyData->BuildCells();
	m_polyData->BuildLinks();
	m_polyData->Modified();

	m_triangleThread->unlockMutex();

	m_paintActor->GetProperty()->SetOpacity(m_setting.opacity);

	if (noDraw) {return;}

	renderGraphics();
}

GeoDataProxy* GeoDataPolygon::getProxy()
{
	return new GeoDataPolygonProxy(this);
}

void GeoDataPolygon::copy()
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	PreProcessorGeoDataTopDataItemInterface* tItem = dynamic_cast<PreProcessorGeoDataTopDataItemInterface*>(gItem->parent());

	QList<PreProcessorGeoDataGroupDataItemInterface*> groups = tItem->groupDataItems();
	QList<PreProcessorGeoDataGroupDataItemInterface*> targetGroups;
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
	PreProcessorGeoDataGroupDataItemInterface* targetGroup = targetGroups[items.indexOf(item)];
	// create a polygon to targetGroup, and set the polygon shape.
	targetGroup->addCopyPolygon(this);
}

void GeoDataPolygon::copyShape(GeoDataPolygon* polygon)
{
	setPolygon(polygon->polygon());
	for (int i = 0; i < polygon->m_holePolygons.count(); ++i) {
		GeoDataPolygonHolePolygon* p = polygon->m_holePolygons.at(i);
		auto holePol = setupHolePolygon();
		holePol->setPolygon(p->polygon());
		holePol->setActive(false);
		holePol->setSelected(false);
		m_holePolygons.append(holePol);
	}
	updatePolyData();
	m_mouseEventMode = meNormal;
	editValue();
	// copy command is not undo-able.
	iRICUndoStack::instance().clear();
}

void GeoDataPolygon::doApplyOffset(double x, double y)
{
	applyOffsetToAbstractPolygon(m_gridRegionPolygon, x, y);
	for (auto it = m_holePolygons.begin(); it != m_holePolygons.end(); ++it) {
		applyOffsetToAbstractPolygon(*it, x, y);
	}
	updatePolyData(true);
}

void GeoDataPolygon::applyOffsetToAbstractPolygon(GeoDataPolygonAbstractPolygon* polygon, double x, double y)
{
	QPolygonF pol = polygon->polygon(QPointF(x, y));
	polygon->setPolygon(pol);
}

void GeoDataPolygon::handleDimensionCurrentIndexChange(int /*oldIndex*/, int /*newIndex*/)
{
	// @todo implement this!
}

void GeoDataPolygon::handleDimensionValuesChange(const std::vector<QVariant>& /*before*/, const std::vector<QVariant>& /*after*/)
{
	// @todo implement this!
}

void GeoDataPolygon::clearHolePolygons()
{
	for (auto p : m_holePolygons) {
		delete p;
	}
	m_holePolygons.clear();
}

void GeoDataPolygon::lockMutex()
{
	if (m_triangleThread == nullptr) {return;}

	m_triangleThread->lockMutex();
}

void GeoDataPolygon::unlockMutex()
{
	if (m_triangleThread == nullptr) {return;}

	m_triangleThread->unlockMutex();
}

GeoDataPolygon::MouseEventMode GeoDataPolygon::mouseEventMode() const
{
	return m_mouseEventMode;
}

GeoDataPolygon::SelectMode GeoDataPolygon::selectMode() const
{
	return m_selectMode;
}

void GeoDataPolygon::setBCSettingMode(bool mode)
{
	m_bcSettingMode = mode;
}

geos::geom::Polygon* GeoDataPolygon::getGeosPolygon(const QPointF& offset)
{
	setupTriangleThread();
	return m_triangleThread->getGeosPolygon(this, offset);
}

void GeoDataPolygon::setupTriangleThread()
{
	if (m_triangleThread != nullptr) {return;}

	m_triangleThread = GeoDataPolygonTriangleThread::instance();
	connect(m_triangleThread, SIGNAL(shapeUpdated(GeoDataPolygon*,vtkPoints*,vtkCellArray*,bool)), this, SLOT(updatePolygon(GeoDataPolygon*,vtkPoints*,vtkCellArray*,bool)));
}
