#include "geodatapoint.h"
#include "geodatapointcolorsimplesettingdialog.h"

#include "private/geodatapoint_editnameandvaluecommand.h"
#include "private/geodatapoint_editpropertycommand.h"
#include "private/geodatapoint_finishpointdefinitioncommand.h"
#include "private/geodatapoint_impl.h"

#include <guibase/point/pointmovecommand.h>
#include <guibase/point/pointsetcommand.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/pre/gridcond/base/gridattributeeditnameandvaluedialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>

#include <QAction>
#include <QFile>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPointF>
#include <QStandardItem>
#include <QToolBar>

#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <vector>

GeoDataPoint::Impl::Impl(GeoDataPoint* parent) :
	m_parent {parent},
	m_pointController {},
	m_setting {},
	m_variantValues {},
	m_mouseEventMode {meBeforeDefining},
	m_dragStartPoint {},
	m_currentPoint {},
	m_rightClickingMenu {new QMenu()},
	m_editNameAction {new QAction(GeoDataPoint::tr("Edit &Name"), m_parent)},
	m_editValueAction {new QAction(GeoDataPoint::tr("Edit &Value..."), m_parent)},
	m_editNameAndValueAction {new QAction(GeoDataPoint::tr("Edit &Name and Value..."), m_parent)},
	m_coordEditAction {new QAction(GeoDataPoint::tr("Edit &Coordinates..."), m_parent)},
	m_editColorSettingAction {new QAction(GeoDataPoint::tr("Color &Setting..."), m_parent)}
{
	QObject::connect(m_editNameAction, SIGNAL(triggered()), m_parent, SLOT(editName()));
	QObject::connect(m_editValueAction, SIGNAL(triggered()), m_parent, SLOT(editValue()));
	QObject::connect(m_editNameAndValueAction, SIGNAL(triggered()), m_parent, SLOT(editNameAndValue()));
	QObject::connect(m_coordEditAction, SIGNAL(triggered()), m_parent, SLOT(editCoordinates()));
	QObject::connect(m_editColorSettingAction, SIGNAL(triggered()), m_parent, SLOT(editColorSetting()));
}

GeoDataPoint::Impl::~Impl()
{
	delete m_rightClickingMenu;
}

GeoDataPoint::GeoDataPoint(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData(d, creator, condition),
	impl {new Impl {this}}
{
	initParams();

	impl->m_pointController.actor()->GetProperty()->SetPointSize(5);
	auto stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		impl->m_pointController.actor()->GetMapper()->SetLookupTable(stcc->vtkObj());
	}
	renderer()->AddActor(impl->m_pointController.actor());
	actorCollection()->AddItem(impl->m_pointController.actor());

	updateActorSettings();
	updateVisibilityWithoutRendering();
}

GeoDataPoint::~GeoDataPoint()
{

	renderer()->RemoveActor(impl->m_pointController.actor());
	actorCollection()->RemoveItem(impl->m_pointController.actor());

	delete impl;
}

void GeoDataPoint::setCaptionAndEmitEdited(const QString& caption)
{
	pushCommand(new EditNameAndValueCommand(caption, variantValue(), this));
}

void GeoDataPoint::setVariantValueAndEmitEdited(const QVariant& value)
{
	pushCommand(new EditNameAndValueCommand(caption(), value, this));
}

void GeoDataPoint::setupMenu()
{
	m_menu->setTitle(tr("Po&int"));
	m_menu->addAction(m_editNameAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_editColorSettingAction);
	m_menu->addSeparator();
	m_menu->addAction(deleteAction());

	impl->m_rightClickingMenu->addAction(impl->m_editColorSettingAction);
}

bool GeoDataPoint::addToolBarButtons(QToolBar* /*parent*/)
{
	return false;
}

void GeoDataPoint::informSelection(PreProcessorGraphicsViewInterface* v)
{
	// @todo
	// impl->m_polyLine->setActive(true);

	updateMouseCursor(v);
}

void GeoDataPoint::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	// @todo
	// impl->m_polyLine->setActive(false);

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
		pushRenderCommand(new PointSetCommand(false, v->viewportToWorld(event->pos()), &(impl->m_pointController)));
		break;
	case meMoveVertex:
		pushRenderCommand(new PointMoveCommand(false, v->viewportToWorld(event->pos()), &(impl->m_pointController)));
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
			pushRenderCommand(new PointSetCommand(true, v->viewportToWorld(event->pos()), &(impl->m_pointController)));
			impl->m_mouseEventMode = meDefining;
			break;
		case meMoveVertexPrepare:
			impl->m_currentPoint = event->pos();
			pushRenderCommand(new PointMoveCommand(true, v->viewportToWorld(event->pos()), &(impl->m_pointController)));
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
	impl->m_pointController.actor()->SetPosition(0, 0, range.min());
}

void GeoDataPoint::definePoint(bool doubleClick, bool noEditVal)
{
	// @todo remove this?
}

QColor GeoDataPoint::color() const
{
	return impl->m_setting.color;
}

QDialog* GeoDataPoint::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPointColorSimpleSettingDialog(parent);
	dialog->setSetting(impl->m_setting);

	return dialog;
}

