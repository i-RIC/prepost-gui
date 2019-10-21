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
#include "private/geodatapolygon_impl.h"
#include "private/geodatapolygon_movepolygoncommand.h"
#include "private/geodatapolygon_movevertexcommand.h"
#include "private/geodatapolygon_pushnewpointcommand.h"
#include "private/geodatapolygon_removevertexcommand.h"

#include <iriclib_polygon.h>

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include <misc/zdepthrange.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>

#include <QAction>
#include <QFile>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPolygonF>
#include <QStandardItem>
#include <QToolBar>

#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

namespace {

QPolygonF buildPolygon(const geos::geom::LineString* ls)
{
	QPolygonF pol;
	for (int i = 0; i < ls->getNumPoints(); ++i) {
		const auto& coord = ls->getCoordinateN(i);
		pol.push_back(QPointF(coord.x, coord.y));
	}
	return pol;
}

} // namespace

GeoDataPolygon::Impl::Impl(GeoDataPolygon* parent) :
	m_parent {parent},
	m_regionPolygon {new GeoDataPolygonRegionPolygon(m_parent)},
	m_selectedPolygon {m_regionPolygon},
	m_triangleThread {nullptr},
	m_inhibitSelect {false},
	m_shapeUpdating {false},
	m_bcSettingMode {false},
	m_selectMode {smPolygon},
	m_polyData {vtkPolyData::New()},
	m_scalarValues {vtkDoubleArray::New()},
	m_rightClickingMenu {new QMenu()},
	m_holeModeAction {new QAction(QIcon(":/libs/guibase/images/iconPolygonHole.png"), GeoDataPolygon::tr("Add &Hole Region"), m_parent)},
	m_deleteAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"), GeoDataPolygon::tr("&Delete Hole Region..."), m_parent)},
	m_editValueAction {new QAction(GeoDataPolygon::tr("Edit &Value..."), m_parent)},
	m_copyAction {new QAction(GeoDataPolygon::tr("&Copy..."), m_parent)},
	m_addVertexAction {new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), GeoDataPolygon::tr("&Add Vertex"), m_parent)},
	m_removeVertexAction {new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), GeoDataPolygon::tr("&Remove Vertex"), m_parent)},
	m_coordEditAction {new QAction(GeoDataPolygon::tr("Edit &Coordinates..."), m_parent)},
	m_editColorSettingAction {new QAction(GeoDataPolygon::tr("Color &Setting..."), m_parent)},
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_removePixmap {":/libs/guibase/images/cursorRemove.png"},
	m_movePointPixmap {":/libs/guibase/images/cursorOpenHandPoint.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_removeCursor {m_removePixmap, 0, 0},
	m_movePointCursor {m_movePointPixmap}
{
	m_regionPolygon->setSelected(true);

	m_holeModeAction->setCheckable(true);
	m_holeModeAction->setDisabled(true);

	m_addVertexAction->setCheckable(true);
	m_removeVertexAction->setCheckable(true);

	QObject::connect(m_holeModeAction, SIGNAL(triggered()), m_parent, SLOT(addHolePolygon()));
	QObject::connect(m_deleteAction, SIGNAL(triggered()), m_parent, SLOT(deletePolygon()));
	QObject::connect(m_editValueAction, SIGNAL(triggered()), m_parent, SLOT(editValue()));
	QObject::connect(m_copyAction, SIGNAL(triggered()), m_parent, SLOT(copy()));
	QObject::connect(m_addVertexAction, SIGNAL(triggered(bool)), m_parent, SLOT(addVertexMode(bool)));
	QObject::connect(m_removeVertexAction, SIGNAL(triggered(bool)), m_parent, SLOT(removeVertexMode(bool)));
	QObject::connect(m_coordEditAction, SIGNAL(triggered()), m_parent, SLOT(editCoordinates()));
	QObject::connect(m_editColorSettingAction, SIGNAL(triggered()), m_parent, SLOT(editColorSetting()));
}

GeoDataPolygon::Impl::~Impl()
{
	m_scalarValues->Delete();
	m_polyData->Delete();
	delete m_rightClickingMenu;
	delete m_regionPolygon;
}

GeoDataPolygon::GeoDataPolygon(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData(d, creator, condition),
	impl {new Impl {this}}
{
	initParams();

	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		impl->m_regionPolygon->setLookupTable(stcc->vtkDarkObj());
	}

	impl->m_mouseEventMode = meBeforeDefining;
	if (gridAttribute() && gridAttribute()->isReferenceInformation()) {
		impl->m_setting.mapping = GeoDataPolygonColorSettingDialog::Arbitrary;
	}

	impl->m_scalarValues->SetName("polygonvalue");
	impl->m_polyData->GetPointData()->AddArray(impl->m_scalarValues);
	impl->m_polyData->GetPointData()->SetActiveScalars("polygonvalue");

	auto mapper = impl->m_actor.mapper();
	if (stcc != nullptr) {
		mapper->SetLookupTable(stcc->vtkObj());
	}
	mapper->SetUseLookupTableScalarRange(true);
	mapper->SetInputData(impl->m_polyData);

	actorCollection()->AddItem(impl->m_actor.actor());
	renderer()->AddActor(impl->m_actor.actor());

	updateActorSettings();
	updateActionStatus();
}

GeoDataPolygon::~GeoDataPolygon()
{
	if (impl->m_triangleThread != nullptr) {
		impl->m_triangleThread->cancelJobs(this);
	}
	clearHolePolygons();

	vtkRenderer* r = renderer();
	r->RemoveActor(impl->m_actor.actor());

	delete impl;
}

void GeoDataPolygon::setupMenu()
{
	bool isRef = false;
	if (gridAttribute()) {
		isRef = gridAttribute()->isReferenceInformation();
	}

	m_menu->setTitle(tr("&Polygon"));
	m_menu->addAction(m_editNameAction);
	if (! isRef) {
		m_menu->addAction(impl->m_editValueAction);
	}
	m_menu->addSeparator();
	m_menu->addAction(impl->m_copyAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_addVertexAction);
	m_menu->addAction(impl->m_removeVertexAction);
	m_menu->addAction(impl->m_coordEditAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_holeModeAction);
	m_menu->addAction(impl->m_deleteAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_editColorSettingAction);
	m_menu->addSeparator();
	m_menu->addAction(deleteAction());

	if (! isRef) {
		impl->m_rightClickingMenu->addAction(impl->m_editValueAction);
		impl->m_rightClickingMenu->addSeparator();
	}
	impl->m_rightClickingMenu->addAction(impl->m_copyAction);
	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_addVertexAction);
	impl->m_rightClickingMenu->addAction(impl->m_removeVertexAction);
	impl->m_rightClickingMenu->addAction(impl->m_coordEditAction);
	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_holeModeAction);
	impl->m_rightClickingMenu->addAction(impl->m_deleteAction);
	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_editColorSettingAction);
}

