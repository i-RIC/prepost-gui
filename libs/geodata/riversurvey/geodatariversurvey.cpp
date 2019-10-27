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
#include "private/geodatariversurvey_mousemoveextensioncommand.h"
#include "private/geodatariversurvey_mouserotaterivercrosssectioncommand.h"
#include "private/geodatariversurvey_mouseshiftriverpathcentercommand.h"
#include "private/geodatariversurvey_removeextensioncommand.h"
#include "private/geodatariversurvey_setdisplaysettingcommand.h"
#include "private/geodatariversurvey_translateriverpathpointcommand.h"

#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/project/colorsource.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <misc/zdepthrange.h>

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

#include <vtkActor2DCollection.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkIdList.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkTextProperty.h>
#include <vtkVertex.h>

#include <iriclib_riversurvey.h>

const int GeoDataRiverSurvey::WSE_NAME_MAXLENGTH = 16;

GeoDataRiverSurvey::GeoDataRiverSurvey(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoData(d, creator, att),
	impl {new Impl {this}}
{
	m_headPoint = new GeoDataRiverPathPoint("Dummy", 0, 0);

	m_points = vtkSmartPointer<vtkPoints>::New();
	m_points->SetDataTypeToDouble();
	m_rightBankPoints = vtkSmartPointer<vtkPoints>::New();
	m_rightBankPoints->SetDataTypeToDouble();
	// setup grid.
	m_rightBankPointSet = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_firstAndLastCrosssections = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_crosssections = vtkSmartPointer<vtkUnstructuredGrid>::New();

	m_leftBankLine = vtkSmartPointer<vtkStructuredGrid>::New();
	m_rightBankLine = vtkSmartPointer<vtkStructuredGrid>::New();
	m_riverCenterLine = vtkSmartPointer<vtkStructuredGrid>::New();

	m_backgroundGrid = vtkSmartPointer<vtkStructuredGrid>::New();

	m_crosssectionLines = vtkSmartPointer<vtkUnstructuredGrid>::New();

	m_labelArray = vtkSmartPointer<vtkStringArray>::New();
	m_labelArray->SetName("Label");

	m_definingBoundingBox = false;
	m_leftButtonDown = false;

	m_gridCreatingCondition = nullptr;

	m_gridThread = new GeoDataRiverSurveyBackgroundGridCreateThread(this);
	connect(m_gridThread, SIGNAL(gridUpdated()), this, SLOT(updateBackgroundGrid()));

	impl->setupCursors();
	impl->setupActions();

	RiverSplineSolver::setLinearMode(false, m_headPoint);
}

GeoDataRiverSurvey::~GeoDataRiverSurvey()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_crossectionsActor);
	r->RemoveActor(m_firstAndLastCrosssectionsActor);
	r->RemoveActor(m_leftBankLineActor);
	r->RemoveActor(m_rightBankLineActor);
	r->RemoveActor(m_riverCenterLineActor);
	r->RemoveActor(m_backgroundActor);
	r->RemoveActor(m_labelActor);
	r->RemoveActor(m_blackCrossectionsActor);
	r->RemoveActor(m_crosssectionLinesActor);

	delete m_gridThread;

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

	vtkSmartPointer<vtkDataSetMapper> mapper;
	vtkRenderer* r = renderer();

	// Label

	m_labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	m_rightBankPointSet->GetPointData()->AddArray(m_labelArray);
	m_labelMapper->SetInputData(m_rightBankPointSet);
	m_labelMapper->SetLabelModeToLabelFieldData();
	m_labelMapper->SetFieldDataName(m_labelArray->GetName());
	m_labelMapper->GetLabelTextProperty()->SetColor(0, 0, 0);
	m_labelMapper->GetLabelTextProperty()->SetFontSize(15);
	m_labelMapper->GetLabelTextProperty()->BoldOff();
	m_labelMapper->GetLabelTextProperty()->ItalicOff();
	m_labelMapper->GetLabelTextProperty()->ShadowOff();
	m_labelMapper->GetLabelTextProperty()->SetJustificationToLeft();
	m_labelMapper->GetLabelTextProperty()->SetVerticalJustificationToCentered();

	m_labelActor = vtkSmartPointer<vtkActor2D>::New();
	m_labelActor->SetMapper(m_labelMapper);
	r->AddActor(m_labelActor);

	// Cross section lines

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_firstAndLastCrosssections);

	m_firstAndLastCrosssectionsActor = vtkSmartPointer<vtkActor>::New();
	m_firstAndLastCrosssectionsActor->SetMapper(mapper);
	// lines are black.
	m_firstAndLastCrosssectionsActor->GetProperty()->SetLighting(false);
	m_firstAndLastCrosssectionsActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_firstAndLastCrosssectionsActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_crosssections);

	m_crossectionsActor = vtkSmartPointer<vtkActor>::New();
	m_crossectionsActor->SetMapper(mapper);
	// lines are black.
	m_crossectionsActor->GetProperty()->SetLighting(false);
	m_crossectionsActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_crossectionsActor);

	vtkSmartPointer<vtkStructuredGridGeometryFilter> f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	f->SetInputData(m_riverCenterLine);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(f->GetOutputPort());

	m_riverCenterLineActor = vtkSmartPointer<vtkLODActor>::New();
	m_riverCenterLineActor->VisibilityOff();
	m_riverCenterLineActor->SetMapper(mapper);

	// register grid with lower resolution.
	vtkSmartPointer<vtkStructuredGrid> tmpgrid = m_riverCenterLine;
	for (int i = 0; i < 2; ++i) {
		vtkSmartPointer<vtkExtractGrid> ext = vtkSmartPointer<vtkExtractGrid>::New();
		ext->SetInputData(tmpgrid);
		ext->SetSampleRate(3, 1, 1);
		f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		f->SetInputConnection(ext->GetOutputPort());

		mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		mapper->SetInputConnection(f->GetOutputPort());
		m_riverCenterLineActor->AddLODMapper(mapper);
		tmpgrid = ext->GetOutput();
	}

	m_riverCenterLineActor->GetProperty()->SetLighting(false);
	m_riverCenterLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_riverCenterLineActor);

	// Left bank line
	f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	f->SetInputData(m_leftBankLine);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(f->GetOutputPort());

	m_leftBankLineActor = vtkSmartPointer<vtkLODActor>::New();
	m_leftBankLineActor->SetMapper(mapper);

	// register grid with lower resolution.
	tmpgrid = m_leftBankLine;
	for (int i = 0; i < 2; ++i) {
		vtkSmartPointer<vtkExtractGrid> ext = vtkSmartPointer<vtkExtractGrid>::New();
		ext->SetInputData(tmpgrid);
		ext->SetSampleRate(3, 1, 1);
		f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		f->SetInputConnection(ext->GetOutputPort());

		mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		mapper->SetInputConnection(f->GetOutputPort());
		m_leftBankLineActor->AddLODMapper(mapper);
		tmpgrid = ext->GetOutput();
	}

	m_leftBankLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_leftBankLineActor);
	m_leftBankLineActor->GetProperty()->SetLighting(false);
	m_leftBankLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_leftBankLineActor);

	// Right bank line
	f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	f->SetInputData(m_rightBankLine);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(f->GetOutputPort());

	m_rightBankLineActor = vtkSmartPointer<vtkLODActor>::New();
	m_rightBankLineActor->SetMapper(mapper);

	// register grid with lower resolution.
	tmpgrid = m_rightBankLine;
	for (int i = 0; i < 2; ++i) {
		vtkSmartPointer<vtkExtractGrid> ext = vtkSmartPointer<vtkExtractGrid>::New();
		ext->SetInputData(tmpgrid);
		ext->SetSampleRate(3, 1, 1);
		f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		f->SetInputConnection(ext->GetOutputPort());

		mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		mapper->SetInputConnection(f->GetOutputPort());
		m_rightBankLineActor->AddLODMapper(mapper);
		tmpgrid = ext->GetOutput();
	}

	m_rightBankLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_rightBankLineActor);
	m_rightBankLineActor->GetProperty()->SetLighting(false);
	m_rightBankLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_rightBankLineActor);

	// black, red, and blue lines
	m_blackCrosssection = vtkSmartPointer<vtkUnstructuredGrid>::New();
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_blackCrosssection);
	m_blackCrossectionsActor = vtkSmartPointer<vtkActor>::New();
	m_blackCrossectionsActor->SetMapper(mapper);
	m_blackCrossectionsActor->GetProperty()->SetColor(0, 0, 0);
	m_blackCrossectionsActor->GetProperty()->SetLineWidth(7);
	m_blackCrossectionsActor->GetProperty()->SetOpacity(0.3);
	m_blackCrossectionsActor->VisibilityOff();
	r->AddActor(m_blackCrossectionsActor);

	// background color.
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_backgroundGrid);
	mapper->SetScalarModeToUsePointData();
	mapper->SetLookupTable(scalarsToColorsContainer()->vtkObj());
	mapper->UseLookupTableScalarRangeOn();
	mapper->SetScalarVisibility(true);

	m_backgroundActor = vtkSmartPointer<vtkActor>::New();
	m_backgroundActor->SetMapper(mapper);
	m_backgroundActor->VisibilityOff();
	r->AddActor(m_backgroundActor);

	// crosssection lines
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_crosssectionLines);
	m_crosssectionLinesActor = vtkSmartPointer<vtkActor>::New();
	m_crosssectionLinesActor->SetMapper(mapper);
	m_crosssectionLinesActor->GetProperty()->SetColor(0, 0, 0);
	m_crosssectionLinesActor->GetProperty()->SetLineWidth(1);
	m_crosssectionLinesActor->VisibilityOff();
	r->AddActor(m_crosssectionLinesActor);
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
	col->AddItem(impl->m_selectedRiverCenterPointsActor);
	col->AddItem(impl->m_selectedLeftBankPointsActor);
	col->AddItem(impl->m_selectedRightBankPointsActor);
	col->AddItem(impl->m_selectedCrossSectionLinesActor);
	col->AddItem(m_riverCenterLineActor);
	col->AddItem(m_leftBankLineActor);
	col->AddItem(m_rightBankLineActor);
	col->AddItem(m_crossectionsActor);
	if (m_setting.showBackground) {
		col->AddItem(m_backgroundActor);
	}
	if (m_setting.showLines) {
		col->AddItem(m_crosssectionLinesActor);
	}

	col2->RemoveAllItems();
	col2->AddItem(m_labelActor);

	updateVisibilityWithoutRendering();
}

