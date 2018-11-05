#include "geodatapolyline.h"
#include "geodatapolylinecolorsettingdialog.h"
#include "geodatapolylineproxy.h"
#include "geodatapolylineimplpolyline.h"

#include "private/geodatapolyline_addvertexcommand.h"
#include "private/geodatapolyline_coordinateseditor.h"
#include "private/geodatapolyline_editpropertycommand.h"
#include "private/geodatapolyline_editvaluecommand.h"
#include "private/geodatapolyline_finishpolylinedefinitioncommand.h"
#include "private/geodatapolyline_impl.h"
#include "private/geodatapolyline_movepolylinecommand.h"
#include "private/geodatapolyline_movevertexcommand.h"
#include "private/geodatapolyline_pushnewpointcommand.h"
#include "private/geodatapolyline_removevertexcommand.h"

#include <iriclib_polyline.h>

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
#include <misc/zdepthrange.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>

#include <QAction>
#include <QFile>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPointF>
#include <QStandardItem>
#include <QToolBar>

#include <vector>

GeoDataPolyLine::Impl::Impl(GeoDataPolyLine* parent) :
	m_parent {parent},
	m_polyLine {new GeoDataPolyLineImplPolyLine(m_parent)},
	m_inhibitSelect {false},
	m_rightClickingMenu {new QMenu()},
	m_editValueAction {new QAction(GeoDataPolyLine::tr("Edit &Value..."), m_parent)},
	m_addVertexAction {new QAction(QIcon(":/libs/guibase/images/iconAddPolyLineVertex.png"), GeoDataPolyLine::tr("&Add Vertex"), m_parent)},
	m_removeVertexAction {new QAction(QIcon(":/libs/guibase/images/iconRemovePolyLineVertex.png"), GeoDataPolyLine::tr("&Remove Vertex"), m_parent)},
	m_coordEditAction {new QAction(GeoDataPolyLine::tr("Edit &Coordinates..."), m_parent)},
	m_editColorSettingAction {new QAction(GeoDataPolyLine::tr("Color &Setting..."), m_parent)},
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_removePixmap {":/libs/guibase/images/cursorRemove.png"},
	m_movePointPixmap {":/libs/guibase/images/cursorOpenHandPoint.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_removeCursor {m_removePixmap, 0, 0},
	m_movePointCursor {m_movePointPixmap}
{
	m_addVertexAction->setCheckable(true);
	m_removeVertexAction->setCheckable(true);

	QObject::connect(m_editValueAction, SIGNAL(triggered()), m_parent, SLOT(editValue()));
	QObject::connect(m_addVertexAction, SIGNAL(triggered(bool)), m_parent, SLOT(addVertexMode(bool)));
	QObject::connect(m_removeVertexAction, SIGNAL(triggered(bool)), m_parent, SLOT(removeVertexMode(bool)));
	QObject::connect(m_coordEditAction, SIGNAL(triggered()), m_parent, SLOT(editCoordinates()));
	QObject::connect(m_editColorSettingAction, SIGNAL(triggered()), m_parent, SLOT(editColorSetting()));
}

GeoDataPolyLine::Impl::~Impl()
{
	delete m_rightClickingMenu;
	delete m_polyLine;
}

GeoDataPolyLine::GeoDataPolyLine(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData(d, creator, condition),
	impl {new Impl {this}}
{
	initParams();

	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		impl->m_polyLine->setLookupTable(stcc->vtkDarkObj());
	}

	impl->m_mouseEventMode = meBeforeDefining;

	updateActorSettings();
	updateActionStatus();
}

GeoDataPolyLine::~GeoDataPolyLine()
{
	delete impl;
}

void GeoDataPolyLine::setupMenu()
{
	m_menu->setTitle(tr("Poly&line"));
	m_menu->addAction(m_editNameAction);
//	m_menu->addAction(impl->m_editValueAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_addVertexAction);
	m_menu->addAction(impl->m_removeVertexAction);
	m_menu->addAction(impl->m_coordEditAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_editColorSettingAction);
	m_menu->addSeparator();
	m_menu->addAction(deleteAction());

//	impl->m_rightClickingMenu->addAction(impl->m_editValueAction);
//	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_addVertexAction);
	impl->m_rightClickingMenu->addAction(impl->m_removeVertexAction);
	impl->m_rightClickingMenu->addAction(impl->m_coordEditAction);
	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_editColorSettingAction);
}

