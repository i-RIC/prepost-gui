#include "geodatarivercrosssection.h"
#include "geodatariverpathpointexpanddialog.h"
#include "geodatariverpathpointextensionadddialog.h"
#include "geodatariverpathpointinsertdialog.h"
#include "geodatariverpathpointmovedialog.h"
#include "geodatariverpathpointrenamedialog.h"
#include "geodatariverpathpointrotatedialog.h"
#include "geodatariverpathpointshiftdialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowprojectdataitem.h"
#include "geodatariversurveydisplaysettingdialog.h"
#include "geodatariversurveyproxy.h"
#include "private/geodatariversurvey_changeselectioncommand.h"
#include "private/geodatariversurvey_deleteriverpathpointcommand.h"
#include "private/geodatariversurvey_impl.h"
#include "private/geodatariversurvey_mousemoveextensioncommand.h"
#include "private/geodatariversurvey_mouserotaterivercrosssectioncommand.h"
#include "private/geodatariversurvey_mouseshiftriverpathcentercommand.h"
#include "private/geodatariversurvey_removeextensioncommand.h"
#include "private/geodatariversurvey_setdisplaysettingcommand.h"
#include "private/geodatariversurvey_translateriverpathpointcommand.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/colorsource.h>
#include <guicore/project/projectdata.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QDomElement>
#include <QFile>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPolygonF>
#include <QSet>
#include <QStandardItem>
#include <QToolBar>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActorCollection.h>
#include <vtkActor2DCollection.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <iriclib_riversurvey.h>

const int GeoDataRiverSurvey::WSE_NAME_MAXLENGTH = 16;

GeoDataRiverSurvey::GeoDataRiverSurvey(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoData(d, creator, att),
	impl {new Impl {this}}
{
	m_headPoint = new GeoDataRiverPathPoint("Dummy", 0, 0);
	connect(impl->m_gridThread, SIGNAL(gridUpdated()), this, SLOT(updateBackgroundGrid()));

	impl->setupCursors();
	impl->setupActions();

	RiverSplineSolver::setLinearMode(false, m_headPoint);
}

GeoDataRiverSurvey::~GeoDataRiverSurvey()
{
	delete impl;
}

void GeoDataRiverSurvey::setCaption(const QString& cap)
{
	GeoData::setCaption(cap);
	updateCrosssectionWindows();
}

void GeoDataRiverSurvey::setupActors()
{
	impl->setupVtkObjects();
}

void GeoDataRiverSurvey::setupMenu()
{
	impl->setupMenu();
}

void GeoDataRiverSurvey::setupDataItem()
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (parent()->parent());
	auto childItems = gItem->childItems();
	int rcount = 1;
	for (GraphicsWindowDataItem* child : childItems) {
		auto i = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (child);
		if (dynamic_cast<GeoDataRiverSurvey*>(i->geoData()) != nullptr) {
			++ rcount;
		}
	}
	auto cs = new ColorSource(this);
	cs->load(":/libs/geodata/riversurvey/data/colorsource_rs.xml");

	m_setting.crosssectionLinesColor = cs->getColor(rcount);
	delete cs;

	GeoData::setupDataItem();
}

bool GeoDataRiverSurvey::addToolBarButtons(QToolBar* /*tb*/)
{
	/*
		 tb->addAction(m_addVertexAction);
		 tb->addAction(m_removeVertexAction);
		 return true;
	 */
	return true;
}

void GeoDataRiverSurvey::informSelection(PreProcessorGraphicsViewInterface*)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	vtkActor2DCollection* col2 = actor2DCollection();
	col->RemoveAllItems();

	col->AddItem(impl->m_riverCenterPointsActor);
	col->AddItem(impl->m_crossSectionLinesActor);
	col->AddItem(impl->m_centerAndBankLinesActor);
	col->AddItem(impl->m_selectedRiverCenterPointsActor);
	col->AddItem(impl->m_selectedLeftBankPointsActor);
	col->AddItem(impl->m_selectedRightBankPointsActor);
	col->AddItem(impl->m_selectedCrossSectionLinesActor);
	if (m_setting.showBackground) {
		col->AddItem(impl->m_backgroundActor);
	}
	if (m_setting.showLines) {
		col->AddItem(impl->m_verticalCrossSectionLinesActor);
	}

	col2->RemoveAllItems();
	col2->AddItem(impl->m_labelActor);

	updateVisibilityWithoutRendering();
}

void GeoDataRiverSurvey::informDeselection(PreProcessorGraphicsViewInterface* /*v*/)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	vtkActor2DCollection* col2 = actor2DCollection();
	col->RemoveAllItems();

	col->AddItem(impl->m_crossSectionLinesActor);
	col->AddItem(impl->m_centerAndBankLinesActor);
	if (m_setting.showBackground) {
		col->AddItem(impl->m_backgroundActor);
	}
	if (m_setting.showLines) {
		col->AddItem(impl->m_verticalCrossSectionLinesActor);
	}
	col2->RemoveAllItems();
	col2->AddItem(impl->m_labelActor);

	updateVisibilityWithoutRendering();
}