void GeoDataPoint::handlePropertyDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<GeoDataPointColorSimpleSettingDialog*> (d);
	pushRenderCommand(new EditPropertyCommand(dialog->setting(), this));
}

void GeoDataPoint::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GeoDataPoint::tr("Information"), GeoDataPoint::tr("Please define point by mouse-clicking."), "geodatapointinit");
}

const QVariant& GeoDataPoint::variantValue() const
{
	int index = 0;
	auto dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	return impl->m_variantValues.at(index);
}

void GeoDataPoint::setVariantValue(const QVariant& v, bool disableInform)
{
	int index = 0;
	auto dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	impl->m_variantValues[index] = v;
	if (! disableInform) {
		auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
		p->informValueRangeChange();
		p->informDataChange();
	}
}

QPointF GeoDataPoint::point() const
{
	return impl->m_pointController.point();
}

void GeoDataPoint::setPoint(const QPointF& point)
{
	impl->m_pointController.setPoint(point);
}

bool GeoDataPoint::getValueRange(double* min, double* max)
{
	*min = variantValue().toDouble();
	*max = variantValue().toDouble();
	switch (impl->m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
		return false;
		break;
	default:
		return true;
	}
}

void GeoDataPoint::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataPoint::editName()
{
	if (! m_gridAttribute || ! m_gridAttribute->isReferenceInformation()) {return;}

	bool ok;
	QString newCaption = QInputDialog::getText(preProcessorWindow(), tr("Edit name"), tr("Name:"), QLineEdit::Normal, caption(), &ok);
	if (! ok) {return;}

	pushCommand(new EditNameAndValueCommand(newCaption, variantValue(), this));
}

void GeoDataPoint::editValue()
{
	GridAttributeEditDialog* dialog = m_gridAttribute->editDialog(preProcessorWindow());
	PreProcessorGeoDataGroupDataItemInterface* i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setLabel(tr("Please input new value at this point.").arg(i->condition()->caption()));
	i->setupEditWidget(dialog->widget());
	dialog->setVariantValue(variantValue());
	if (impl->m_mouseEventMode == meDefining || impl->m_mouseEventMode == meBeforeDefining) {
		dialog->disableCancel();
	}
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {return;}

	pushCommand(new EditNameAndValueCommand(caption(), dialog->variantValue(), this));
}

void GeoDataPoint::editNameAndValue()
{
	if (m_gridAttribute && m_gridAttribute->isReferenceInformation()) {return;}

	auto dialog = m_gridAttribute->editNameAndValueDialog(preProcessorWindow());
	auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setName(caption());
	i->setupEditWidget(dialog->widget());
	dialog->setVariantValue(variantValue());
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {
		delete dialog;
		return;
	}
	pushCommand(new EditNameAndValueCommand(dialog->name(), dialog->variantValue(), this));

	delete dialog;
}

void GeoDataPoint::restoreMouseEventMode()
{
	impl->m_mouseEventMode = meNormal;
}

void GeoDataPoint::handleDimensionCurrentIndexChange(int /*oldIndex*/, int /*newIndex*/)
{
	// @todo implement this!
}

void GeoDataPoint::handleDimensionValuesChange(const std::vector<QVariant>& /*before*/, const std::vector<QVariant>& /*after*/)
{
	// @todo implement this!
}

void GeoDataPoint::editCoordinates()
{
	// @todo implement this!
}

void GeoDataPoint::editColorSetting()
{
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->showPropertyDialog();
}

void GeoDataPoint::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_setting.load(node);
}

void GeoDataPoint::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_setting.save(writer);
}

void GeoDataPoint::doApplyOffset(double x, double y)
{
	auto p = point();
	p.setX(p.x() + x);
	p.setY(p.y() + y);
	setPoint(p);
}

int GeoDataPoint::iRICLibType() const
{
	return 0;
}

void GeoDataPoint::setColor(const QColor& color)
{
	impl->m_setting.color = color;
	updateActorSettings();
}

GeoDataPoint::MouseEventMode GeoDataPoint::mouseEventMode() const
{
	return impl->m_mouseEventMode;
}

void GeoDataPoint::setMouseEventMode(MouseEventMode mode)
{
	impl->m_mouseEventMode = mode;
}

GeoDataPointColorSimpleSettingDialog::Setting GeoDataPoint::colorSetting() const
{
	return impl->m_setting;
}

void GeoDataPoint::setColorSetting(const GeoDataPointColorSimpleSettingDialog::Setting& setting)
{
	impl->m_setting = setting;
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

void GeoDataPoint::updateActorSettings()
{
	impl->m_pointController.actor()->GetProperty()->SetColor(impl->m_setting.color);
}

void GeoDataPoint::initParams()
{
	impl->m_variantValues.clear();

	int maxIndex = 1;
	auto dims = dimensions();
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

void GeoDataPoint::emitNameAndValueEdited()
{
	emit nameAndValueEdited();
}

QAction* GeoDataPoint::editNameAction() const
{
	return impl->m_editNameAction;
}

QAction* GeoDataPoint::editValueAction() const
{
	return impl->m_editValueAction;
}

QAction* GeoDataPoint::editNameAndValueAction() const
{
	return impl->m_editNameAndValueAction;
}

QAction* GeoDataPoint::coordEditAction() const
{
	return impl->m_coordEditAction;
}