void GeoDataRiverSurvey::informDeselection(PreProcessorGraphicsViewInterface* /*v*/)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	vtkActor2DCollection* col2 = actor2DCollection();
	col->RemoveAllItems();

	col->AddItem(m_leftBankLineActor);
	col->AddItem(m_rightBankLineActor);
	col->AddItem(m_firstAndLastCrosssectionsActor);
	col->AddItem(m_crossectionsActor);
	if (m_setting.showBackground) {
		col->AddItem(m_backgroundActor);
	}
	if (m_setting.showLines) {
		col->AddItem(m_crosssectionLinesActor);
	}
	col2->RemoveAllItems();
	col2->AddItem(m_labelActor);

	updateVisibilityWithoutRendering();
}

void GeoDataRiverSurvey::allActorsOff()
{
	m_riverCenterLineActor->VisibilityOff();
	m_leftBankLineActor->VisibilityOff();
	m_rightBankLineActor->VisibilityOff();
	m_crossectionsActor->VisibilityOff();
	m_firstAndLastCrosssectionsActor->VisibilityOff();
	m_backgroundActor->VisibilityOff();
	m_labelActor->VisibilityOff();
	m_crosssectionLinesActor->VisibilityOff();

	impl->m_riverCenterPointsActor->VisibilityOff();
	impl->m_selectedRiverCenterPointsActor->VisibilityOff();
	impl->m_selectedLeftBankPointsActor->VisibilityOff();
	impl->m_selectedRightBankPointsActor->VisibilityOff();
	impl->m_selectedCrossSectionLinesActor->VisibilityOff();
}