void GeoDataRiverSurvey::allActorsOff()
{
	impl->m_riverCenterPointsActor->VisibilityOff();
	impl->m_crossSectionLinesActor->VisibilityOff();
	impl->m_centerAndBankLinesActor->VisibilityOff();
	impl->m_selectedRiverCenterPointsActor->VisibilityOff();
	impl->m_selectedLeftBankPointsActor->VisibilityOff();
	impl->m_selectedRightBankPointsActor->VisibilityOff();
	impl->m_selectedCrossSectionLinesActor->VisibilityOff();
	impl->m_verticalCrossSectionLinesActor->VisibilityOff();
	impl->m_labelActor->VisibilityOff();
	impl->m_backgroundActor->VisibilityOff();
}

void GeoDataRiverSurvey::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GeoDataRiverSurvey::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_editMouseEventMode) {
	case Impl::EditMouseEventMode::Normal:
	case Impl::EditMouseEventMode::TranslatePrepare:
	case Impl::EditMouseEventMode::RotatePrepareRight:
	case Impl::EditMouseEventMode::RotatePrepareLeft:
	case Impl::EditMouseEventMode::ShiftPrepare:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
	case Impl::EditMouseEventMode::ExpansionPrepareRight:
	case Impl::EditMouseEventMode::ExpansionPrepareLeft:
		impl->m_keyboardModifiers = event->modifiers();
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case Impl::EditMouseEventMode::Translate:
	case Impl::EditMouseEventMode::RotateRight:
	case Impl::EditMouseEventMode::RotateLeft:
	case Impl::EditMouseEventMode::Shift:
	case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
	case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
	case Impl::EditMouseEventMode::ExpansionRight:
	case Impl::EditMouseEventMode::ExpansionLeft:
	case Impl::EditMouseEventMode::AddingExtension:
	case Impl::EditMouseEventMode::Inserting:
	case Impl::EditMouseEventMode::TranslateDialog:
	case Impl::EditMouseEventMode::RotateDialog:
	case Impl::EditMouseEventMode::ShiftDialog:
	case Impl::EditMouseEventMode::ExpansionDialog:
		break;
	}
}

void GeoDataRiverSurvey::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_editMouseEventMode) {
	case Impl::EditMouseEventMode::Normal:
	case Impl::EditMouseEventMode::TranslatePrepare:
	case Impl::EditMouseEventMode::RotatePrepareRight:
	case Impl::EditMouseEventMode::RotatePrepareLeft:
	case Impl::EditMouseEventMode::ShiftPrepare:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
	case Impl::EditMouseEventMode::ExpansionPrepareRight:
	case Impl::EditMouseEventMode::ExpansionPrepareLeft:
		impl->m_keyboardModifiers = event->modifiers();
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case Impl::EditMouseEventMode::Translate:
	case Impl::EditMouseEventMode::RotateRight:
	case Impl::EditMouseEventMode::RotateLeft:
	case Impl::EditMouseEventMode::Shift:
	case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
	case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
	case Impl::EditMouseEventMode::ExpansionRight:
	case Impl::EditMouseEventMode::ExpansionLeft:
	case Impl::EditMouseEventMode::AddingExtension:
	case Impl::EditMouseEventMode::Inserting:
	case Impl::EditMouseEventMode::TranslateDialog:
	case Impl::EditMouseEventMode::RotateDialog:
	case Impl::EditMouseEventMode::ShiftDialog:
	case Impl::EditMouseEventMode::ExpansionDialog:
		break;
	}
}

void GeoDataRiverSurvey::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	GeoDataRiverPathPoint* selP = singleSelectedPoint();
	if (selP == nullptr) {
		// no point is selected.
		return;
	}
	openCrossSectionWindow();
}