bool GeoDataPolygon::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(impl->m_holeModeAction);

	tb->addSeparator();
	tb->addAction(impl->m_addVertexAction);
	tb->addAction(impl->m_removeVertexAction);

	tb->addSeparator();
	tb->addAction(impl->m_deleteAction);
	return true;
}

void GeoDataPolygon::setMapping(GeoDataPolygonColorSettingDialog::Mapping m)
{
	impl->m_setting.mapping = m;
	updateActorSettings();
}

void GeoDataPolygon::setOpacity(int opacity)
{
	impl->m_setting.opacity = opacity;
	updateActorSettings();
}

void GeoDataPolygon::setColor(const QColor& color)
{
	impl->m_setting.color = color;
	updateActorSettings();
}

void GeoDataPolygon::setMouseEventMode(MouseEventMode mode)
{
	impl->m_mouseEventMode = mode;
}

void GeoDataPolygon::setSelectMode(SelectMode mode)
{
	impl->m_selectMode = mode;
}

GeoDataPolygonColorSettingDialog::Setting GeoDataPolygon::colorSetting() const
{
	return impl->m_setting;
}

void GeoDataPolygon::setColorSetting(GeoDataPolygonColorSettingDialog::Setting setting)
{
	impl->m_setting = setting;
}

void GeoDataPolygon::informSelection(PreProcessorGraphicsViewInterface* v)
{
	impl->m_regionPolygon->setActive(true);
	for (int i = 0; i < impl->m_holePolygons.size(); ++i) {
		GeoDataPolygonHolePolygon* p = impl->m_holePolygons.at(i);
		p->setActive(true);
	}

	impl->m_selectMode = smPolygon;
	impl->m_regionPolygon->setSelected(true);
	impl->m_selectedPolygon = impl->m_regionPolygon;

	updateMouseCursor(v);
}

void GeoDataPolygon::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	impl->m_regionPolygon->setActive(false);
	for (int i = 0; i < impl->m_holePolygons.size(); ++i) {
		GeoDataPolygonHolePolygon* p = impl->m_holePolygons.at(i);
		p->setActive(false);
	}
	if (impl->m_selectedPolygon != nullptr) {
		impl->m_selectedPolygon->setSelected(false);
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
	if (impl->m_mouseEventMode != meDefining) {return;}
	if (impl->m_selectMode != smPolygon) {return;}

	definePolygon(false);
}

void GeoDataPolygon::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GeoDataPolygon::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (impl->m_mouseEventMode != meDefining) {return;}
	if (impl->m_selectMode != smPolygon) {return;}

	definePolygon(true);
}