bool GeoDataPolyLine::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(impl->m_addVertexAction);
	tb->addAction(impl->m_removeVertexAction);
	return true;
}

QColor GeoDataPolyLine::doubleToColor(double /*d*/)
{
	return Qt::red;
}

void GeoDataPolyLine::setMouseEventMode(MouseEventMode mode)
{
	impl->m_mouseEventMode = mode;
}

GeoDataPolyLineColorSimpleSettingDialog::Setting GeoDataPolyLine::colorSetting() const
{
	return impl->m_setting;
}

void GeoDataPolyLine::setColorSetting(GeoDataPolyLineColorSimpleSettingDialog::Setting setting)
{
	impl->m_setting = setting;
}

void GeoDataPolyLine::informSelection(PreProcessorGraphicsViewInterface* v)
{
	impl->m_polyLine->setActive(true);

	updateMouseCursor(v);
}

void GeoDataPolyLine::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	impl->m_polyLine->setActive(false);

	v->unsetCursor();
}

void GeoDataPolyLine::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GeoDataPolyLine::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (! iRIC::isEnterKey(event->key())) {return;}
	if (impl->m_mouseEventMode != meDefining) {return;}

	definePolyLine(false);
}

void GeoDataPolyLine::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GeoDataPolyLine::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (impl->m_mouseEventMode == meDefining) {
		definePolyLine(true);
	}
}