void GeoDataRiverSurvey::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if ((impl->m_editMouseEventMode == Impl::EditMouseEventMode::AddingExtension || impl->m_editMouseEventMode == Impl::EditMouseEventMode::Inserting) && impl->m_leftButtonDown) {
		graphicsView()->emitWorldPosition(event->x(), event->y());
	} else {
		if (impl->m_definingBoundingBox) {
			// drawing bounding box using mouse dragging.
			MouseBoundingBox* box = dataModel()->mouseBoundingBox();
			box->setEndPoint(event->x(), event->y());
			renderGraphicsView();
		} else {
			switch (impl->m_editMouseEventMode) {
			case Impl::EditMouseEventMode::Normal:
			case Impl::EditMouseEventMode::TranslatePrepare:
			case Impl::EditMouseEventMode::RotatePrepareRight:
			case Impl::EditMouseEventMode::RotatePrepareLeft:
			case Impl::EditMouseEventMode::ShiftPrepare:
			case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
			case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
			case Impl::EditMouseEventMode::ExpansionPrepareRight:
			case Impl::EditMouseEventMode::ExpansionPrepareLeft:
				impl->m_currentPoint = event->pos();
				updateMouseEventMode();
				updateMouseCursor(v);
				break;
			case Impl::EditMouseEventMode::Translate:
				// execute translation.
				iRICUndoStack::instance().push(new TranslateRiverPathPointCommand(impl->m_currentPoint, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::RotateRight:
				iRICUndoStack::instance().push(new MouseRotateRiverCrosssectionCommand(impl->m_currentPoint, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::RotateLeft:
				iRICUndoStack::instance().push(new MouseRotateRiverCrosssectionCommand(impl->m_currentPoint, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::Shift:
				iRICUndoStack::instance().push(new MouseShiftRiverPathCenterCommand(impl->m_currentPoint, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
				iRICUndoStack::instance().push(new MouseMoveExtensionCommand(true, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
				iRICUndoStack::instance().push(new MouseMoveExtensionCommand(false, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::ExpansionRight:
			case Impl::EditMouseEventMode::ExpansionLeft:
				// not used.
				break;
			case Impl::EditMouseEventMode::AddingExtension:
			case Impl::EditMouseEventMode::Inserting:
				if (impl->m_leftButtonDown) {
					graphicsView()->emitWorldPosition(event->x(), event->y());
				}
				break;
			case Impl::EditMouseEventMode::TranslateDialog:
			case Impl::EditMouseEventMode::RotateDialog:
			case Impl::EditMouseEventMode::ShiftDialog:
			case Impl::EditMouseEventMode::ExpansionDialog:
				break;
			}
		}
	}
}

void GeoDataRiverSurvey::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_editMouseEventMode) {
		case Impl::EditMouseEventMode::Normal: {
				// start drawing the mouse bounding box.
				impl->m_definingBoundingBox = true;
				MouseBoundingBox* box = dataModel()->mouseBoundingBox();
				box->setStartPoint(event->x(), event->y());
				box->enable();
				v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
				renderGraphicsView();
			}
			break;
		case Impl::EditMouseEventMode::TranslatePrepare:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::Translate;
			break;
		case Impl::EditMouseEventMode::RotatePrepareRight:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::RotateRight;
			break;
		case Impl::EditMouseEventMode::RotatePrepareLeft:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::RotateLeft;
			break;
		case Impl::EditMouseEventMode::ShiftPrepare:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::Shift;
			break;
		case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::MoveExtentionEndPointLeft;
			break;
		case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::MoveExtentionEndPointRight;
			break;
		case Impl::EditMouseEventMode::ExpansionPrepareRight:
		case Impl::EditMouseEventMode::ExpansionPrepareLeft:
			// do nothing. mouse expansion is not used.
			break;
		case Impl::EditMouseEventMode::Translate:
		case Impl::EditMouseEventMode::RotateRight:
		case Impl::EditMouseEventMode::RotateLeft:
		case Impl::EditMouseEventMode::Shift:
		case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
		case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
		case Impl::EditMouseEventMode::ExpansionRight:
		case Impl::EditMouseEventMode::ExpansionLeft:
			// these does not happen.
			break;
		case Impl::EditMouseEventMode::AddingExtension:
		case Impl::EditMouseEventMode::Inserting:
			impl->m_leftButtonDown = true;
			graphicsView()->emitWorldPosition(event->x(), event->y());
			break;
		case Impl::EditMouseEventMode::TranslateDialog:
		case Impl::EditMouseEventMode::RotateDialog:
		case Impl::EditMouseEventMode::ShiftDialog:
		case Impl::EditMouseEventMode::ExpansionDialog:
			// do nothing.
			break;
		}
	}
	impl->m_dragStartPoint = event->pos();
}

void GeoDataRiverSurvey::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_editMouseEventMode) {
		case Impl::EditMouseEventMode::Normal:
			if (impl->m_definingBoundingBox) {
				// bounding box selecting ended.
				MouseBoundingBox* box = dataModel()->mouseBoundingBox();
				box->setEndPoint(event->x(), event->y());
				box->disable();
				if (iRIC::isNear(box->startPoint(), box->endPoint())) {
					int x = (box->startPoint().x() + box->endPoint().x()) / 2;
					int y = (box->startPoint().y() + box->endPoint().y()) / 2;
					box->setStartPoint(x - 8, y - 8);
					box->setEndPoint(x + 8, y + 8);
				}

				v->restoreUpdateRate();

				// selection change is made not undo-able
				ChangeSelectionCommand com(this, box);
				com.redo();
			}
			impl->m_definingBoundingBox = false;
			impl->m_currentPoint = event->pos();
			updateMouseEventMode();
			updateMouseCursor(v);
			break;
		case Impl::EditMouseEventMode::TranslatePrepare:
		case Impl::EditMouseEventMode::RotatePrepareRight:
		case Impl::EditMouseEventMode::RotatePrepareLeft:
		case Impl::EditMouseEventMode::ShiftPrepare:
		case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
		case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
		case Impl::EditMouseEventMode::ExpansionPrepareRight:
		case Impl::EditMouseEventMode::ExpansionPrepareLeft:
			// do nothing.
			break;
		case Impl::EditMouseEventMode::Translate:
		case Impl::EditMouseEventMode::RotateRight:
		case Impl::EditMouseEventMode::RotateLeft:
		case Impl::EditMouseEventMode::Shift:
		case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
		case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
		case Impl::EditMouseEventMode::ExpansionRight:
		case Impl::EditMouseEventMode::ExpansionLeft:
			// operation ended.
			impl->m_currentPoint = event->pos();
			updateMouseEventMode();
			updateMouseCursor(v);
			break;
		case Impl::EditMouseEventMode::AddingExtension:
		case Impl::EditMouseEventMode::Inserting:
			impl->m_leftButtonDown = false;
			break;
		case Impl::EditMouseEventMode::TranslateDialog:
		case Impl::EditMouseEventMode::RotateDialog:
		case Impl::EditMouseEventMode::ShiftDialog:
		case Impl::EditMouseEventMode::ExpansionDialog:
			// do nothing.
			break;
		}


	} else if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(impl->m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GeoDataRiverSurvey::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_editMouseEventMode) {
	case Impl::EditMouseEventMode::Normal:
	case Impl::EditMouseEventMode::AddingExtension:
		v->setCursor(Qt::ArrowCursor);
		break;
	case Impl::EditMouseEventMode::Translate:
	case Impl::EditMouseEventMode::TranslatePrepare:
		v->setCursor(impl->m_cursorMove);
		break;
	case Impl::EditMouseEventMode::RotateRight:
	case Impl::EditMouseEventMode::RotatePrepareRight:
	case Impl::EditMouseEventMode::RotateLeft:
	case Impl::EditMouseEventMode::RotatePrepareLeft:
		v->setCursor(impl->m_cursorRotate);
		break;
	case Impl::EditMouseEventMode::Shift:
	case Impl::EditMouseEventMode::ShiftPrepare:
		v->setCursor(impl->m_cursorShift);
		break;
	case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
	case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
		v->setCursor(impl->m_cursorMove);
		break;
	case Impl::EditMouseEventMode::ExpansionRight:
	case Impl::EditMouseEventMode::ExpansionPrepareRight:
	case Impl::EditMouseEventMode::ExpansionLeft:
	case Impl::EditMouseEventMode::ExpansionPrepareLeft:
		v->setCursor(impl->m_cursorExpand);
		break;
	default:
		break;
	}
}

void GeoDataRiverSurvey::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
}

void GeoDataRiverSurvey::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);

	m_setting.load(node);

	impl->m_backgroundActor->GetProperty()->SetOpacity(m_setting.opacity);

	QDomElement elem = node.toElement();
	int linearMode = elem.attribute("interpolateLinear").toInt();
	bool lMode = (linearMode != 0);
	RiverSplineSolver::setLinearMode(lMode, m_headPoint);
	if (lMode) {
		impl->m_interpolateSplineAction->setChecked(false);
		impl->m_interpolateLinearAction->setChecked(true);
	}
}

void GeoDataRiverSurvey::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);

	m_setting.save(writer);

	int lMode = 0;
	if (RiverSplineSolver::linearMode()) {
		lMode = 1;
	}
	writer.writeAttribute("interpolateLinear", QString::number(static_cast<int>(lMode)));
}

