#include "geodatapoint.h"

#include "private/geodatapoint_coordinateseditor.h"
#include "private/geodatapoint_finishpointdefinitioncommand.h"
#include "private/geodatapoint_movevertexcommand.h"
#include "private/geodatapoint_setvertexcommand.h"
#include "private/geodatapoint_impl.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <misc/informationdialog.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>
#include <QToolBar>

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <vector>

namespace {

const std::string SCALARNAME = "pointvalue";

}

GeoDataPoint::Impl::Impl(GeoDataPoint* parent) :
	m_parent {parent},
	m_actor {vtkActor::New()},
	m_scalarValues {vtkDoubleArray::New()},
	m_pointController {},
	m_mouseEventMode {meBeforeDefining},
	m_dragStartPoint {},
	m_currentPoint {},
	m_rightClickingMenu {new QMenu()},
	m_coordEditAction {new QAction(GeoDataPoint::tr("Edit &Coordinates..."), m_parent)}
{
	connect(m_coordEditAction, SIGNAL(triggered()), m_parent, SLOT(editCoordinates()));

	setupScalarValues();

}

GeoDataPoint::Impl::~Impl()
{
	m_actor->Delete();
	m_scalarValues->Delete();
	delete m_rightClickingMenu;
}

void GeoDataPoint::Impl::setupScalarValues()
{
	m_scalarValues->SetName(SCALARNAME.c_str());
	auto cd = m_pointController.polyData()->GetCellData();
	cd->AddArray(m_scalarValues);
	cd->SetActiveScalars(SCALARNAME.c_str());
}

GeoDataPoint::GeoDataPoint(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataPolyData(d, creator, condition),
	impl {new Impl {this}}
{
	setMapping(GeoDataPolyDataColorSettingDialog::Arbitrary);
	setColor(Qt::black);
	setOpacity(100);

	renderer()->AddActor(impl->m_actor);
	actorCollection()->AddItem(impl->m_actor);

	updateActorSettings();
	updateVisibilityWithoutRendering();
}

GeoDataPoint::~GeoDataPoint()
{
	renderer()->RemoveActor(impl->m_actor);
	actorCollection()->RemoveItem(impl->m_actor);

	delete impl;
}

void GeoDataPoint::setPointSize(int pointSize)
{
	impl->m_actor->GetProperty()->SetPointSize(pointSize);
}

void GeoDataPoint::setupMenu()
{
	m_menu->setTitle(tr("Po&int"));
	m_menu->addAction(editNameAction());
	m_menu->addSeparator();
	m_menu->addAction(editColorSettingAction());
	m_menu->addSeparator();
	m_menu->addAction(deleteAction());

	impl->m_rightClickingMenu->addAction(editColorSettingAction());
}

bool GeoDataPoint::addToolBarButtons(QToolBar* /*parent*/)
{
	return false;
}

void GeoDataPoint::informSelection(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GeoDataPoint::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	v->unsetCursor();
}

void GeoDataPoint::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
}

void GeoDataPoint::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GeoDataPoint::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_mouseEventMode) {
	case meNormal:
	case meMoveVertexPrepare:
		impl->m_currentPoint = event->pos();
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case meBeforeDefining:
		// do nothing.
		break;
	case meDefining:
		pushRenderCommand(new SetVertexCommand(true, v->viewportToWorld(event->pos()), this));
		break;
	case meMoveVertex:
		pushRenderCommand(new MoveVertexCommand(true, v->viewportToWorld(event->pos()), this));
		impl->m_currentPoint = event->pos();
		break;
	default:
		break;
	}
}

void GeoDataPoint::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_mouseEventMode) {
		case meBeforeDefining:
			pushRenderCommand(new SetVertexCommand(true, v->viewportToWorld(event->pos()), this));
			impl->m_mouseEventMode = meDefining;
			break;
		case meMoveVertexPrepare:
			impl->m_currentPoint = event->pos();
			pushRenderCommand(new MoveVertexCommand(true, v->viewportToWorld(event->pos()), this));
			impl->m_mouseEventMode = meMoveVertex;
			updateMouseCursor(v);
			break;
		default:
			break;
		}
	} else if (event->button() == Qt::RightButton) {
		impl->m_dragStartPoint = event->pos();
	}
}