void GeoDataPolygon::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_mouseEventMode) {
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		impl->m_currentPoint = event->pos();
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case meBeforeDefining:
		// do nothing.
		break;
	case meDefining:
		// update the position of the last point.
		if (impl->m_selectMode == smPolygon) {
			pushRenderCommand(new PushNewPointCommand(false, event->pos(), this));
		}
		break;
	case meTranslate:
		// execute translation.
		if (impl->m_selectMode == smPolygon) {
			pushRenderCommand(new MovePolygonCommand(false, impl->m_currentPoint, event->pos(), this));
		}
		impl->m_currentPoint = event->pos();
		break;
	case meMoveVertex:
		if (impl->m_selectMode == smPolygon) {
			pushRenderCommand(new MoveVertexCommand(false, impl->m_currentPoint, event->pos(), impl->m_selectedPolygon->selectedVertexId(), this));
		}
		impl->m_currentPoint = event->pos();
		break;
	case meAddVertex:
		if (impl->m_selectMode == smPolygon) {
			pushRenderCommand(new AddVertexCommand(false, impl->m_selectedPolygon->selectedEdgeId(), event->pos(), this));
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

		switch (impl->m_mouseEventMode) {
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
			impl->m_mouseEventMode = meDefining;
			if (impl->m_selectMode == smPolygon) {
				pushRenderCommand(new PushNewPointCommand(true, event->pos(), this));
			}
			break;
		case meDefining:
			if (impl->m_selectMode == smPolygon) {
				pushRenderCommand(new PushNewPointCommand(true, event->pos(), this));
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
				impl->m_mouseEventMode = meTranslate;
				impl->m_currentPoint = event->pos();
				updateMouseCursor(v);
				// push the first translation command.
				if (impl->m_selectMode == smPolygon) {
					pushRenderCommand(new MovePolygonCommand(true, impl->m_currentPoint, impl->m_currentPoint, this));
				}
			}
			break;
		case meMoveVertexPrepare:
			impl->m_mouseEventMode = meMoveVertex;
			impl->m_currentPoint = event->pos();
			// push the first move command.
			if (impl->m_selectMode == smPolygon) {
				pushRenderCommand(new MoveVertexCommand(true, impl->m_currentPoint, impl->m_currentPoint, impl->m_selectedPolygon->selectedVertexId(), this));
			}
			break;
		case meAddVertexPrepare:
			impl->m_mouseEventMode = meAddVertex;
			if (impl->m_selectMode == smPolygon) {
				pushRenderCommand(new AddVertexCommand(true, impl->m_selectedPolygon->selectedEdgeId(), event->pos(), this));
			}
			break;
		case meAddVertexNotPossible:
			// do nothing.
			break;
		case meRemoveVertexPrepare:
			if (impl->m_selectMode == smPolygon) {
				if (impl->m_selectedPolygon->polygon().count() == 1) {
					// you are going to remove the last point.
					deletePolygon(true);
				} else {
					impl->m_mouseEventMode = meNormal;
					pushRenderCommand(new RemoveVertexCommand(impl->m_selectedPolygon->selectedVertexId(), this));
				}
			}
			impl->m_inhibitSelect = true;
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
		impl->m_dragStartPoint = event->pos();
	}
}

void GeoDataPolygon::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_mouseEventMode) {
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
			impl->m_currentPoint = event->pos();
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
		impl->m_inhibitSelect = false;
	} else if (event->button() == Qt::RightButton) {
		if (impl->m_mouseEventMode == meEditVerticesDialog) {return;}
		if (iRIC::isNear(impl->m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GeoDataPolygon::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_mouseEventMode) {
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
		v->setCursor(impl->m_movePointCursor);
		break;
	case meAddVertexPrepare:
		v->setCursor(impl->m_addCursor);
		break;
	case meRemoveVertexPrepare:
		v->setCursor(impl->m_removeCursor);
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
	if (gridAttribute() && ! gridAttribute()->isReferenceInformation()) {
		menu->addAction(impl->m_editValueAction);
	}

	menu->addSeparator();
	menu->addAction(impl->m_copyAction);
}

void GeoDataPolygon::definePolygon(bool doubleClick, bool noEditVal)
{
	int minCount = 4;
	if (doubleClick) {
		minCount = 3;
	}
	if (impl->m_selectedPolygon == nullptr) {return;}
	if (impl->m_selectedPolygon->cleanedPolygon().count() <= minCount) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Polygon must have three vertices at least."));
		return;
	}
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.undo();
	stack.beginMacro(tr("Finish Defining Polygon"));
	// finish defining the polygon.
	stack.push(new FinishPolygonDefiningCommand(this));
	stack.endMacro();
	if (impl->m_selectedPolygon == impl->m_regionPolygon && (! noEditVal)) {
		editValue();
	}
}

QColor GeoDataPolygon::color() const
{
	return impl->m_setting.color;
}

void GeoDataPolygon::setShape(geos::geom::Polygon* polygon)
{
	auto ls = polygon->getExteriorRing();
	impl->m_regionPolygon->setPolygon(buildPolygon(ls));
	clearHolePolygons();

	for (int i = 0; i < polygon->getNumInteriorRing(); ++i) {
		auto holePol = setupHolePolygon();
		holePol->setActive(false);
		ls = polygon->getInteriorRingN(i);
		holePol->setPolygon(buildPolygon(ls));
		impl->m_holePolygons.push_back(holePol);
	}
	impl->m_selectedPolygon = impl->m_regionPolygon;
	updatePolyData();
	updateActionStatus();
}

void GeoDataPolygon::addVertexMode(bool on)
{
	if (on) {
		impl->m_mouseEventMode = meAddVertexNotPossible;
	} else {
		impl->m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPolygon::removeVertexMode(bool on)
{
	if (on) {
		impl->m_mouseEventMode = meRemoveVertexNotPossible;
	} else {
		impl->m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPolygon::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_setting.load(node);
}

void GeoDataPolygon::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_setting.save(writer);
}

void GeoDataPolygon::loadExternalData(const QString& filename)
{
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		impl->m_regionPolygon->setLookupTable(stcc->vtkDarkObj());
	}
	if (projectData()->version().build() >= 3607) {
		iRICLib::Polygon* pol = new iRICLib::Polygon();
		GridAttributeDimensionsContainer* dims = dimensions();
		bool noDim = true;
		if (dims != nullptr) {
			noDim = dims->containers().size() == 0;
		}

		pol->load(iRIC::toStr(filename).c_str(), noDim);
		impl->m_variantValues.clear();
		for (unsigned int i = 0; i < pol->values.size(); ++i) {
			impl->m_variantValues.push_back(pol->values[i]);
		}
		QPolygonF qpol;
		iRICLib::InternalPolygon* regionPolygon = pol->polygon;
		for (int i = 0; i < regionPolygon->pointCount; ++i) {
			qpol << QPointF(*(regionPolygon->x + i), *(regionPolygon->y + i));
		}
		impl->m_regionPolygon->setPolygon(qpol);

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
			impl->m_holePolygons.push_back(holePol);
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
		impl->m_regionPolygon->setPolygon(poly);

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
				impl->m_holePolygons.push_back(holePol);
			}
		}
		f.close();
	}

	if (impl->m_regionPolygon->polygon().size() > 0) {
		impl->m_mouseEventMode = meNormal;
		informDeselection(graphicsView());
	} else {
		impl->m_mouseEventMode = meBeforeDefining;
		impl->m_selectMode = smPolygon;
		impl->m_selectedPolygon = impl->m_regionPolygon;
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
	for (int i = 0; i < impl->m_variantValues.size(); ++i) {
		pol.values.push_back(impl->m_variantValues.at(i).toDouble());
	}
	iRICLib::InternalPolygon* regionPolygon = new iRICLib::InternalPolygon();
	QPolygonF qpol = impl->m_regionPolygon->polygon();
	regionPolygon->pointCount = qpol.count();
	regionPolygon->x = new double[regionPolygon->pointCount];
	regionPolygon->y = new double[regionPolygon->pointCount];
	for (int i = 0; i < regionPolygon->pointCount; ++i) {
		*(regionPolygon->x + i) = qpol.at(i).x();
		*(regionPolygon->y + i) = qpol.at(i).y();
	}
	pol.polygon = regionPolygon;
	for (int i = 0; i < impl->m_holePolygons.size(); ++i) {
		iRICLib::InternalPolygon* holePolygon = new iRICLib::InternalPolygon();
		QPolygonF hqpol = impl->m_holePolygons[i]->polygon();
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
	impl->m_depthRange = range;
	impl->m_regionPolygon->setZDepthRange(range.min(), range.max());
	for (int i = 0; i < impl->m_holePolygons.size(); ++i) {
		GeoDataPolygonHolePolygon* p = impl->m_holePolygons[i];
		p->setZDepthRange(range.min(), range.max());
	}
	impl->m_actor.actor()->SetPosition(0, 0, range.min());
}

void GeoDataPolygon::updateMouseEventMode()
{
	double dx, dy;
	dx = impl->m_currentPoint.x();
	dy = impl->m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF worldPos(dx, dy);
	bool shapeUpdating = impl->m_shapeUpdating;
	shapeUpdating = shapeUpdating || (impl->m_triangleThread != nullptr && impl->m_triangleThread->isOutputting(this));
	switch (impl->m_mouseEventMode) {
	case meAddVertexNotPossible:
	case meAddVertexPrepare:
		if (impl->m_selectMode == smNone) {return;}
		if (impl->m_selectMode == smPolygon) {
			if (shapeUpdating) {
				impl->m_mouseEventMode = meAddVertexNotPossible;
			} else if (impl->m_selectedPolygon->isEdgeSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				impl->m_mouseEventMode = meAddVertexPrepare;
			} else {
				impl->m_mouseEventMode = meAddVertexNotPossible;
			}
		}
		break;
	case meRemoveVertexNotPossible:
	case meRemoveVertexPrepare:
		if (impl->m_selectMode == smNone) {return;}
		if (impl->m_selectMode == smPolygon) {
			if (shapeUpdating) {
				impl->m_mouseEventMode = meRemoveVertexNotPossible;
			} else if (impl->m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				impl->m_mouseEventMode = meRemoveVertexPrepare;
			} else {
				impl->m_mouseEventMode = meRemoveVertexNotPossible;
			}
		}
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		if (impl->m_selectMode == smNone) {return;}
		if (impl->m_selectMode == smPolygon) {
			if (shapeUpdating) {
				impl->m_mouseEventMode = meNormal;
			} else if (impl->m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				impl->m_mouseEventMode = meMoveVertexPrepare;
			} else if (impl->m_selectedPolygon == impl->m_regionPolygon && impl->m_selectedPolygon->isPolygonSelectable(worldPos)) {
				impl->m_mouseEventMode = meTranslatePrepare;
			} else {
				impl->m_mouseEventMode = meNormal;
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
	switch (impl->m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
		impl->m_addVertexAction->setDisabled(true);
		impl->m_addVertexAction->setChecked(false);
		impl->m_removeVertexAction->setDisabled(true);
		impl->m_removeVertexAction->setChecked(false);
		impl->m_coordEditAction->setEnabled(false);

		impl->m_holeModeAction->setDisabled(true);
		impl->m_holeModeAction->setChecked(false);
		impl->m_deleteAction->setDisabled(true);
		if (dynamic_cast<GeoDataPolygonRegionPolygon*>(impl->m_selectedPolygon) != nullptr) {
//			m_defineModeAction->setChecked(true);
		} else if (dynamic_cast<GeoDataPolygonHolePolygon*>(impl->m_selectedPolygon) != nullptr) {
			impl->m_holeModeAction->setChecked(true);
		}
		break;
	case meTranslate:
	case meMoveVertex:
		impl->m_addVertexAction->setDisabled(true);
		impl->m_addVertexAction->setChecked(false);
		impl->m_removeVertexAction->setDisabled(true);
		impl->m_removeVertexAction->setChecked(false);
		impl->m_coordEditAction->setDisabled(true);

		impl->m_holeModeAction->setDisabled(true);
		impl->m_deleteAction->setDisabled(true);
		break;

		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		impl->m_addVertexAction->setChecked(false);
		impl->m_removeVertexAction->setChecked(false);

		if (impl->m_selectMode != smNone) {
			impl->m_addVertexAction->setEnabled(true);
			if (impl->m_selectMode == smPolygon) {
				impl->m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
			}
			impl->m_coordEditAction->setEnabled(true);
		} else {
			impl->m_addVertexAction->setDisabled(true);
			impl->m_removeVertexAction->setDisabled(true);
			impl->m_coordEditAction->setDisabled(true);
		}

		impl->m_holeModeAction->setEnabled(true);
		impl->m_holeModeAction->setChecked(false);
		if (impl->m_selectedPolygon != nullptr) {
			impl->m_addVertexAction->setEnabled(true);
			impl->m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
			impl->m_coordEditAction->setEnabled(true);
			impl->m_deleteAction->setEnabled(impl->m_selectedPolygon != impl->m_regionPolygon);
		} else {
			impl->m_addVertexAction->setDisabled(true);
			impl->m_removeVertexAction->setDisabled(true);
			impl->m_coordEditAction->setDisabled(true);
			impl->m_deleteAction->setDisabled(true);
		}
		break;
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meAddVertex:
		impl->m_addVertexAction->setChecked(true);
		impl->m_removeVertexAction->setChecked(false);

		if (impl->m_selectMode != smNone) {
			impl->m_addVertexAction->setEnabled(true);
			if (impl->m_selectMode == smPolygon) {
				impl->m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
			}
			impl->m_coordEditAction->setEnabled(true);
		} else {
			impl->m_addVertexAction->setDisabled(true);
			impl->m_removeVertexAction->setDisabled(true);
			impl->m_coordEditAction->setDisabled(true);
		}
		impl->m_holeModeAction->setDisabled(true);
		impl->m_holeModeAction->setChecked(false);
		impl->m_deleteAction->setEnabled(impl->m_selectedPolygon != impl->m_regionPolygon);

		break;
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		impl->m_addVertexAction->setEnabled(true);
		impl->m_addVertexAction->setChecked(false);
		impl->m_removeVertexAction->setEnabled(true);
		impl->m_removeVertexAction->setChecked(true);
		impl->m_coordEditAction->setEnabled(false);

		impl->m_holeModeAction->setDisabled(true);
		impl->m_holeModeAction->setChecked(false);
		impl->m_deleteAction->setEnabled(impl->m_selectedPolygon != impl->m_regionPolygon);
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
	impl->m_mouseEventMode = meNormal;
}

void GeoDataPolygon::clear()
{
	initParams();
	clearHolePolygons();
	delete impl->m_regionPolygon;

	impl->m_regionPolygon = new GeoDataPolygonRegionPolygon(this);
	impl->m_regionPolygon->setSelected(true);
	impl->m_regionPolygon->setZDepthRange(impl->m_depthRange.min(), impl->m_depthRange.max());
	impl->m_mouseEventMode = meBeforeDefining;
	impl->m_selectedPolygon = impl->m_regionPolygon;
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
	impl->m_variantValues.clear();

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
		impl->m_variantValues.push_back(defaultValue);
	}
}

void GeoDataPolygon::addHolePolygon()
{
	auto holePol = setupHolePolygon();
	if (impl->m_selectedPolygon != nullptr) {
		impl->m_selectedPolygon->setSelected(false);
	}
	pushRenderCommand(new AddHolePolygonCommand(holePol, this));
	InformationDialog::information(preProcessorWindow(), GeoDataPolygon::tr("Information"), GeoDataPolygon::tr("Please define hole region. Hole region can be defined as polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gctriangle_addholepolygon");
}

vtkPolyData* GeoDataPolygon::polyData() const
{
	return impl->m_polyData;
}

GeoDataPolygonHolePolygon* GeoDataPolygon::setupHolePolygon()
{
	GeoDataPolygonHolePolygon* pol = new GeoDataPolygonHolePolygon(this);

	pol->setZDepthRange(impl->m_depthRange.min(), impl->m_depthRange.max());
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		pol->setLookupTable(scalarsToColorsContainer()->vtkDarkObj());
	}
	pol->setActive(true);
	pol->setMapping(impl->m_setting.mapping);
	pol->setColor(impl->m_setting.color);

	return pol;
}

void GeoDataPolygon::deletePolygon(bool force)
{
	if (impl->m_selectedPolygon == nullptr) {return;}
	if (! force) {
		int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to remove this polygon?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	if (dynamic_cast<GeoDataPolygonRegionPolygon*>(impl->m_selectedPolygon) != nullptr) {
		delete impl->m_regionPolygon;
		impl->m_regionPolygon = new GeoDataPolygonRegionPolygon(this);
		impl->m_regionPolygon->setActive(true);
		impl->m_regionPolygon->setSelected(true);
		impl->m_regionPolygon->setZDepthRange(impl->m_depthRange.min(), impl->m_depthRange.max());
		impl->m_selectedPolygon = impl->m_regionPolygon;
		impl->m_mouseEventMode = meBeforeDefining;
	} else if (dynamic_cast<GeoDataPolygonHolePolygon*>(impl->m_selectedPolygon) != nullptr) {
		GeoDataPolygonHolePolygon* tmpPoly = dynamic_cast<GeoDataPolygonHolePolygon*>(impl->m_selectedPolygon);
		impl->m_holePolygons.removeOne(tmpPoly);
		delete impl->m_selectedPolygon;
		impl->m_selectedPolygon = nullptr;
		impl->m_selectMode = smNone;
		impl->m_mouseEventMode = meNormal;
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
	SelectMode oldSelectMode = impl->m_selectMode;
	GeoDataPolygonAbstractPolygon* oldSelectedPolygon = impl->m_selectedPolygon;
	deselectAll();

	double dx = point.x();
	double dy = point.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF p(dx, dy);

	double selectlimit = graphicsView()->stdRadius(iRIC::nearRadius());

	// find polygon that contains this point.
	GeoDataPolygonAbstractPolygon* newSelPol = nullptr;
	bool selected = false;
	for (int i = impl->m_holePolygons.size() - 1; i >= 0 && (!selected); --i) {
		GeoDataPolygonAbstractPolygon* pol = impl->m_holePolygons[i];
		QPolygonF polF = pol->polygon();
		if (polF.count() <= 3) {
			if (pol->isEdgeSelectable(p, selectlimit)) {
				newSelPol = pol;
				selected = true;
			} else if (pol->isVertexSelectable(p, selectlimit)) {
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
		QPolygonF polF = impl->m_regionPolygon->polygon();
		if (polF.count() <= 3) {
			if (impl->m_regionPolygon->isEdgeSelectable(p, selectlimit)) {
				newSelPol = impl->m_regionPolygon;
				selected = true;
			} else if (impl->m_regionPolygon->isVertexSelectable(p, selectlimit)) {
				newSelPol = impl->m_regionPolygon;
				selected = true;
			}
		} else {
			if (impl->m_regionPolygon->polygon().containsPoint(p, Qt::OddEvenFill)) {
				newSelPol = impl->m_regionPolygon;
				selected = true;
			}
		}
	}
	if (newSelPol != nullptr) {
		impl->m_selectMode = smPolygon;
		impl->m_selectedPolygon = newSelPol;
		impl->m_selectedPolygon->setSelected(true);
	}
	if (! selected) {
		impl->m_selectMode = smNone;
		impl->m_selectedPolygon = nullptr;
	}

	if (impl->m_selectMode != oldSelectMode) {return true;}
	if (impl->m_selectMode == smPolygon) {
		return impl->m_selectedPolygon != oldSelectedPolygon;
	}
	return false;
}

void GeoDataPolygon::deselectAll()
{
//	m_editMaxAreaAction->disconnect();
	if (impl->m_selectMode == smPolygon) {
		if (impl->m_selectedPolygon != nullptr) {
			impl->m_selectedPolygon->setSelected(false);
			impl->m_selectedPolygon = nullptr;
		}
	}
	impl->m_selectMode = smNone;
}

bool GeoDataPolygon::checkCondition()
{
	if (impl->m_regionPolygon->polygon().count() < 4) {
//		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid region polygon have to consists of more than three vertices."));
		return false;
	}
	if (iRIC::hasIntersection(impl->m_regionPolygon->polygon())) {
//		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid region polygon shape is invalid."));
		return false;
	}
	QPolygonF gridPol = impl->m_regionPolygon->polygon();
	QList<QPolygonF> polygons;
	for (int i = 0; i < impl->m_holePolygons.count(); ++i) {
		GeoDataPolygonHolePolygon* hpol = impl->m_holePolygons[i];
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
	if (impl->m_selectedPolygon == nullptr) {return false;}
	QPolygonF pol = impl->m_selectedPolygon->polygon();
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
	return impl->m_variantValues.at(index);
}

void GeoDataPolygon::setVariantValue(const QVariant &v, bool disableInform)
{
	int index = 0;
	GridAttributeDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	impl->m_variantValues[index] = v;
	updateScalarValues();
	if (! disableInform) {
		auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
		p->informValueRangeChange();
		p->informDataChange();
	}
}

void GeoDataPolygon::editValue()
{
	if (m_gridAttribute && m_gridAttribute->isReferenceInformation()) {return;}

	GridAttributeEditDialog* dialog = m_gridAttribute->editDialog(preProcessorWindow());
	PreProcessorGeoDataGroupDataItemInterface* i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setLabel(tr("Please input new value in this polygon.").arg(i->condition()->caption()));
	i->setupEditWidget(dialog->widget());
	dialog->setVariantValue(variantValue());
	if (impl->m_mouseEventMode == meDefining || impl->m_mouseEventMode == meBeforeDefining) {
		dialog->disableCancel();
	}
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {return;}
	pushCommand(new EditValueCommand(dialog->variantValue(), this));
}

void GeoDataPolygon::addHolePolygon(const QPolygonF& p)
{
	auto newPol = setupHolePolygon();
	newPol->setSelected(false);
	newPol->setPolygon(p);
	impl->m_holePolygons.append(newPol);
}

void GeoDataPolygon::updateScalarValues()
{
	vtkPoints* points = impl->m_polyData->GetPoints();
	if (points == nullptr) {return;}
	impl->m_scalarValues->Reset();
	double doubleval = variantValue().toDouble();
	for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
		impl->m_scalarValues->InsertNextValue(doubleval);
	}
	impl->m_scalarValues->Modified();
	impl->m_regionPolygon->updateScalarValues();
	for (auto hole : impl->m_holePolygons) {
		hole->updateScalarValues();
	}
}

void GeoDataPolygon::updatePolyData(bool noDraw)
{
	if (impl->m_triangleThread != nullptr && impl->m_triangleThread->isOutputting(this)){
		// it has already started outputting. Wait until it ends.
		while (impl->m_triangleThread->isOutputting(this)){
			impl->m_triangleThread->wait(100);
		}
	}
	setupTriangleThread();

	impl->m_triangleThread->addJob(this, noDraw);
	if (! noDraw){
		impl->m_actor.actor()->SetVisibility(0);
	}
}

void GeoDataPolygon::editColorSetting()
{
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->showPropertyDialog();
}

void GeoDataPolygon::updateActorSettings()
{
	// color
	impl->m_regionPolygon->setColor(impl->m_setting.color);
	for (auto hole : impl->m_holePolygons) {
		hole->setColor(impl->m_setting.color);
	}
	impl->m_actor.actor()->GetProperty()->SetColor(impl->m_setting.color);

	// opacity
	impl->m_actor.actor()->GetProperty()->SetOpacity(impl->m_setting.opacity);

	// mapping
	if (impl->m_setting.mapping == GeoDataPolygonColorSettingDialog::Arbitrary) {
		impl->m_actor.mapper()->SetScalarVisibility(false);
	} else {
		impl->m_actor.mapper()->SetScalarVisibility(true);
	}
	impl->m_regionPolygon->setMapping(impl->m_setting.mapping);
	for (auto hole : impl->m_holePolygons) {
		hole->setMapping(impl->m_setting.mapping);
	}
}

QDialog* GeoDataPolygon::propertyDialog(QWidget* parent)
{
	GeoDataPolygonColorSettingDialog* dialog = new GeoDataPolygonColorSettingDialog(parent);
	dialog->setSetting(impl->m_setting);
	auto gridAtt = gridAttribute();
	if (gridAtt != nullptr) {
		dialog->setIsReferenceInformation(gridAtt->isReferenceInformation());
	}

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
	if (impl->m_selectedPolygon != impl->m_regionPolygon) {return true;}
	switch (impl->m_mouseEventMode) {
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
	impl->m_actor.actor()->SetVisibility(visibility);
	renderGraphicsView();
}

void GeoDataPolygon::updatePolygon(GeoDataPolygon* polygon, vtkPoints* points, vtkCellArray* ca, bool noDraw)
{
	if (polygon != this) {return;}

	impl->m_triangleThread->lockMutex();

	impl->m_polyData->SetPoints(points);
	points->Delete();

	impl->m_polyData->SetPolys(ca);
	ca->Delete();

	updateScalarValues();

	impl->m_polyData->BuildCells();
	impl->m_polyData->BuildLinks();
	impl->m_polyData->Modified();

	impl->m_triangleThread->unlockMutex();

	impl->m_actor.actor()->GetProperty()->SetOpacity(impl->m_setting.opacity);

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
	impl->m_regionPolygon->setPolygon(polygon->regionPolygon()->polygon());
	for (int i = 0; i < polygon->holePolygons().size(); ++i) {
		GeoDataPolygonHolePolygon* p = polygon->holePolygons().at(i);
		auto holePol = setupHolePolygon();
		holePol->setPolygon(p->polygon());
		holePol->setActive(false);
		holePol->setSelected(false);
		impl->m_holePolygons.append(holePol);
	}
	updatePolyData();
	impl->m_mouseEventMode = meNormal;
	editValue();
	// copy command is not undo-able.
	iRICUndoStack::instance().clear();
}

void GeoDataPolygon::doApplyOffset(double x, double y)
{
	applyOffsetToAbstractPolygon(impl->m_regionPolygon, x, y);
	for (GeoDataPolygonHolePolygon* pol : impl->m_holePolygons) {
		applyOffsetToAbstractPolygon(pol, x, y);
	}
	updatePolyData(true);
}

int GeoDataPolygon::iRICLibType() const
{
	return IRIC_GEO_POLYGON;
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
	for (auto p : impl->m_holePolygons) {
		delete p;
	}
	impl->m_holePolygons.clear();
}

void GeoDataPolygon::lockMutex()
{
	if (impl->m_triangleThread == nullptr) {return;}

	impl->m_triangleThread->lockMutex();
}

void GeoDataPolygon::unlockMutex()
{
	if (impl->m_triangleThread == nullptr) {return;}

	impl->m_triangleThread->unlockMutex();
}

GeoDataPolygon::MouseEventMode GeoDataPolygon::mouseEventMode() const
{
	return impl->m_mouseEventMode;
}

GeoDataPolygon::SelectMode GeoDataPolygon::selectMode() const
{
	return impl->m_selectMode;
}

void GeoDataPolygon::setBCSettingMode(bool mode)
{
	impl->m_bcSettingMode = mode;
}

geos::geom::Polygon* GeoDataPolygon::getGeosPolygon(const QPointF& offset)
{
	setupTriangleThread();
	return impl->m_triangleThread->getGeosPolygon(this, offset);
}

void GeoDataPolygon::setupTriangleThread()
{
	if (impl->m_triangleThread != nullptr) {return;}

	impl->m_triangleThread = GeoDataPolygonTriangleThread::instance();
	connect(impl->m_triangleThread, SIGNAL(shapeUpdated(GeoDataPolygon*,vtkPoints*,vtkCellArray*,bool)), this, SLOT(updatePolygon(GeoDataPolygon*,vtkPoints*,vtkCellArray*,bool)));
}

GeoDataPolygonRegionPolygon* GeoDataPolygon::regionPolygon() const
{
	return impl->m_regionPolygon;
}

GeoDataPolygonAbstractPolygon* GeoDataPolygon::selectedPolygon() const
{
	return impl->m_selectedPolygon;
}

void GeoDataPolygon::setSelectedPolygon(GeoDataPolygonAbstractPolygon* pol)
{
	impl->m_selectedPolygon = pol;
}

const QList<GeoDataPolygonHolePolygon*>& GeoDataPolygon::holePolygons() const
{
	return impl->m_holePolygons;
}

QList<GeoDataPolygonHolePolygon*>& GeoDataPolygon::holePolygons()
{
	return impl->m_holePolygons;
}

vtkActor* GeoDataPolygon::paintActor() const
{
	return impl->m_actor.actor();
}

vtkMapper* GeoDataPolygon::paintMapper() const
{
	return impl->m_actor.mapper();
}

QAction* GeoDataPolygon::editValueAction() const
{
	return impl->m_editValueAction;
}

QAction* GeoDataPolygon::addVertexAction() const
{
	return impl->m_addVertexAction;
}

QAction* GeoDataPolygon::removeVertexAction() const
{
	return impl->m_removeVertexAction;
}

QAction* GeoDataPolygon::coordEditAction() const
{
	return impl->m_coordEditAction;
}

QAction* GeoDataPolygon::holeModeAction() const
{
	return impl->m_holeModeAction;
}

QAction* GeoDataPolygon::deleteAction() const
{
	return impl->m_deleteAction;
}

void GeoDataPolygon::setShapeUpdating(bool updating)
{
	impl->m_shapeUpdating = updating;
}