void GeoDataRiverSurvey::loadExternalData(const QString& filename)
{
	if (projectData()->version().build() >= 3607) {
		iRICLib::RiverSurvey* rs = new iRICLib::RiverSurvey();

		rs->load(iRIC::toStr(filename).c_str());
		GeoDataRiverPathPoint* before = m_headPoint;
		GeoDataRiverPathPoint* newPoint;
		for (unsigned int i = 0; i < rs->points.size(); ++i) {
			iRICLib::RiverPathPoint* libp = rs->points.at(i);
			newPoint = new GeoDataRiverPathPoint();
			newPoint->loadFromiRICLibObject(libp);
			before->addPathPoint(newPoint);
			before = newPoint;
		}
		delete rs;
	} else {
		QFile f(filename);
		f.open(QIODevice::ReadOnly);
		QDataStream s(&f);

		GeoDataRiverPathPoint* before = m_headPoint;
		GeoDataRiverPathPoint* newPoint;
		bool nextExist;
		s >> nextExist;
		while (nextExist) {
			newPoint = new GeoDataRiverPathPoint();
			newPoint->load(s, projectData()->version());
			before->addPathPoint(newPoint);
			before = newPoint;
			s >> nextExist;
		}
		f.close();
	}
	updateInterpolators();
	informDeselection(nullptr);
}