void GeoDataRiverSurvey::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GeoDataRiverSurvey::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_mouseEventMode) {
	case Impl::meNormal:
	case Impl::meTranslatePrepare:
	case Impl::meRotatePrepareRight:
	case Impl::meRotatePrepareLeft:
	case Impl::meShiftPrepare:
	case Impl::meMoveExtensionEndPointPrepareLeft:
	case Impl::meMoveExtensionEndPointPrepareRight:
	case Impl::meExpansionPrepareRight:
	case Impl::meExpansionPrepareLeft:
		m_keyboardModifiers = event->modifiers();
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case Impl::meTranslate:
	case Impl::meRotateRight:
	case Impl::meRotateLeft:
	case Impl::meShift:
	case Impl::meMoveExtentionEndPointLeft:
	case Impl::meMoveExtentionEndPointRight:
	case Impl::meExpansionRight:
	case Impl::meExpansionLeft:
	case Impl::meAddingExtension:
	case Impl::meInserting:
	case Impl::meTranslateDialog:
	case Impl::meRotateDialog:
	case Impl::meShiftDialog:
	case Impl::meExpansionDialog:
		break;
	}
}

void GeoDataRiverSurvey::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_mouseEventMode) {
	case Impl::meNormal:
	case Impl::meTranslatePrepare:
	case Impl::meRotatePrepareRight:
	case Impl::meRotatePrepareLeft:
	case Impl::meShiftPrepare:
	case Impl::meMoveExtensionEndPointPrepareLeft:
	case Impl::meMoveExtensionEndPointPrepareRight:
	case Impl::meExpansionPrepareRight:
	case Impl::meExpansionPrepareLeft:
		m_keyboardModifiers = event->modifiers();
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case Impl::meTranslate:
	case Impl::meRotateRight:
	case Impl::meRotateLeft:
	case Impl::meShift:
	case Impl::meMoveExtentionEndPointLeft:
	case Impl::meMoveExtentionEndPointRight:
	case Impl::meExpansionRight:
	case Impl::meExpansionLeft:
	case Impl::meAddingExtension:
	case Impl::meInserting:
	case Impl::meTranslateDialog:
	case Impl::meRotateDialog:
	case Impl::meShiftDialog:
	case Impl::meExpansionDialog:
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
	if ((impl->m_mouseEventMode == Impl::meAddingExtension || impl->m_mouseEventMode == Impl::meInserting) && m_leftButtonDown) {
		graphicsView()->emitWorldPosition(event->x(), event->y());
	} else {
		if (m_definingBoundingBox) {
			// drawing bounding box using mouse dragging.
			MouseBoundingBox* box = dataModel()->mouseBoundingBox();
			box->setEndPoint(event->x(), event->y());
			renderGraphicsView();
		} else {
			switch (impl->m_mouseEventMode) {
			case Impl::meNormal:
			case Impl::meTranslatePrepare:
			case Impl::meRotatePrepareRight:
			case Impl::meRotatePrepareLeft:
			case Impl::meShiftPrepare:
			case Impl::meMoveExtensionEndPointPrepareLeft:
			case Impl::meMoveExtensionEndPointPrepareRight:
			case Impl::meExpansionPrepareRight:
			case Impl::meExpansionPrepareLeft:
				m_currentPoint = QPoint(event->x(), event->y());
				updateMouseEventMode();
				updateMouseCursor(v);
				break;
			case Impl::meTranslate:
				// execute translation.
				iRICUndoStack::instance().push(new TranslateRiverPathPointCommand(m_currentPoint, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case Impl::meRotateRight:
				iRICUndoStack::instance().push(new MouseRotateRiverCrosssectionCommand(m_currentPoint, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case Impl::meRotateLeft:
				iRICUndoStack::instance().push(new MouseRotateRiverCrosssectionCommand(m_currentPoint, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case Impl::meShift:
				iRICUndoStack::instance().push(new MouseShiftRiverPathCenterCommand(m_currentPoint, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case Impl::meMoveExtentionEndPointLeft:
				iRICUndoStack::instance().push(new MouseMoveExtensionCommand(true, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case Impl::meMoveExtentionEndPointRight:
				iRICUndoStack::instance().push(new MouseMoveExtensionCommand(false, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case Impl::meExpansionRight:
			case Impl::meExpansionLeft:
				// not used.
				break;
			case Impl::meAddingExtension:
			case Impl::meInserting:
				if (m_leftButtonDown) {
					graphicsView()->emitWorldPosition(event->x(), event->y());
				}
				break;
			case Impl::meTranslateDialog:
			case Impl::meRotateDialog:
			case Impl::meShiftDialog:
			case Impl::meExpansionDialog:
				break;
			}
		}
	}
}

void GeoDataRiverSurvey::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		m_dragStartPoint = QPoint(event->x(), event->y());

		switch (impl->m_mouseEventMode) {
		case Impl::meNormal: {
				// start drawing the mouse bounding box.
				m_definingBoundingBox = true;
				MouseBoundingBox* box = dataModel()->mouseBoundingBox();
				box->setStartPoint(event->x(), event->y());
				box->enable();
				v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
				renderGraphicsView();
			}
			break;
		case Impl::meTranslatePrepare:
			impl->m_mouseEventMode = Impl::meTranslate;
			break;
		case Impl::meRotatePrepareRight:
			impl->m_mouseEventMode = Impl::meRotateRight;
			break;
		case Impl::meRotatePrepareLeft:
			impl->m_mouseEventMode = Impl::meRotateLeft;
			break;
		case Impl::meShiftPrepare:
			impl->m_mouseEventMode = Impl::meShift;
			break;
		case Impl::meMoveExtensionEndPointPrepareLeft:
			impl->m_mouseEventMode = Impl::meMoveExtentionEndPointLeft;
			break;
		case Impl::meMoveExtensionEndPointPrepareRight:
			impl->m_mouseEventMode = Impl::meMoveExtentionEndPointRight;
			break;
		case Impl::meExpansionPrepareRight:
		case Impl::meExpansionPrepareLeft:
			// do nothing. mouse expansion is not used.
			break;
		case Impl::meTranslate:
		case Impl::meRotateRight:
		case Impl::meRotateLeft:
		case Impl::meShift:
		case Impl::meMoveExtentionEndPointLeft:
		case Impl::meMoveExtentionEndPointRight:
		case Impl::meExpansionRight:
		case Impl::meExpansionLeft:
			// these does not happen.
			break;
		case Impl::meAddingExtension:
		case Impl::meInserting:
			m_leftButtonDown = true;
			graphicsView()->emitWorldPosition(event->x(), event->y());
			break;
		case Impl::meTranslateDialog:
		case Impl::meRotateDialog:
		case Impl::meShiftDialog:
		case Impl::meExpansionDialog:
			// do nothing.
			break;
		}
	} else if (event->button() == Qt::RightButton) {
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GeoDataRiverSurvey::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_mouseEventMode) {
		case Impl::meNormal:
			if (m_definingBoundingBox) {
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
			m_definingBoundingBox = false;
			m_currentPoint = QPoint(event->x(), event->y());
			updateMouseEventMode();
			updateMouseCursor(v);
			break;
		case Impl::meTranslatePrepare:
		case Impl::meRotatePrepareRight:
		case Impl::meRotatePrepareLeft:
		case Impl::meShiftPrepare:
		case Impl::meMoveExtensionEndPointPrepareLeft:
		case Impl::meMoveExtensionEndPointPrepareRight:
		case Impl::meExpansionPrepareRight:
		case Impl::meExpansionPrepareLeft:
			// do nothing.
			break;
		case Impl::meTranslate:
		case Impl::meRotateRight:
		case Impl::meRotateLeft:
		case Impl::meShift:
		case Impl::meMoveExtentionEndPointLeft:
		case Impl::meMoveExtentionEndPointRight:
		case Impl::meExpansionRight:
		case Impl::meExpansionLeft:
			// operation ended.
			m_currentPoint = QPoint(event->x(), event->y());
			updateMouseEventMode();
			updateMouseCursor(v);
			break;
		case Impl::meAddingExtension:
		case Impl::meInserting:
			m_leftButtonDown = false;
			break;
		case Impl::meTranslateDialog:
		case Impl::meRotateDialog:
		case Impl::meShiftDialog:
		case Impl::meExpansionDialog:
			// do nothing.
			break;
		}


	} else if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GeoDataRiverSurvey::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_mouseEventMode) {
	case Impl::meNormal:
	case Impl::meAddingExtension:
		v->setCursor(Qt::ArrowCursor);
		break;
	case Impl::meTranslate:
	case Impl::meTranslatePrepare:
		v->setCursor(impl->m_cursorMove);
		break;
	case Impl::meRotateRight:
	case Impl::meRotatePrepareRight:
	case Impl::meRotateLeft:
	case Impl::meRotatePrepareLeft:
		v->setCursor(impl->m_cursorRotate);
		break;
	case Impl::meShift:
	case Impl::meShiftPrepare:
		v->setCursor(impl->m_cursorShift);
		break;
	case Impl::meMoveExtentionEndPointLeft:
	case Impl::meMoveExtensionEndPointPrepareLeft:
	case Impl::meMoveExtentionEndPointRight:
	case Impl::meMoveExtensionEndPointPrepareRight:
		v->setCursor(impl->m_cursorMove);
		break;
	case Impl::meExpansionRight:
	case Impl::meExpansionPrepareRight:
	case Impl::meExpansionLeft:
	case Impl::meExpansionPrepareLeft:
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

	m_backgroundActor->GetProperty()->SetOpacity(m_setting.opacity);

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
	informDeselection(0);
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
	vtkDataArray* data = m_backgroundGrid->GetPointData()->GetArray("Data");
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
	m_points->Reset();
	m_rightBankPoints->Reset();
	m_firstAndLastCrosssections->Reset();
	m_crosssections->Reset();
	m_leftBankLine->Initialize();
	m_rightBankLine->Initialize();
	m_riverCenterLine->Initialize();

	vtkSmartPointer<vtkPoints> centerLinePoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> leftBankPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> rightBankPoints = vtkSmartPointer<vtkPoints>::New();

	// calculate the number of grid size of m_riverCenterLine etc.
	int pointCount = 0;
	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	while (p != nullptr) {
		++pointCount;
		p = p->nextPoint();
	}
	int gridSize = pointCount + (pointCount - 1) * (LINEDIVS - 1);
	m_riverCenterLine->SetDimensions(gridSize, 1, 1);
	m_leftBankLine->SetDimensions(gridSize, 1, 1);
	m_rightBankLine->SetDimensions(gridSize, 1, 1);

	p = m_headPoint->nextPoint();
	double point[3];
	point[2] = 0;
	int index = 0;
	vtkLine* line;
	bool firstOrLast = false;
	bool first = true;
	while (p != nullptr) {
		firstOrLast = first || (p->nextPoint() == nullptr);
		// left bank
		QPointF leftBank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
		point[0] = leftBank.x();
		point[1] = leftBank.y();
		m_points->InsertNextPoint(point);
		++index;

		// left fixed point
		QPointF leftFixed;
		if (p->crosssection().fixedPointLSet()) {
			leftFixed = p->crosssectionPosition(p->crosssection().fixedPointL().position());
		} else {
			// use left bank.
			leftFixed = p->crosssectionPosition(p->crosssection().leftBank(true).position());
		}
		point[0] = leftFixed.x();
		point[1] = leftFixed.y();
		m_points->InsertNextPoint(point);

		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		m_crosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		if (firstOrLast) {
			m_firstAndLastCrosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		}
		line->Delete();

		++index;

		// river center
		point[0] = p->position().x();
		point[1] = p->position().y();
		m_points->InsertNextPoint(point);

		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		m_crosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		if (firstOrLast) {
			m_firstAndLastCrosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		}
		line->Delete();

		++index;

		// right fixed point
		QPointF rightFixed;
		if (p->crosssection().fixedPointRSet()) {
			rightFixed = p->crosssectionPosition(p->crosssection().fixedPointR().position());
		} else {
			// use right bank.
			rightFixed = p->crosssectionPosition(p->crosssection().rightBank(true).position());
		}
		point[0] = rightFixed.x();
		point[1] = rightFixed.y();
		m_points->InsertNextPoint(point);

		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		m_crosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		if (firstOrLast) {
			m_firstAndLastCrosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		}
		line->Delete();

		++index;

		// right bank
		QPointF rightBank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
		point[0] = rightBank.x();
		point[1] = rightBank.y();
		m_points->InsertNextPoint(point);
		m_rightBankPoints->InsertNextPoint(point);

		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		m_crosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		if (firstOrLast) {
			m_firstAndLastCrosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		}
		line->Delete();

		++index;

		QPointF tmpp;
		// river center line
		point[0] = p->position().x();
		point[1] = p->position().y();
		centerLinePoints->InsertNextPoint(point);
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				tmpp = p->riverCenter()->interpolate(i / static_cast<double>(LINEDIVS));
				point[0] = tmpp.x();
				point[1] = tmpp.y();
				centerLinePoints->InsertNextPoint(point);
			}
		}
		// left bank line
		tmpp = p->crosssectionPosition(p->crosssection().leftBank(true).position());
		point[0] = tmpp.x();
		point[1] = tmpp.y();
		leftBankPoints->InsertNextPoint(point);
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				tmpp = p->leftBank()->interpolate(i / static_cast<double>(LINEDIVS));
				point[0] = tmpp.x();
				point[1] = tmpp.y();
				leftBankPoints->InsertNextPoint(point);
			}
		}
		// right bank line
		tmpp = p->crosssectionPosition(p->crosssection().rightBank(true).position());
		point[0] = tmpp.x();
		point[1] = tmpp.y();
		rightBankPoints->InsertNextPoint(point);
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				tmpp = p->rightBank()->interpolate(i / static_cast<double>(LINEDIVS));
				point[0] = tmpp.x();
				point[1] = tmpp.y();
				rightBankPoints->InsertNextPoint(point);
			}
		}
		p = p->nextPoint();
		first = false;
	}
	m_points->Modified();
	m_rightBankPoints->Modified();
	m_firstAndLastCrosssections->SetPoints(m_points);
	m_firstAndLastCrosssections->Modified();
	m_crosssections->SetPoints(m_points);
	m_crosssections->Modified();

	m_riverCenterLine->SetPoints(centerLinePoints);
	m_riverCenterLine->Modified();
	m_leftBankLine->SetPoints(leftBankPoints);
	m_leftBankLine->Modified();
	m_rightBankLine->SetPoints(rightBankPoints);
	m_rightBankLine->Modified();

	m_rightBankPointSet->SetPoints(m_rightBankPoints);
	m_labelArray->Reset();
	p = m_headPoint->nextPoint();
	while (p != nullptr) {
		QString name = p->name();
		name.prepend(tr("  "));
		m_labelArray->InsertNextValue(iRIC::toStr(name).c_str());
		p = p->nextPoint();
	}

	m_rightBankPointSet->Modified();

	m_backgroundActor->VisibilityOff();

	m_crosssectionLines->Reset();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	p = m_headPoint->nextPoint();
	vtkIdType pointNum = 0;
	while (p != nullptr) {
		double maxHeight = 0;
		GeoDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
		// calculate maxHeight.
		for (int i = 0; i < alist.size(); ++i) {
			GeoDataRiverCrosssection::Altitude alt = alist[i];
			if (i == 0 || maxHeight < alt.height()) {maxHeight = alt.height();}
		}
		// now draw lines.
		QPointF offsetDir = p->crosssectionDirection();
		iRIC::rotateVector270(offsetDir);

		double offset;
		GeoDataRiverCrosssection::Altitude alt = alist[0];
		offset = (maxHeight - alt.height()) * m_setting.crosssectionLinesScale;
		QPointF tmpp = p->crosssectionPosition(alt.position()) + offsetDir * offset;
		points->InsertNextPoint(tmpp.x(), tmpp.y(), 0);
		++ pointNum;
		for (int i = 1; i < alist.size(); ++i) {
			GeoDataRiverCrosssection::Altitude alt = alist[i];
			offset = (maxHeight - alt.height()) * m_setting.crosssectionLinesScale;
			QPointF tmpp = p->crosssectionPosition(alt.position()) + offsetDir * offset;
			points->InsertNextPoint(tmpp.x(), tmpp.y(), 0);
			++ pointNum;
			vtkSmartPointer<vtkLine> tmpline = vtkSmartPointer<vtkLine>::New();
			tmpline->GetPointIds()->SetId(0, pointNum - 2);
			tmpline->GetPointIds()->SetId(1, pointNum - 1);
			m_crosssectionLines->InsertNextCell(tmpline->GetCellType(), tmpline->GetPointIds());
		}
		p = p->nextPoint();
	}
	m_crosssectionLines->SetPoints(points);
	points->Modified();
	m_crosssectionLines->Modified();
	m_crosssectionLines->BuildLinks();

	vtkActorCollection* col = actorCollection();
	col->RemoveAllItems();

	col->AddItem(m_leftBankLineActor);
	col->AddItem(m_rightBankLineActor);
	col->AddItem(m_firstAndLastCrosssectionsActor);
	col->AddItem(m_crossectionsActor);
	if (m_setting.showBackground) {
		col->AddItem(m_backgroundActor);
		m_backgroundActor->GetProperty()->SetOpacity(m_setting.opacity);
	}
	m_crosssectionLinesActor->VisibilityOff();

	if (m_setting.showLines) {
		col->AddItem(m_crosssectionLinesActor);
		m_crosssectionLinesActor->GetProperty()->SetColor(m_setting.crosssectionLinesColor);
	}

	vtkActor2DCollection* col2 = actor2DCollection();
	col2->AddItem(m_labelActor);

	impl->updateVtkPointsObjects();
	impl->updateSelectedVtkObjects();

	updateVisibilityWithoutRendering();

	emit dataUpdated();
	m_gridThread->update();
}

void GeoDataRiverSurvey::updateSelectionShapeData()
{
	impl->updateSelectedVtkObjects();
}

GeoDataRiverPathPoint* GeoDataRiverSurvey::headPoint() const
{
	return m_headPoint;
}

vtkStructuredGrid* GeoDataRiverSurvey::backgroundGrid() const
{
	return m_backgroundGrid;
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

	m_firstAndLastCrosssectionsActor->SetPosition(0, 0, lines);
	m_crossectionsActor->SetPosition(0, 0, lines);
	m_riverCenterLineActor->SetPosition(0, 0, lines);
	m_leftBankLineActor->SetPosition(0, 0, lines);
	m_rightBankLineActor->SetPosition(0, 0, lines);
	m_blackCrossectionsActor->SetPosition(0, 0, backlines);
	m_crosssectionLinesActor->SetPosition(0, 0, backlines);
	m_backgroundActor->SetPosition(0, 0, background);

	impl->m_riverCenterPointsActor->SetPosition(0, 0, points);
	impl->m_selectedRiverCenterPointsActor->SetPosition(0, 0, points);
	impl->m_selectedLeftBankPointsActor->SetPosition(0, 0, points);
	impl->m_selectedRightBankPointsActor->SetPosition(0, 0, points);
	impl->m_selectedCrossSectionLinesActor->SetPosition(0, 0, lines);
}

void GeoDataRiverSurvey::moveSelectedPoints()
{
	GeoDataRiverPathPointMoveDialog* dialog = new GeoDataRiverPathPointMoveDialog(this, preProcessorWindow());
	impl->m_mouseEventMode = Impl::meTranslateDialog;
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
	impl->m_mouseEventMode = Impl::meShiftDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::expandSelectedPoints()
{
	GeoDataRiverPathPointExpandDialog* dialog = new GeoDataRiverPathPointExpandDialog(this, preProcessorWindow());
	impl->m_mouseEventMode = Impl::meExpansionDialog;
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
	impl->m_mouseEventMode = Impl::meRotateDialog;
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
	impl->m_mouseEventMode = Impl::meAddingExtension;
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
	impl->m_mouseEventMode = Impl::meAddingExtension;
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
	impl->m_mouseEventMode = Impl::meNormal;
}

void GeoDataRiverSurvey::insertNewPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointInsertDialog* dialog = new GeoDataRiverPathPointInsertDialog(selected, true, this, preProcessorWindow());
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_mouseEventMode = Impl::meInserting;

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
	impl->m_mouseEventMode = Impl::meInserting;

	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::updateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();
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
			if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
					(m_keyboardModifiers & Qt::ControlModifier) == 0) {
				// preparing for moving
				impl->m_mouseEventMode = Impl::meTranslatePrepare;
			}
			if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
					(m_keyboardModifiers & Qt::ControlModifier) == 0) {
				// preparing for center-point shift
				impl->m_mouseEventMode = Impl::meShiftPrepare;
			}
		} else {
			impl->m_mouseEventMode = Impl::meNormal;
			QPointF lbank = selected->crosssectionPosition(selected->crosssection().leftBank(true).position());
			if (iRIC::lengthSquared(lbank - worldPos) < stdLen2 * 9) {
				// cursor is near left bank.
				if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0) {
					// preparing for rotating
					impl->m_mouseEventMode = Impl::meRotatePrepareLeft;
				}
				if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0 &&
						selected->crosssection().fixedPointLSet()) {
					// preparing for center-point shift
					impl->m_mouseEventMode = Impl::meMoveExtensionEndPointPrepareLeft;
				}
			}
			QPointF rbank = selected->crosssectionPosition(selected->crosssection().rightBank(true).position());
			if (iRIC::lengthSquared(rbank - worldPos) < stdLen2 * 9) {
				// cursor is near right bank.
				if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0) {
					// preparing for rotating
					impl->m_mouseEventMode = Impl::meRotatePrepareRight;
				}
				if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0 &&
						selected->crosssection().fixedPointRSet()) {
					// preparing for center-point shift
					impl->m_mouseEventMode = Impl::meMoveExtensionEndPointPrepareRight;
				}
			}
		}
	} else {
		// multiple selection, or none.
		GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
		impl->m_mouseEventMode = Impl::meNormal;
		while (p != nullptr) {
			if (p->IsSelected) {
				if (iRIC::lengthSquared(p->position() - worldPos) < stdLen2 * 9) {
					// cursor is near to the river center point
					if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
							(m_keyboardModifiers & Qt::ControlModifier) == 0) {
						// preparing for moving
						impl->m_mouseEventMode = Impl::meTranslatePrepare;
						return;
					}
					if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
							(m_keyboardModifiers & Qt::ControlModifier) == 0) {
						// preparing for center-point shift
						impl->m_mouseEventMode = Impl::meShiftPrepare;
						return;
					}
				} else {
					impl->m_mouseEventMode = Impl::meNormal;
					QPointF lbank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
					if (iRIC::lengthSquared(lbank - worldPos) < stdLen2 * 9) {
						// cursor is near left bank.
						if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
								(m_keyboardModifiers & Qt::ControlModifier) == 0) {
							// preparing for center-point shift
							impl->m_mouseEventMode = Impl::meExpansionPrepareLeft;
							return;
						}
					}
					QPointF rbank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
					if (iRIC::lengthSquared(rbank - worldPos) < stdLen2 * 9) {
						// cursor is near right bank.
						if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
								(m_keyboardModifiers & Qt::ControlModifier) == 0) {
							// preparing for center-point shift
							impl->m_mouseEventMode = Impl::meExpansionPrepareRight;
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
	m_backgroundGrid = m_gridThread->grid();
	m_backgroundGrid->GetPointData()->SetActiveScalars("Data");

	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	m_gridThread->startBGGridCopy();
	while (p != nullptr) {
		vtkPointSet* grid = m_gridThread->partialGrid(p);
		if (grid != nullptr) {
			p->areaGrid()->DeepCopy(grid);
		}
		p = p->nextPoint();
	}
	m_gridThread->finishBGGridCopy();
	vtkDataSetMapper* mapper = vtkDataSetMapper::SafeDownCast(m_backgroundActor->GetMapper());
	mapper->SetInputData(m_backgroundGrid);
	if (isVisible() && m_setting.showBackground) {
		m_backgroundActor->VisibilityOn();
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
		m_blackCrossectionsActor->VisibilityOff();
	} else {
		setupLine(m_blackCrosssection, black);
		m_blackCrossectionsActor->VisibilityOn();
	}
	renderGraphicsView();
}

void GeoDataRiverSurvey::setGridCreatingCondition(GridCreatingConditionRiverSurveyInterface* cond)
{
	m_gridCreatingCondition = cond;
}

GridCreatingConditionRiverSurveyInterface* GeoDataRiverSurvey::gridCreatingCondition() const
{
	return m_gridCreatingCondition;
}

void GeoDataRiverSurvey::setupLine(vtkUnstructuredGrid* grid, GeoDataRiverPathPoint* p)
{
	grid->Reset();
	grid->Allocate(5, 5);

	double point[3];
	point[2] = 0;
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkLine> line;
	points->SetDataTypeToDouble();
	grid->SetPoints(points);

	// left bank
	QPointF leftBank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
	point[0] = leftBank.x();
	point[1] = leftBank.y();
	points->InsertNextPoint(point);

	// left fixed point
	QPointF leftFixed;
	if (p->crosssection().fixedPointLSet()) {
		leftFixed = p->crosssectionPosition(p->crosssection().fixedPointL().position());
	} else {
		// use left bank.
		leftFixed = p->crosssectionPosition(p->crosssection().leftBank(true).position());
	}
	point[0] = leftFixed.x();
	point[1] = leftFixed.y();
	points->InsertNextPoint(point);

	// line between left bank and left fixed point
	line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 0);
	line->GetPointIds()->SetId(1, 1);
	grid->InsertNextCell(line->GetCellType(), line->GetPointIds());

	// river center
	point[0] = p->position().x();
	point[1] = p->position().y();
	points->InsertNextPoint(point);

	// line between left fixed point and river center
	line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 1);
	line->GetPointIds()->SetId(1, 2);
	grid->InsertNextCell(line->GetCellType(), line->GetPointIds());

	// right fixed point
	QPointF rightFixed;
	if (p->crosssection().fixedPointRSet()) {
		rightFixed = p->crosssectionPosition(p->crosssection().fixedPointR().position());
	} else {
		// use right bank.
		rightFixed = p->crosssectionPosition(p->crosssection().rightBank(true).position());
	}
	point[0] = rightFixed.x();
	point[1] = rightFixed.y();
	points->InsertNextPoint(point);

	// line between river center and right fixed point.
	line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 2);
	line->GetPointIds()->SetId(1, 3);
	grid->InsertNextCell(line->GetCellType(), line->GetPointIds());

	// right bank
	QPointF rightBank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
	point[0] = rightBank.x();
	point[1] = rightBank.y();
	points->InsertNextPoint(point);

	// line betwen right fixed point and right bank
	line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 3);
	line->GetPointIds()->SetId(1, 4);
	grid->InsertNextCell(line->GetCellType(), line->GetPointIds());
}

void GeoDataRiverSurvey::useDivisionPointsForBackgroundGrid(bool use)
{
	m_gridThread->setUseDivisionPoints(use);
	m_gridThread->update();
}

void GeoDataRiverSurvey::refreshBackgroundGrid()
{
	m_gridThread->update();
}

void GeoDataRiverSurvey::cancelBackgroundGridUpdate()
{
	m_gridThread->cancel();
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
