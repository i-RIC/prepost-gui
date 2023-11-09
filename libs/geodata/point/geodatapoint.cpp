#include "geodatapoint.h"
#include "geodatapointproxy.h"

#include "private/geodatapoint_coordinateseditor.h"
#include "private/geodatapoint_finishpointdefinitioncommand.h"
#include "private/geodatapoint_movevertexcommand.h"
#include "private/geodatapoint_setvertexcommand.h"
#include "private/geodatapoint_impl.h"
#include "public/geodatapoint_displaysettingwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/pre/geodata/private/geodata_propertydialog.h>
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
#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageMapper.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkQImageToImageSource.h>
#include <vtkRenderer.h>

#include <vector>

namespace {

const std::string SCALARNAME = "pointvalue";

}

GeoDataPoint::Impl::Impl(GeoDataPoint* parent) :
	m_parent {parent},
	m_actor {vtkActor::New()},
	m_imageActor {vtkActor2D::New()},
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
	m_imageActor->Delete();
	m_scalarValues->Delete();
	delete m_rightClickingMenu;
}

QPixmap GeoDataPoint::Impl::shrinkPixmap(const QPixmap pixmap, int maxSize)
{
	if (pixmap.width() <= maxSize && pixmap.height() <= maxSize) {return pixmap;}

	if (pixmap.width() > pixmap.height()) {
		return pixmap.scaledToWidth(maxSize);
	} else {
		return pixmap.scaledToHeight(maxSize);
	}
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
	renderer()->AddActor(impl->m_actor);

	auto att = gridAttribute();
	if (att && att->isReferenceInformation()) {
		impl->m_displaySetting.mapping = DisplaySetting::Mapping::Arbitrary;
	}

	updateActorSetting();
}

GeoDataPoint::~GeoDataPoint()
{
	renderer()->RemoveActor(impl->m_actor);

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

void GeoDataPoint::informSelection(PreProcessorGraphicsViewI* v)
{
	updateMouseCursor(v);
}

void GeoDataPoint::informDeselection(PreProcessorGraphicsViewI* v)
{
	v->unsetCursor();
}

void GeoDataPoint::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
}

void GeoDataPoint::viewOperationEnded(PreProcessorGraphicsViewI* v)
{
	updateMouseCursor(v);
}

void GeoDataPoint::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
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

void GeoDataPoint::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
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

void GeoDataPoint::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
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

QDialog* GeoDataPoint::propertyDialog(QWidget* parent)
{
	auto dialog = new PropertyDialog(this, parent);
	auto widget = new DisplaySettingWidget(dialog);

	if (geoDataGroupDataItem()->condition()->isReferenceInformation()) {
		widget->setIsReferenceInformation(true);
	}

	widget->setSetting(&impl->m_displaySetting);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Point Display Setting"));

	return dialog;
}

void GeoDataPoint::showPropertyDialog()
{
	showPropertyDialogModeless();
}

GeoDataProxy* GeoDataPoint::getProxy()
{
	return new GeoDataPointProxy(this);
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

void GeoDataPoint::updateMouseCursor(PreProcessorGraphicsViewI* v)
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

void GeoDataPoint::updateActorSetting()
{
	auto r = renderer();
	impl->m_actor->VisibilityOff();
	impl->m_imageActor->VisibilityOff();

	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	auto& ds = impl->m_displaySetting;

	if (ds.shape == DisplaySetting::Shape::Point || ds.image.isNull()) {
		// color
		impl->m_actor->GetProperty()->SetColor(ds.color);

		auto cm = colorMapSettingContainer();
		if (cm == nullptr) {
			ds.mapping = DisplaySetting::Mapping::Arbitrary;
		}

		// mapping
		if (ds.mapping == DisplaySetting::Mapping::Value) {
			vtkMapper* mapper = nullptr;

			mapper = cm->buildCellDataMapper(impl->m_pointController.polyData(), true);
			impl->m_actor->SetMapper(mapper);
			mapper->Delete();
		} else {
			vtkPolyDataMapper* mapper = nullptr;

			mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
			mapper->SetInputData(impl->m_pointController.polyData());
			impl->m_actor->SetMapper(mapper);
			mapper->Delete();
		}

		// opacity
		impl->m_actor->GetProperty()->SetOpacity(ds.opacity);

		// pointSize
		impl->m_actor->GetProperty()->SetPointSize(ds.pointSize);

		actorCollection()->AddItem(impl->m_actor);
	} else {
		auto pixmap = QPixmap::fromImage(impl->m_displaySetting.image);
		impl->m_shrinkedImage = Impl::shrinkPixmap(pixmap, impl->m_displaySetting.imageMaxSize).toImage();
		auto imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
		imgToImg->SetQImage(&impl->m_shrinkedImage);
		auto imageInfo = vtkSmartPointer<vtkImageChangeInformation>::New();
		imageInfo->SetInputConnection(imgToImg->GetOutputPort());
		imageInfo->CenterImageOn();

		auto mapper = vtkSmartPointer<vtkImageMapper>::New();
		mapper->SetColorWindow(255);
		mapper->SetColorLevel(127.5);
		mapper->SetInputConnection(imageInfo->GetOutputPort());

		impl->m_imageActor->SetMapper(mapper);
		impl->m_imageActor->GetProperty()->SetOpacity(impl->m_displaySetting.opacity);

		auto coord = impl->m_imageActor->GetPositionCoordinate();
		coord->SetCoordinateSystemToWorld();
		auto p = impl->m_pointController.point();
		coord->SetValue(p.x(), p.y(), 0);

		actor2DCollection()->AddItem(impl->m_imageActor);
	}
	updateVisibilityWithoutRendering();

	emit updateActorSettingExecuted();
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