void GeoDataRiverSurvey::saveExternalData(const QString& filename)
{
	iRICLib::RiverSurvey* rs = new iRICLib::RiverSurvey();
	bool first = true;

	iRICLib::RiverPathPoint* prevP = nullptr;
	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	while (p != nullptr) {
		iRICLib::RiverPathPoint* libp = new iRICLib::RiverPathPoint();
		p->saveToiRICLibObject(libp);
		if (first) {
			rs->firstPoint = libp;
			first = false;
		} else {
			prevP->nextPoint = libp;
		}
		prevP = libp;

		p = p->nextPoint();
	}
	rs->save(iRIC::toStr(filename).c_str());

	delete rs;
}

bool GeoDataRiverSurvey::getValueRange(double* min, double* max)
{
	double range[2];
	vtkDataArray* data = impl->m_backgroundGrid->GetPointData()->GetArray("Data");
	if (data == nullptr) {
		return false;
	}
	data->GetRange(range);
	if (range[0] > range[1]) {
		// min > max. It means that valid range can not be obtained from data.
		return false;
	}
	*min = range[0];
	*max = range[1];
	return true;
}

QDialog* GeoDataRiverSurvey::propertyDialog(QWidget*)
{
	return nullptr;
}

void GeoDataRiverSurvey::handlePropertyDialogAccepted(QDialog*)
{}

void GeoDataRiverSurvey::updateShapeData()
{
	impl->updateVtkPointsObjects();
	impl->updateVtkCenterAndBankLinesObjects();
	impl->updateVtkSelectedObjects();
	impl->updateVtkVerticalCenterLinesObjects();
	impl->updateVtkNameLabelObjects();
	impl->updateVtkBackgroundObjects();

	emit dataUpdated();
	impl->m_gridThread->update();
}

void GeoDataRiverSurvey::updateSelectionShapeData()
{
	impl->updateVtkSelectedObjects();
}

GeoDataRiverPathPoint* GeoDataRiverSurvey::headPoint() const
{
	return m_headPoint;
}

vtkStructuredGrid* GeoDataRiverSurvey::backgroundGrid() const
{
	return impl->m_backgroundGrid;
}

void GeoDataRiverSurvey::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(3);
}

void GeoDataRiverSurvey::assignActorZValues(const ZDepthRange& range)
{
	double background = range.min();
	double backlines = .7 * range.min() + .3 * range.max();
	double lines = .5 * range.min() + .5 * range.max();
	double points = range.max();

	impl->m_riverCenterPointsActor->SetPosition(0, 0, points);
	impl->m_crossSectionLinesActor->SetPosition(0, 0, lines);
	impl->m_centerAndBankLinesActor->SetPosition(0, 0, lines);
	impl->m_selectedRiverCenterPointsActor->SetPosition(0, 0, points);
	impl->m_selectedLeftBankPointsActor->SetPosition(0, 0, points);
	impl->m_selectedRightBankPointsActor->SetPosition(0, 0, points);
	impl->m_selectedCrossSectionLinesActor->SetPosition(0, 0, lines);
	impl->m_verticalCrossSectionLinesActor->SetPosition(0, 0, backlines);
	impl->m_blackCrossSectionActor->SetPosition(0, 0, backlines);
	impl->m_backgroundActor->SetPosition(0, 0, background);
}

void GeoDataRiverSurvey::moveSelectedPoints()
{
	GeoDataRiverPathPointMoveDialog* dialog = new GeoDataRiverPathPointMoveDialog(this, preProcessorWindow());
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::TranslateDialog;
	int selectCount = m_headPoint->selectedPoints();
	bool singleSelection = (selectCount == 1);
	GeoDataRiverPathPoint* selected = nullptr;
	if (singleSelection) {
		selected = singleSelectedPoint();
	}
	dialog->setSingleSelection(singleSelection);
	if (singleSelection) {
		dialog->setCurrentCenter(selected->position());
	}
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::deleteSelectedPoints()
{
	int num = 0;
	int selectedNum = 0;
	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	while (p != nullptr) {
		num++;
		if (p->IsSelected) {
			selectedNum++;
		}
		p = p->nextPoint();
	}

	if (num - selectedNum < 2) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("River survey data need at least 2 center points."), QMessageBox::Ok, QMessageBox::Ok);
	} else {
		iRICUndoStack::instance().push(new DeleteRiverPathPointCommand(this));
	}
}