void GeoDataPolyLine::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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
		pushRenderCommand(new PushNewPointCommand(false, event->pos(), this));
		break;
	case meTranslate:
		// execute translation.
		pushRenderCommand(new MovePolyLineCommand(false, impl->m_currentPoint, event->pos(), this));
		impl->m_currentPoint = event->pos();
		break;
	case meMoveVertex:
		pushRenderCommand(new MoveVertexCommand(false, impl->m_currentPoint, event->pos(), impl->m_polyLine->selectedVertexId(), this));
		impl->m_currentPoint = event->pos();
		break;
	case meAddVertex:
		pushRenderCommand(new AddVertexCommand(false, impl->m_polyLine->selectedEdgeId(), event->pos(), this));
		break;
	case meTranslateDialog:
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GeoDataPolyLine::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {

		// left click
		double worldX = static_cast<double>(event->x());
		double worldY = static_cast<double>(event->y());
		v->viewportToWorld(worldX, worldY);

		switch (impl->m_mouseEventMode) {
		case meNormal:
			break;
		case meBeforeDefining:
			// enter defining mode.
			impl->m_mouseEventMode = meDefining;
			pushRenderCommand(new PushNewPointCommand(true, event->pos(), this));
			break;
		case meDefining:
			pushRenderCommand(new PushNewPointCommand(true, event->pos(), this));
			break;
		case meTranslatePrepare:
			// start translating
			impl->m_mouseEventMode = meTranslate;
			impl->m_currentPoint = event->pos();
			updateMouseCursor(v);
			// push the first translation command.
			pushRenderCommand(new MovePolyLineCommand(true, impl->m_currentPoint, impl->m_currentPoint, this));
			break;
		case meMoveVertexPrepare:
			impl->m_mouseEventMode = meMoveVertex;
			impl->m_currentPoint = event->pos();
			// push the first move command.
			pushRenderCommand(new MoveVertexCommand(true, impl->m_currentPoint, impl->m_currentPoint, impl->m_polyLine->selectedVertexId(), this));
			break;
		case meAddVertexPrepare:
			impl->m_mouseEventMode = meAddVertex;
			pushRenderCommand(new AddVertexCommand(true, impl->m_polyLine->selectedEdgeId(), event->pos(), this));
			break;
		case meAddVertexNotPossible:
			// do nothing.
			break;
		case meRemoveVertexPrepare:
			if (impl->m_polyLine->polyLine().size() == 1) {
				// you are going to remove the last point. you can not do this.
			} else {
				impl->m_mouseEventMode = meNormal;
				pushRenderCommand(new RemoveVertexCommand(impl->m_polyLine->selectedVertexId(), this));
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

void GeoDataPolyLine::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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

void GeoDataPolyLine::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
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

void GeoDataPolyLine::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
//	menu->addAction(impl->m_editValueAction);
}

void GeoDataPolyLine::definePolyLine(bool doubleClick, bool noEditVal)
{
	int minCount = 2;
	if (impl->m_polyLine->polyLine().size() <= minCount) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Polyline must have two vertices at least."));
		return;
	}
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.undo();
	stack.beginMacro(tr("Finish Defining Polyline"));
	// finish defining the polyline.
	stack.push(new FinishPolyLineDefiningCommand(this));
	stack.endMacro();
	if (! noEditVal) {
//		editValue();
	}
}

QColor GeoDataPolyLine::color() const
{
	return impl->m_setting.color;
}

void GeoDataPolyLine::addVertexMode(bool on)
{
	if (on) {
		impl->m_mouseEventMode = meAddVertexNotPossible;
	} else {
		impl->m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPolyLine::removeVertexMode(bool on)
{
	if (on) {
		impl->m_mouseEventMode = meRemoveVertexNotPossible;
	} else {
		impl->m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPolyLine::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_setting.load(node);
}

void GeoDataPolyLine::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_setting.save(writer);
}

void GeoDataPolyLine::loadExternalData(const QString& filename)
{
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		impl->m_polyLine->setLookupTable(stcc->vtkDarkObj());
	}
	iRICLib::Polyline* line = new iRICLib::Polyline();
	GridAttributeDimensionsContainer* dims = dimensions();
	bool noDim = true;
	if (dims != nullptr) {
		noDim = dims->containers().size() == 0;
	}

	line->load(iRIC::toStr(filename).c_str(), noDim);
	impl->m_variantValues.clear();
	for (unsigned int i = 0; i < line->values.size(); ++i) {
		impl->m_variantValues.push_back(line->values[i]);
	}
	std::vector<QPointF> qline;
	iRICLib::InternalPolyline* polyLine = line->polyline;
	for (int i = 0; i < polyLine->pointCount; ++i) {
		qline.push_back(QPointF(*(polyLine->x + i), *(polyLine->y + i)));
	}
	impl->m_polyLine->setPolyLine(qline);

	delete line;

	if (impl->m_polyLine->polyLine().size() > 0) {
		impl->m_mouseEventMode = meNormal;
		informDeselection(graphicsView());
	} else {
		impl->m_mouseEventMode = meBeforeDefining;
	}
	updateActorSettings();
	updateActionStatus();
}

void GeoDataPolyLine::saveExternalData(const QString& filename)
{
	iRICLib::Polyline line;
	line.values.clear();
	for (int i = 0; i < impl->m_variantValues.size(); ++i) {
		line.values.push_back(impl->m_variantValues.at(i).toDouble());
	}
	iRICLib::InternalPolyline* polyLine = new iRICLib::InternalPolyline();
	auto qline = impl->m_polyLine->polyLine();
	polyLine->pointCount = qline.size();
	polyLine->x = new double[polyLine->pointCount];
	polyLine->y = new double[polyLine->pointCount];
	for (int i = 0; i < polyLine->pointCount; ++i) {
		*(polyLine->x + i) = qline.at(i).x();
		*(polyLine->y + i) = qline.at(i).y();
	}
	line.polyline = polyLine;
	GridAttributeDimensionsContainer* dims = dimensions();
	bool noDim = true;
	if (dims != nullptr) {
		noDim = dims->containers().size() == 0;
	}
	line.save(iRIC::toStr(filename).c_str(), noDim);
}

void GeoDataPolyLine::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GeoDataPolyLine::assignActorZValues(const ZDepthRange& range)
{
	impl->m_depthRange = range;
	impl->m_polyLine->setZDepthRange(range.min(), range.max());
}

void GeoDataPolyLine::updateMouseEventMode()
{
	double dx, dy;
	dx = impl->m_currentPoint.x();
	dy = impl->m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF worldPos(dx, dy);
	switch (impl->m_mouseEventMode) {
	case meAddVertexNotPossible:
	case meAddVertexPrepare:
		if (impl->m_polyLine->isEdgeSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
			impl->m_mouseEventMode = meAddVertexPrepare;
		} else {
			impl->m_mouseEventMode = meAddVertexNotPossible;
		}
		break;
	case meRemoveVertexNotPossible:
	case meRemoveVertexPrepare:
		if (impl->m_polyLine->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
			impl->m_mouseEventMode = meRemoveVertexPrepare;
		} else {
			impl->m_mouseEventMode = meRemoveVertexNotPossible;
		}
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		if (impl->m_polyLine->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
			impl->m_mouseEventMode = meMoveVertexPrepare;
		}
		else if (impl->m_polyLine->isEdgeSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
			impl->m_mouseEventMode = meTranslatePrepare;
		} else {
			impl->m_mouseEventMode = meNormal;
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

void GeoDataPolyLine::updateActionStatus()
{
	switch (impl->m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
		impl->m_addVertexAction->setDisabled(true);
		impl->m_addVertexAction->setChecked(false);
		impl->m_removeVertexAction->setDisabled(true);
		impl->m_removeVertexAction->setChecked(false);
		impl->m_coordEditAction->setEnabled(false);

		break;
	case meTranslate:
	case meMoveVertex:
		impl->m_addVertexAction->setDisabled(true);
		impl->m_addVertexAction->setChecked(false);
		impl->m_removeVertexAction->setDisabled(true);
		impl->m_removeVertexAction->setChecked(false);
		impl->m_coordEditAction->setDisabled(true);

		break;

		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		impl->m_addVertexAction->setChecked(false);
		impl->m_removeVertexAction->setChecked(false);

		impl->m_addVertexAction->setEnabled(true);
		impl->m_removeVertexAction->setEnabled(polylineHasThreeVertices());
		impl->m_coordEditAction->setEnabled(true);

		impl->m_addVertexAction->setEnabled(true);
		impl->m_removeVertexAction->setEnabled(polylineHasThreeVertices());
		impl->m_coordEditAction->setEnabled(true);
		break;
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meAddVertex:
		impl->m_addVertexAction->setChecked(true);
		impl->m_removeVertexAction->setChecked(false);

		impl->m_addVertexAction->setEnabled(true);
		impl->m_removeVertexAction->setEnabled(polylineHasThreeVertices());
		impl->m_coordEditAction->setEnabled(true);
		break;
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		impl->m_addVertexAction->setEnabled(true);
		impl->m_addVertexAction->setChecked(false);
		impl->m_removeVertexAction->setEnabled(true);
		impl->m_removeVertexAction->setChecked(true);
		impl->m_coordEditAction->setEnabled(false);
		break;
	case meTranslateDialog:
	case meEditVerticesDialog:
		break;
	}
}

void GeoDataPolyLine::editCoordinates()
{
	CoordinatesEditor::edit(this);
}

void GeoDataPolyLine::restoreMouseEventMode()
{
	impl->m_mouseEventMode = meNormal;
}

void GeoDataPolyLine::clear()
{
	initParams();
	delete impl->m_polyLine;

	impl->m_polyLine = new GeoDataPolyLineImplPolyLine(this);
	impl->m_polyLine->setZDepthRange(impl->m_depthRange.min(), impl->m_depthRange.max());
	impl->m_mouseEventMode = meBeforeDefining;
	updateMouseCursor(graphicsView());
	updateActionStatus();
	renderGraphicsView();
}

bool GeoDataPolyLine::ready() const
{
	return true;
}

void GeoDataPolyLine::initParams()
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

bool GeoDataPolyLine::polylineHasThreeVertices()
{
	if (impl->m_polyLine == nullptr) {return false;}
	auto pol = impl->m_polyLine->polyLine();
	return pol.size() >= 3;
}

void GeoDataPolyLine::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GeoDataPolyLine::tr("Information"), GeoDataPolyLine::tr("Please define polyline by mouse-clicking. Finish definining by double clicking, or pressing return key."), "geodatapolylineinit");
}

const QVariant& GeoDataPolyLine::variantValue() const
{
	int index = 0;
	GridAttributeDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	return impl->m_variantValues.at(index);
}

void GeoDataPolyLine::setVariantValue(const QVariant &v, bool disableInform)
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

void GeoDataPolyLine::editValue()
{
	GridAttributeEditDialog* dialog = m_gridAttribute->editDialog(preProcessorWindow());
	PreProcessorGeoDataGroupDataItemInterface* i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setLabel(tr("Please input new value in this polyline.").arg(i->condition()->caption()));
	i->setupEditWidget(dialog->widget());
	dialog->setVariantValue(variantValue());
	if (impl->m_mouseEventMode == meDefining || impl->m_mouseEventMode == meBeforeDefining) {
		dialog->disableCancel();
	}
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {return;}
	pushCommand(new EditValueCommand(dialog->variantValue(), this));
}

void GeoDataPolyLine::updateScalarValues()
{
	impl->m_polyLine->updateScalarValues();
}

void GeoDataPolyLine::editColorSetting()
{
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->showPropertyDialog();
}

void GeoDataPolyLine::updateActorSettings()
{
	impl->m_polyLine->setColor(impl->m_setting.color);
//	impl->m_polyLine->setMapping(impl->m_setting.mapping);
	impl->m_polyLine->setMapping(GeoDataPolyLineColorSettingDialog::Arbitrary);
}

QDialog* GeoDataPolyLine::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPolyLineColorSimpleSettingDialog(parent);
	dialog->setSetting(impl->m_setting);

	return dialog;
}

void GeoDataPolyLine::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<GeoDataPolyLineColorSimpleSettingDialog*>(propDialog);
	pushRenderCommand(new EditPropertyCommand(dialog->setting(), this));
}

bool GeoDataPolyLine::getValueRange(double* min, double* max)
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

void GeoDataPolyLine::updateFilename()
{
	setFilename(name().append(".dat"));
}

GeoDataProxy* GeoDataPolyLine::getProxy()
{
	return new GeoDataPolyLineProxy(this);
}

void GeoDataPolyLine::copyShape(GeoDataPolyLine* polyline)
{
	impl->m_polyLine->setPolyLine(polyline->polyLine()->polyLine());
	impl->m_mouseEventMode = meNormal;
	editValue();
	// copy command is not undo-able.
	iRICUndoStack::instance().clear();
}

void GeoDataPolyLine::doApplyOffset(double x, double y)
{
	std::vector<QPointF> pol = impl->m_polyLine->polyLine(QPointF(x, y));
	impl->m_polyLine->setPolyLine(pol);
}

int GeoDataPolyLine::iRICLibType() const
{
	return IRIC_GEO_POLYLINE;
}

void GeoDataPolyLine::handleDimensionCurrentIndexChange(int /*oldIndex*/, int /*newIndex*/)
{
	// @todo implement this!
}

void GeoDataPolyLine::handleDimensionValuesChange(const std::vector<QVariant>& /*before*/, const std::vector<QVariant>& /*after*/)
{
	// @todo implement this!
}

geos::geom::LineString* GeoDataPolyLine::getGeosLineString(const QPointF& offset)
{
	// @TODO Implement this
	return nullptr;
/*
	setupTriangleThread();
	return impl->m_triangleThread->getGeosPolyLine(this, offset);
*/
}

GeoDataPolyLineImplPolyLine* GeoDataPolyLine::polyLine() const
{
	return impl->m_polyLine;
}