void GeoDataPoint::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_mouseEventMode) {
		case meNormal:
		case meMoveVertexPrepare:
		case meMoveVertex:
			impl->m_currentPoint = event->pos();
			updateMouseEventMode();
			updateMouseCursor(v);
			break;
		case meDefining:
			pushCommand(new FinishPointDefiningCommand(this));
			updateMouseCursor(v);
			break;
		default:
			break;
		}
	} else if (event->button() == Qt::RightButton) {
		if (impl->m_mouseEventMode == meEditCoordinatesDialog) {return;}
		if (iRIC::isNear(impl->m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GeoDataPoint::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPoint::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.min());
}

void GeoDataPoint::getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax)
{
	auto p = point();
	*xmin = p.x();
	*xmax = p.x();
	*ymin = p.y();
	*ymax = p.y();
}

bool GeoDataPoint::inNormalMode() const
{
	return impl->m_mouseEventMode == meNormal;
}

bool GeoDataPoint::isDefined() const
{
	return impl->m_pointController.pointIsSet();
}

void GeoDataPoint::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GeoDataPoint::tr("Information"), GeoDataPoint::tr("Please define point by mouse-clicking."), "geodatapointinit");
}

QPointF GeoDataPoint::point() const
{
	return impl->m_pointController.point();
}

void GeoDataPoint::setPoint(const QPointF& point)
{
	impl->m_pointController.setPoint(point);
}

void GeoDataPoint::restoreMouseEventMode()
{
	impl->m_mouseEventMode = meNormal;
}

void GeoDataPoint::editCoordinates()
{
	CoordinatesEditor::edit(this);
}

void GeoDataPoint::doApplyOffset(double x, double y)
{
	auto p = point();
	p.setX(p.x() - x);
	p.setY(p.y() - y);
	setPoint(p);
}

GeoDataPoint::MouseEventMode GeoDataPoint::mouseEventMode() const
{
	return impl->m_mouseEventMode;
}

void GeoDataPoint::setMouseEventMode(MouseEventMode mode)
{
	impl->m_mouseEventMode = mode;
}

void GeoDataPoint::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
		v->setCursor(Qt::CrossCursor);
		break;
	case meMoveVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	case meMoveVertexPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	default:
		v->setCursor(Qt::ArrowCursor);
		break;
	}
}

void GeoDataPoint::updateScalarValues()
{
	impl->m_scalarValues->Reset();
	double doubleval = variantValue().toDouble();
	impl->m_scalarValues->InsertNextValue(doubleval);
	impl->m_scalarValues->Modified();
}

void GeoDataPoint::updateActorSettings()
{
	auto cs = colorSetting();
	// color
	impl->m_actor->GetProperty()->SetColor(cs.color);

	// opacity
	impl->m_actor->GetProperty()->SetOpacity(cs.opacity);

	// mapping
	if (cs.mapping == GeoDataPolyDataColorSettingDialog::Arbitrary) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(impl->m_pointController.polyData());
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
	} else {
		auto cs = colorMapSettingContainer();
		auto mapper = cs->buildCellDataMapper(impl->m_pointController.polyData(), true);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
	}

	impl->m_pointController.actor()->GetProperty()->SetColor(cs.color);
	impl->m_pointController.actor()->GetProperty()->SetOpacity(cs.opacity);
}

void GeoDataPoint::updateMouseEventMode()
{
	double dx, dy;
	dx = impl->m_currentPoint.x();
	dy = impl->m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF worldPos(dx, dy);
	switch (impl->m_mouseEventMode) {
	case meNormal:
	case meMoveVertexPrepare:
	case meMoveVertex:
		if (impl->m_pointController.isSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
			impl->m_mouseEventMode = meMoveVertexPrepare;
		} else {
			impl->m_mouseEventMode = meNormal;
		}
		break;
	default:
		break;
	}
}

bool GeoDataPoint::isReady() const
{
	return true;
}

QString GeoDataPoint::shapeName() const
{
	return tr("point");
}

QString GeoDataPoint::shapeNameCamelCase() const
{
	return tr("Point");
}

QAction* GeoDataPoint::coordEditAction() const
{
	return impl->m_coordEditAction;
}