void GeoDataRiverSurvey::shiftSelectedPoints()
{
	GeoDataRiverPathPointShiftDialog* dialog = new GeoDataRiverPathPointShiftDialog(this, preProcessorWindow());
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::ShiftDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::expandSelectedPoints()
{
	GeoDataRiverPathPointExpandDialog* dialog = new GeoDataRiverPathPointExpandDialog(this, preProcessorWindow());
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::ExpansionDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::rotateSelectedPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	QPointF dir = selected->previousPoint()->position() - selected->position();
	if (selected->previousPoint()->firstPoint() && selected->nextPoint() != nullptr) {
		dir = selected->position() - selected->nextPoint()->position();
	}
	double angle = iRIC::angle(dir, selected->crosssectionDirection());

	GeoDataRiverPathPointRotateDialog* dialog = new GeoDataRiverPathPointRotateDialog(this, preProcessorWindow());
	dialog->setCurrentRelativeAngle(angle);
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::RotateDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::renameSelectedPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointRenameDialog dialog(selected, this, preProcessorWindow());
	dialog.exec();
}

void GeoDataRiverSurvey::addLeftExtensionPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointExtensionAddDialog* dialog = new GeoDataRiverPathPointExtensionAddDialog(selected, this, preProcessorWindow());
	dialog->setLineMode(GeoDataRiverPathPointExtensionAddDialog::Left);
	dialog->setPoint(selected->crosssectionPosition(selected->crosssection().leftBank(true).position()));
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::AddingExtension;
	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::addRightExtensionPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointExtensionAddDialog* dialog = new GeoDataRiverPathPointExtensionAddDialog(selected, this, preProcessorWindow());
	dialog->setLineMode(GeoDataRiverPathPointExtensionAddDialog::Right);
	dialog->setPoint(selected->crosssectionPosition(selected->crosssection().rightBank(true).position()));
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::AddingExtension;
	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::removeLeftExtensionPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	iRICUndoStack::instance().push(new RemoveExtensionCommand(true, selected->leftBank()->interpolate(0), selected, this));
}

void GeoDataRiverSurvey::removeRightExtensionPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	iRICUndoStack::instance().push(new RemoveExtensionCommand(false, selected->rightBank()->interpolate(0), selected, this));
}

void GeoDataRiverSurvey::restoreMouseEventMode()
{
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::Normal;
}

void GeoDataRiverSurvey::insertNewPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointInsertDialog* dialog = new GeoDataRiverPathPointInsertDialog(selected, true, this, preProcessorWindow());
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::Inserting;

	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::addNewPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointInsertDialog* dialog = new GeoDataRiverPathPointInsertDialog(selected, false, this, preProcessorWindow());
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::Inserting;

	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::updateMouseEventMode()
{
	double dx, dy;
	dx = impl->m_currentPoint.x();
	dy = impl->m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF worldPos(dx, dy);
	double stdLen = graphicsView()->stdRadius(1);
	double stdLen2 = stdLen * stdLen;
	int selectCount = m_headPoint->selectedPoints();
	GeoDataRiverPathPoint* selected = nullptr;
	if (selectCount == 1) {
		selected = m_headPoint->nextPoint();
		while (1) {
			if (selected->IsSelected) {break;}
			selected = selected->nextPoint();
		}
		// only one point is selected.
		if (iRIC::lengthSquared(selected->position() - worldPos) < stdLen2 * 9) {
			// cursor is near to the river center point
			if ((impl->m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
					(impl->m_keyboardModifiers & Qt::ControlModifier) == 0) {
				// preparing for moving
				impl->m_editMouseEventMode = Impl::EditMouseEventMode::TranslatePrepare;
			}
			if ((impl->m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
					(impl->m_keyboardModifiers & Qt::ControlModifier) == 0) {
				// preparing for center-point shift
				impl->m_editMouseEventMode = Impl::EditMouseEventMode::ShiftPrepare;
			}
		} else {
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::Normal;
			QPointF lbank = selected->crosssectionPosition(selected->crosssection().leftBank(true).position());
			if (iRIC::lengthSquared(lbank - worldPos) < stdLen2 * 9) {
				// cursor is near left bank.
				if ((impl->m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
						(impl->m_keyboardModifiers & Qt::ControlModifier) == 0) {
					// preparing for rotating
					impl->m_editMouseEventMode = Impl::EditMouseEventMode::RotatePrepareLeft;
				}
				if ((impl->m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
						(impl->m_keyboardModifiers & Qt::ControlModifier) == 0 &&
						selected->crosssection().fixedPointLSet()) {
					// preparing for center-point shift
					impl->m_editMouseEventMode = Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft;
				}
			}
			QPointF rbank = selected->crosssectionPosition(selected->crosssection().rightBank(true).position());
			if (iRIC::lengthSquared(rbank - worldPos) < stdLen2 * 9) {
				// cursor is near right bank.
				if ((impl->m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
						(impl->m_keyboardModifiers & Qt::ControlModifier) == 0) {
					// preparing for rotating
					impl->m_editMouseEventMode = Impl::EditMouseEventMode::RotatePrepareRight;
				}
				if ((impl->m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
						(impl->m_keyboardModifiers & Qt::ControlModifier) == 0 &&
						selected->crosssection().fixedPointRSet()) {
					// preparing for center-point shift
					impl->m_editMouseEventMode = Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight;
				}
			}
		}
	} else {
		// multiple selection, or none.
		GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
		impl->m_editMouseEventMode = Impl::EditMouseEventMode::Normal;
		while (p != nullptr) {
			if (p->IsSelected) {
				if (iRIC::lengthSquared(p->position() - worldPos) < stdLen2 * 9) {
					// cursor is near to the river center point
					if ((impl->m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
							(impl->m_keyboardModifiers & Qt::ControlModifier) == 0) {
						// preparing for moving
						impl->m_editMouseEventMode = Impl::EditMouseEventMode::TranslatePrepare;
						return;
					}
					if ((impl->m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
							(impl->m_keyboardModifiers & Qt::ControlModifier) == 0) {
						// preparing for center-point shift
						impl->m_editMouseEventMode = Impl::EditMouseEventMode::ShiftPrepare;
						return;
					}
				} else {
					impl->m_editMouseEventMode = Impl::EditMouseEventMode::Normal;
					QPointF lbank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
					if (iRIC::lengthSquared(lbank - worldPos) < stdLen2 * 9) {
						// cursor is near left bank.
						if ((impl->m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
								(impl->m_keyboardModifiers & Qt::ControlModifier) == 0) {
							// preparing for center-point shift
							impl->m_editMouseEventMode = Impl::EditMouseEventMode::ExpansionPrepareLeft;
							return;
						}
					}
					QPointF rbank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
					if (iRIC::lengthSquared(rbank - worldPos) < stdLen2 * 9) {
						// cursor is near right bank.
						if ((impl->m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
								(impl->m_keyboardModifiers & Qt::ControlModifier) == 0) {
							// preparing for center-point shift
							impl->m_editMouseEventMode = Impl::EditMouseEventMode::ExpansionPrepareRight;
							return;
						}
					}
				}
			}
			p = p->nextPoint();
		}
	}
}


GeoDataRiverPathPoint* GeoDataRiverSurvey::singleSelectedPoint()
{
	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {return p;}
		p = p->nextPoint();
	}
	return nullptr;
}

void GeoDataRiverSurvey::updateSplineSolvers()
{
	m_CenterLineSolver.update();
	m_LeftBankSolver.update();
	m_RightBankSolver.update();
}

void GeoDataRiverSurvey::updateBackgroundGrid()
{
	impl->m_backgroundGrid = impl->m_gridThread->grid();
	impl->m_backgroundGrid->GetPointData()->SetActiveScalars("Data");

	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	impl->m_gridThread->startBGGridCopy();
	while (p != nullptr) {
		vtkPointSet* grid = impl->m_gridThread->partialGrid(p);
		if (grid != nullptr) {
			p->areaGrid()->DeepCopy(grid);
		}
		p = p->nextPoint();
	}
	impl->m_gridThread->finishBGGridCopy();
	vtkDataSetMapper* mapper = vtkDataSetMapper::SafeDownCast(impl->m_backgroundActor->GetMapper());
	mapper->SetInputData(impl->m_backgroundGrid);
	if (isVisible() && m_setting.showBackground) {
		impl->m_backgroundActor->VisibilityOn();
	}
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->informValueRangeChange();
}

void GeoDataRiverSurvey::updateInterpolators()
{
	// update interpolators
	m_CenterLineSolver.setHeadPoint(m_headPoint);
	m_CenterLineSolver.update();
	m_LeftBankSolver.setHeadPoint(m_headPoint);
	m_LeftBankSolver.update();
	m_RightBankSolver.setHeadPoint(m_headPoint);
	m_RightBankSolver.update();
	m_headPoint->UpdateCtrlSections();

	// update interpolators for altitude interpolation on crosssections.
	GeoDataRiverPathPoint* tmpp = m_headPoint->nextPoint();
	while (tmpp != nullptr) {
		tmpp->updateXSecInterpolators();
		tmpp = tmpp->nextPoint();
	}
	updateShapeData();
}

void GeoDataRiverSurvey::openCrossSectionWindow()
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	gItem->openCrossSectionWindow(this, singleSelectedPoint()->name());
}

void GeoDataRiverSurvey::updateCrosssectionWindows()
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	gItem->updateCrossectionWindows();
}

void GeoDataRiverSurvey::toggleCrosssectionWindowsGridCreatingMode(bool gridMode)
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	gItem->toggleCrosssectionWindowsGridCreatingMode(gridMode, this);
}

void GeoDataRiverSurvey::informCtrlPointUpdateToCrosssectionWindows()
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	gItem->informCtrlPointUpdateToCrosssectionWindows();
}

void GeoDataRiverSurvey::displaySetting()
{
	GeoDataRiverSurveyDisplaySettingDialog dialog(preProcessorWindow());
	dialog.setSetting(m_setting);

	int ret = dialog.exec();
	if (ret != dialog.Accepted) {return;}

	pushRenderCommand(new SetDisplaySettingCommand(dialog.setting(), this));
}

void GeoDataRiverSurvey::switchInterpolateModeToLinear()
{
	RiverSplineSolver::setLinearMode(true, m_headPoint);
	impl->m_interpolateLinearAction->setChecked(true);
	impl->m_interpolateSplineAction->setChecked(false);
	updateInterpolators();
}

void GeoDataRiverSurvey::switchInterpolateModeToSpline()
{
	RiverSplineSolver::setLinearMode(false, m_headPoint);
	impl->m_interpolateLinearAction->setChecked(false);
	impl->m_interpolateSplineAction->setChecked(true);
	updateInterpolators();
}

void GeoDataRiverSurvey::setColoredPoints(GeoDataRiverPathPoint* black)
{
	if (black == nullptr) {
		impl->m_blackCrossSectionActor->VisibilityOff();
	} else {
		setupLine(impl->m_blackCrossSection, black);
		impl->m_blackCrossSectionActor->VisibilityOn();
	}
	renderGraphicsView();
}

void GeoDataRiverSurvey::setGridCreatingCondition(GridCreatingConditionRiverSurveyInterface* cond)
{
	impl->m_gridCreatingCondition = cond;
}

GridCreatingConditionRiverSurveyInterface* GeoDataRiverSurvey::gridCreatingCondition() const
{
	return impl->m_gridCreatingCondition;
}

void GeoDataRiverSurvey::setupLine(vtkPolyData* polyData, GeoDataRiverPathPoint* p)
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto cells = vtkSmartPointer<vtkCellArray>::New();

	// left bank
	auto point = p->crosssectionPosition(p->crosssection().leftBank(true).position());
	points->InsertNextPoint(point.x(), point.y(), 0);

	// left fixed point
	if (p->crosssection().fixedPointLSet()) {
		point = p->crosssectionPosition(p->crosssection().fixedPointL().position());
	} else {
		// use left bank.
		point = p->crosssectionPosition(p->crosssection().leftBank(true).position());
	}
	points->InsertNextPoint(point.x(), point.y(), 0);

	// river center
	point = p->position();
	points->InsertNextPoint(point.x(), point.y(), 0);

	// right fixed point
	if (p->crosssection().fixedPointRSet()) {
		point = p->crosssectionPosition(p->crosssection().fixedPointR().position());
	} else {
		// use right bank.
		point = p->crosssectionPosition(p->crosssection().rightBank(true).position());
	}
	points->InsertNextPoint(point.x(), point.y(), 0);

	// right bank
	point = p->crosssectionPosition(p->crosssection().rightBank(true).position());
	points->InsertNextPoint(point.x(), point.y(), 0);

	vtkIdType ids[5];
	for (vtkIdType i = 0; i < 5; ++i) {
		ids[i] = i;
	}
	cells->InsertNextCell(5, ids);

	polyData->Initialize();
	polyData->SetPoints(points);
	polyData->SetLines(cells);
}

void GeoDataRiverSurvey::useDivisionPointsForBackgroundGrid(bool use)
{
	impl->m_gridThread->setUseDivisionPoints(use);
	impl->m_gridThread->update();
}

void GeoDataRiverSurvey::refreshBackgroundGrid()
{
	impl->m_gridThread->update();
}

void GeoDataRiverSurvey::cancelBackgroundGridUpdate()
{
	impl->m_gridThread->cancel();
}

GeoDataProxy* GeoDataRiverSurvey::getProxy()
{
	return new GeoDataRiverSurveyProxy(this);
}

void GeoDataRiverSurvey::updateFilename()
{
	setFilename(name().append(".dat"));
}

int GeoDataRiverSurvey::iRICLibType() const
{
	return IRIC_GEO_RIVERSURVEY;
}

void GeoDataRiverSurvey::doApplyOffset(double x, double y)
{
	GeoDataRiverPathPoint* p = this->m_headPoint->nextPoint();
	while (p != nullptr) {
		QPointF v = p->position();
		v.setX(v.x() - x);
		v.setY(v.y() - y);
		p->setPosition(v);
		p = p->nextPoint();
	}
	this->headPoint()->updateRiverShapeInterpolators();
	this->updateShapeData();
}
