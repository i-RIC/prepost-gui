#include "rawdatarivercrosssection.h"
#include "rawdatariverpathpointexpanddialog.h"
#include "rawdatariverpathpointextensionadddialog.h"
#include "rawdatariverpathpointinsertdialog.h"
#include "rawdatariverpathpointmovedialog.h"
#include "rawdatariverpathpointrenamedialog.h"
#include "rawdatariverpathpointrotatedialog.h"
#include "rawdatariverpathpointshiftdialog.h"
#include "rawdatariversurvey.h"
#include "rawdatariversurveybackgroundgridcreatethread.h"
#include "rawdatariversurveycrosssectionwindow.h"
#include "rawdatariversurveycrosssectionwindowprojectdataitem.h"
#include "rawdatariversurveydisplaysettingdialog.h"
#include "rawdatariversurveyproxy.h"

#include <iriclib_riversurvey.h>

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioneditdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>

#include <QAction>
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

RawDataRiverSurvey::RawDataRiverSurvey(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
	: RawData(d, creator, condition)
{
	m_headPoint = new RawDataRiverPathPoint("Dummy", 0, 0);

	m_points = vtkSmartPointer<vtkPoints>::New();
	m_points->SetDataTypeToDouble();
	m_rightBankPoints = vtkSmartPointer<vtkPoints>::New();
	m_rightBankPoints->SetDataTypeToDouble();
	// setup grid.
	m_riverCenters = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_selectedRiverCenters = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_selectedLeftBanks = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_selectedRightBanks = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_rightBankPointSet = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_firstAndLastCrosssections = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_crosssections = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_selectedCrosssections = vtkSmartPointer<vtkUnstructuredGrid>::New();

	m_leftBankLine = vtkSmartPointer<vtkStructuredGrid>::New();
	m_rightBankLine = vtkSmartPointer<vtkStructuredGrid>::New();
	m_riverCenterLine = vtkSmartPointer<vtkStructuredGrid>::New();

	m_backgroundGrid = vtkSmartPointer<vtkStructuredGrid>::New();

	m_crosssectionLines = vtkSmartPointer<vtkUnstructuredGrid>::New();

	m_labelArray = vtkSmartPointer<vtkStringArray>::New();
	m_labelArray->SetName("Label");

	m_definingBoundingBox = false;
	m_leftButtonDown = false;
	m_showBackground = false;
	m_showLines = false;
	m_opacityPercent = 50;
	m_crosssectionLinesScale = 10;
	m_crosssectionLinesColor = Qt::red;
	m_mouseEventMode = meNormal;

	m_gridCreatingCondition = nullptr;
	m_rightClickingMenu = nullptr;

	m_gridThread = new RawDataRiverSurveyBackgroundGridCreateThread(this);
	connect(m_gridThread, SIGNAL(gridUpdated()), this, SLOT(updateBackgroundGrid()));

	setupCursors();
	setupActions();

	RiverSplineSolver::setLinearMode(false, m_headPoint);
}

RawDataRiverSurvey::~RawDataRiverSurvey()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_riverCenterActor);
	r->RemoveActor(m_selectedRiverCenterActor);
	r->RemoveActor(m_selectedLeftBankActor);
	r->RemoveActor(m_selectedRightBankActor);
	r->RemoveActor(m_crossectionsActor);
	r->RemoveActor(m_firstAndLastCrosssectionsActor);
	r->RemoveActor(m_selectedCrossectionsActor);
	r->RemoveActor(m_leftBankLineActor);
	r->RemoveActor(m_rightBankLineActor);
	r->RemoveActor(m_riverCenterLineActor);
	r->RemoveActor(m_backgroundActor);
	r->RemoveActor(m_labelActor);
	r->RemoveActor(m_blackCrossectionsActor);
	r->RemoveActor(m_redCrossectionsActor);
	r->RemoveActor(m_blueCrossectionsActor);
	r->RemoveActor(m_crosssectionLinesActor);

	delete m_gridThread;
	if (m_rightClickingMenu) {
		delete m_rightClickingMenu;
	}
}

void RawDataRiverSurvey::setupActors()
{
	vtkSmartPointer<vtkDataSetMapper> mapper;
	vtkRenderer* r = renderer();

	// River center points

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_riverCenters);

	m_riverCenterActor = vtkSmartPointer<vtkActor>::New();
	m_riverCenterActor->SetMapper(mapper);
	m_riverCenterActor->GetProperty()->SetPointSize(5);
	m_riverCenterActor->GetProperty()->SetColor(0, 0, 1.0);
	m_riverCenterActor->VisibilityOff();
	r->AddActor(m_riverCenterActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_selectedRiverCenters);

	m_selectedRiverCenterActor = vtkSmartPointer<vtkActor>::New();
	m_selectedRiverCenterActor->SetMapper(mapper);
	m_selectedRiverCenterActor->GetProperty()->SetPointSize(9);
	m_selectedRiverCenterActor->GetProperty()->SetColor(0, 0, 1.0);
	r->AddActor(m_selectedRiverCenterActor);

	// Left bank points

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_selectedLeftBanks);

	m_selectedLeftBankActor = vtkSmartPointer<vtkActor>::New();
	m_selectedLeftBankActor->SetMapper(mapper);
	m_selectedLeftBankActor->GetProperty()->SetPointSize(5);
	m_selectedLeftBankActor->GetProperty()->SetColor(1.0, 0, 0);
	r->AddActor(m_selectedLeftBankActor);

	// Right bank points

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_selectedRightBanks);

	m_selectedRightBankActor = vtkSmartPointer<vtkActor>::New();
	m_selectedRightBankActor->SetMapper(mapper);
	m_selectedRightBankActor->GetProperty()->SetPointSize(5);
	m_selectedRightBankActor->GetProperty()->SetColor(0, 1.0, 0);
	r->AddActor(m_selectedRightBankActor);

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

	// Selected cross section lines
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_selectedCrosssections);

	m_selectedCrossectionsActor = vtkSmartPointer<vtkActor>::New();
	m_selectedCrossectionsActor->SetMapper(mapper);
	// lines are black, and bold
	m_selectedCrossectionsActor->GetProperty()->SetColor(0, 0, 0);
	m_selectedCrossectionsActor->GetProperty()->SetLineWidth(3);
	r->AddActor(m_selectedCrossectionsActor);

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

	m_redCrosssection = vtkSmartPointer<vtkUnstructuredGrid>::New();
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_redCrosssection);
	m_redCrossectionsActor = vtkSmartPointer<vtkActor>::New();
	m_redCrossectionsActor->SetMapper(mapper);
	m_redCrossectionsActor->GetProperty()->SetColor(1.0, 0, 0);
	m_redCrossectionsActor->GetProperty()->SetLineWidth(7);
	m_redCrossectionsActor->GetProperty()->SetOpacity(0.3);
	m_redCrossectionsActor->VisibilityOff();
	r->AddActor(m_redCrossectionsActor);

	m_blueCrosssection = vtkSmartPointer<vtkUnstructuredGrid>::New();
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_blueCrosssection);
	m_blueCrossectionsActor = vtkSmartPointer<vtkActor>::New();
	m_blueCrossectionsActor->SetMapper(mapper);
	m_blueCrossectionsActor->GetProperty()->SetColor(0, 0, 1.0);
	m_blueCrossectionsActor->GetProperty()->SetLineWidth(7);
	m_blueCrossectionsActor->GetProperty()->SetOpacity(0.3);
	m_blueCrossectionsActor->VisibilityOff();
	r->AddActor(m_blueCrossectionsActor);

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

	vtkActorCollection* col = actorCollection();
	col->AddItem(m_leftBankLineActor);
	col->AddItem(m_rightBankLineActor);
	col->AddItem(m_firstAndLastCrosssectionsActor);
	col->AddItem(m_crossectionsActor);
	if (m_showBackground) {
		col->AddItem(m_backgroundActor);
	}
	if (m_showLines) {
		col->AddItem(m_crosssectionLinesActor);
	}
	vtkActor2DCollection* col2 = actor2DCollection();
	col2->AddItem(m_labelActor);

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

void RawDataRiverSurvey::setupMenu()
{
	m_menu->setTitle(tr("&River Survey"));
	m_menu->addAction(m_editNameAction);

	m_menu->addSeparator();
	m_menu->addAction(m_openCrossSectionWindowAction);

	m_menu->addSeparator();
	m_menu->addAction(m_addUpperSideAction);
	m_menu->addAction(m_addLowerSideAction);
	m_menu->addAction(m_moveAction);
	m_menu->addAction(m_rotateAction);
	m_menu->addAction(m_shiftAction);
	m_menu->addAction(m_expandAction);
	m_menu->addAction(m_deleteAction);
	m_menu->addAction(m_renameAction);

	m_menu->addSeparator();
	m_menu->addAction(m_addLeftExtensionPointAction);
	m_menu->addAction(m_addRightExtensionPointAction);
	m_menu->addAction(m_removeLeftExtensionPointAction);
	m_menu->addAction(m_removeRightExtensionPointAction);

	m_menu->addSeparator();
	m_menu->addAction(m_showBackgroundAction);

//	m_menu->addSeparator();
//	m_menu->addAction(m_reverseAction);

	m_menu->addSeparator();
	QMenu* iMenu = m_menu->addMenu(tr("Interpolation Mode"));
	iMenu->addAction(m_interpolateSplineAction);
	iMenu->addAction(m_interpolateLinearAction);

	m_menu->addSeparator();
	m_menu->addAction(deleteAction());

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(m_openCrossSectionWindowAction);

	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addUpperSideAction);
	m_rightClickingMenu->addAction(m_addLowerSideAction);
	m_rightClickingMenu->addAction(m_moveAction);
	m_rightClickingMenu->addAction(m_rotateAction);
	m_rightClickingMenu->addAction(m_shiftAction);
	m_rightClickingMenu->addAction(m_expandAction);
	m_rightClickingMenu->addAction(m_deleteAction);
	m_rightClickingMenu->addAction(m_renameAction);

	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addLeftExtensionPointAction);
	m_rightClickingMenu->addAction(m_addRightExtensionPointAction);
	m_rightClickingMenu->addAction(m_removeLeftExtensionPointAction);
	m_rightClickingMenu->addAction(m_removeRightExtensionPointAction);

	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_showBackgroundAction);

	m_rightClickingMenu->addSeparator();
	iMenu = m_rightClickingMenu->addMenu(tr("Interpolation Mode"));
	iMenu->addAction(m_interpolateSplineAction);
	iMenu->addAction(m_interpolateLinearAction);
}

bool RawDataRiverSurvey::addToolBarButtons(QToolBar* /*tb*/)
{
	/*
		 tb->addAction(m_addVertexAction);
		 tb->addAction(m_removeVertexAction);
		 return true;
	 */
	return true;
}

void RawDataRiverSurvey::informSelection(PreProcessorGraphicsViewInterface*)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	vtkActor2DCollection* col2 = actor2DCollection();
	col->RemoveAllItems();
	col->AddItem(m_riverCenterActor);
	col->AddItem(m_selectedRiverCenterActor);
	col->AddItem(m_selectedLeftBankActor);
	col->AddItem(m_selectedRightBankActor);
	col->AddItem(m_riverCenterLineActor);
	col->AddItem(m_leftBankLineActor);
	col->AddItem(m_rightBankLineActor);
	col->AddItem(m_crossectionsActor);
	col->AddItem(m_selectedCrossectionsActor);
	if (m_showBackground) {
		col->AddItem(m_backgroundActor);
	}
	if (m_showLines) {
		col->AddItem(m_crosssectionLinesActor);
	}
	col2->RemoveAllItems();
	col2->AddItem(m_labelActor);

	updateVisibilityWithoutRendering();
}

void RawDataRiverSurvey::informDeselection(PreProcessorGraphicsViewInterface* /*v*/)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	vtkActor2DCollection* col2 = actor2DCollection();
	col->RemoveAllItems();
	col->AddItem(m_leftBankLineActor);
	col->AddItem(m_rightBankLineActor);
	col->AddItem(m_firstAndLastCrosssectionsActor);
	col->AddItem(m_crossectionsActor);
	if (m_showBackground) {
		col->AddItem(m_backgroundActor);
	}
	if (m_showLines) {
		col->AddItem(m_crosssectionLinesActor);
	}
	col2->RemoveAllItems();
	col2->AddItem(m_labelActor);

	updateVisibilityWithoutRendering();
}

void RawDataRiverSurvey::allActorsOff()
{
	m_riverCenterActor->VisibilityOff();
	m_selectedRiverCenterActor->VisibilityOff();
	m_selectedLeftBankActor->VisibilityOff();
	m_selectedRightBankActor->VisibilityOff();
	m_riverCenterLineActor->VisibilityOff();
	m_leftBankLineActor->VisibilityOff();
	m_rightBankLineActor->VisibilityOff();
	m_crossectionsActor->VisibilityOff();
	m_firstAndLastCrosssectionsActor->VisibilityOff();
	m_selectedCrossectionsActor->VisibilityOff();
	m_backgroundActor->VisibilityOff();
	m_labelActor->VisibilityOff();
	m_crosssectionLinesActor->VisibilityOff();
}

void RawDataRiverSurvey::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void RawDataRiverSurvey::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meNormal:
	case meTranslatePrepare:
	case meRotatePrepareRight:
	case meRotatePrepareLeft:
	case meShiftPrepare:
	case meMoveExtensionEndPointPrepareLeft:
	case meMoveExtensionEndPointPrepareRight:
	case meExpansionPrepareRight:
	case meExpansionPrepareLeft:
		m_keyboardModifiers = event->modifiers();
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case meTranslate:
	case meRotateRight:
	case meRotateLeft:
	case meShift:
	case meMoveExtentionEndPointLeft:
	case meMoveExtentionEndPointRight:
	case meExpansionRight:
	case meExpansionLeft:
	case meAddingExtension:
	case meInserting:
	case meTranslateDialog:
	case meRotateDialog:
	case meShiftDialog:
	case meExpansionDialog:
		break;
	}
}

void RawDataRiverSurvey::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meNormal:
	case meTranslatePrepare:
	case meRotatePrepareRight:
	case meRotatePrepareLeft:
	case meShiftPrepare:
	case meMoveExtensionEndPointPrepareLeft:
	case meMoveExtensionEndPointPrepareRight:
	case meExpansionPrepareRight:
	case meExpansionPrepareLeft:
		m_keyboardModifiers = event->modifiers();
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case meTranslate:
	case meRotateRight:
	case meRotateLeft:
	case meShift:
	case meMoveExtentionEndPointLeft:
	case meMoveExtentionEndPointRight:
	case meExpansionRight:
	case meExpansionLeft:
	case meAddingExtension:
	case meInserting:
	case meTranslateDialog:
	case meRotateDialog:
	case meShiftDialog:
	case meExpansionDialog:
		break;
	}
}

void RawDataRiverSurvey::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	RawDataRiverPathPoint* selP = selectedPoint();
	if (selP == nullptr) {
		// no point is selected.
		return;
	}
	openCrossSectionWindow();
}

class RawDataRiverSurveyTranslateCommand : public QUndoCommand
{
public:
	RawDataRiverSurveyTranslateCommand(QPoint from, QPoint to, RawDataRiverSurvey* data)
		: QUndoCommand(RawDataRiverSurvey::tr("Move Traversal Line")) {
		PreProcessorGraphicsViewInterface* gview = data->graphicsView();
		m_rs = data;
		double fromX, fromY, toX, toY;
		fromX = from.x();
		fromY = from.y();
		toX = to.x();
		toY = to.y();
		gview->viewportToWorld(fromX, fromY);
		gview->viewportToWorld(toX, toY);
		QVector2D offset(toX - fromX, toY - fromY);

		RawDataRiverPathPoint* p = data->m_headPoint->nextPoint();
		while (p != nullptr) {
			if (p->IsSelected) {
				m_points.append(p);
				m_oldPositions.append(p->position());
				m_newPositions.append(p->position() + offset);
			}
			p = p->nextPoint();
		}
	}
	void redo() {
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i) {
			m_points[i]->setPosition(m_newPositions[i]);
		}
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
		m_rs->m_mapped = false;
	}
	void undo() {
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i) {
			m_points[i]->setPosition(m_oldPositions[i]);
		}
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	int id() const {
		return iRIC::generateCommandId("RawDataRiverSurveyTranslateCommand");
	}
	virtual bool mergeWith(const QUndoCommand* other) {
		const RawDataRiverSurveyTranslateCommand* other2 = dynamic_cast<const RawDataRiverSurveyTranslateCommand*>(other);
		if (other2 == nullptr) { return false; }
		if (other2->m_points == m_points) {
			m_newPositions = other2->m_newPositions;
			return true;
		}
		return false;
	}

private:
	QList<RawDataRiverPathPoint*> m_points;
	QList<QVector2D> m_oldPositions;
	QList<QVector2D> m_newPositions;
	RawDataRiverSurvey* m_rs;
};

class RawDataRiverSurveyMouseRotateCommand : public QUndoCommand
{
public:
	RawDataRiverSurveyMouseRotateCommand(QPoint from, QPoint to, RawDataRiverSurvey* data)
		: QUndoCommand(RawDataRiverSurvey::tr("Rotate Traversal Line")) {
		RawDataRiverPathPoint* p = data->m_headPoint->nextPoint();
		while (p != nullptr) {
			if (p->IsSelected) {
				m_point = p;
				break;
			}
			p = p->nextPoint();
		}
		PreProcessorGraphicsViewInterface* gview = data->graphicsView();
		m_rs = data;
		m_oldDirection = m_point->crosssectionDirection();

		double fromX, fromY, toX, toY;
		fromX = from.x();
		fromY = from.y();
		toX = to.x();
		toY = to.y();
		gview->viewportToWorld(fromX, fromY);
		gview->viewportToWorld(toX, toY);
		QVector2D vec1 = QVector2D(fromX - m_point->position().x(), fromY - m_point->position().y());
		QVector2D vec2 = QVector2D(toX - m_point->position().x(), toY - m_point->position().y());
		double angle = iRIC::angleRadian(vec1, vec2);

		m_newDirection = m_oldDirection;
		iRIC::rotateVectorRadian(m_newDirection, angle);
	}
	void redo() {
		m_rs->m_gridThread->cancel();
		m_point->setCrosssectionDirection(m_newDirection);
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->m_mapped = false;
	}
	void undo() {
		m_rs->m_gridThread->cancel();
		m_point->setCrosssectionDirection(m_oldDirection);
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	int id() const {
		return iRIC::generateCommandId("RawDataRiverSurveyMouseRotate");
	}
	virtual bool mergeWith(const QUndoCommand* other) {
		const RawDataRiverSurveyMouseRotateCommand* other2 = dynamic_cast<const RawDataRiverSurveyMouseRotateCommand*>(other);
		if (other2 == nullptr) { return false; }
		if (other2->m_point == m_point) {
			m_newDirection = other2->m_newDirection;
			return true;
		}
		return false;
	}

private:
	RawDataRiverPathPoint* m_point;
	QVector2D m_oldDirection;
	QVector2D m_newDirection;
	RawDataRiverSurvey* m_rs;
};

class RawDataRiverSurveyMouseShiftCommand : public QUndoCommand
{
public:
	RawDataRiverSurveyMouseShiftCommand(QPoint from, QPoint to, RawDataRiverSurvey* data)
		: QUndoCommand(RawDataRiverSurvey::tr("Shift Center Point")) {
		PreProcessorGraphicsViewInterface* gview = data->graphicsView();
		m_rs = data;
		double fromX, fromY, toX, toY;
		fromX = from.x();
		fromY = from.y();
		toX = to.x();
		toY = to.y();
		gview->viewportToWorld(fromX, fromY);
		gview->viewportToWorld(toX, toY);
		QVector2D offset(toX - fromX, toY - fromY);
		RawDataRiverPathPoint* first = nullptr;

		RawDataRiverPathPoint* p = data->m_headPoint->nextPoint();
		while (p != nullptr) {
			if (p->IsSelected) {
				m_points.append(p);
				if (first == nullptr) {first = p;}
			}
			p = p->nextPoint();
		}
		m_shiftValue = QVector2D::dotProduct(offset, first->crosssectionDirection());
	}
	void redo() {
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i) {
			m_points[i]->shiftCenter(m_shiftValue);
		}
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
		m_rs->m_mapped = false;
	}
	void undo() {
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i) {
			m_points[i]->shiftCenter(- m_shiftValue);
		}
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	int id() const {
		return iRIC::generateCommandId("RawDataRiverSurveyMouseShift");
	}
	virtual bool mergeWith(const QUndoCommand* other) {
		const RawDataRiverSurveyMouseShiftCommand* other2 = dynamic_cast<const RawDataRiverSurveyMouseShiftCommand*>(other);
		if (other2 == nullptr) { return false; }
		if (other2->m_points == m_points) {
			m_shiftValue += other2->m_shiftValue;
			return true;
		}
		return false;
	}

private:
	QList<RawDataRiverPathPoint*> m_points;
	double m_shiftValue;
	RawDataRiverSurvey* m_rs;
};

class RawDataRiverSurveyMouseMoveExtensionCommand : public QUndoCommand
{
public:
	RawDataRiverSurveyMouseMoveExtensionCommand(bool left, QPoint to, RawDataRiverSurvey* data)
		: QUndoCommand(RawDataRiverSurvey::tr("Move Extension Line End")) {
		m_left = left;
		PreProcessorGraphicsViewInterface* gview = data->graphicsView();
		m_rs = data;
		double toX, toY;
		toX = to.x();
		toY = to.y();
		gview->viewportToWorld(toX, toY);

		RawDataRiverPathPoint* p = data->m_headPoint->nextPoint();
		while (p != nullptr) {
			if (p->IsSelected) {
				m_point = p;
				break;
			}
			p = p->nextPoint();
		}
		m_newPosition = QVector2D(toX, toY);
		if (m_left) {
			m_oldPosition = m_point->crosssectionPosition(m_point->crosssection().leftBank(true).position());
		} else {
			m_oldPosition = m_point->crosssectionPosition(m_point->crosssection().rightBank(true).position());
		}
	}
	void redo() {
		m_rs->m_gridThread->cancel();
		if (m_left) {
			m_point->moveExtentionPointLeft(m_newPosition);
		} else {
			m_point->moveExtentionPointRight(m_newPosition);
		}
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
		m_rs->m_mapped = false;
	}
	void undo() {
		m_rs->m_gridThread->cancel();
		if (m_left) {
			m_point->moveExtentionPointLeft(m_oldPosition);
		} else {
			m_point->moveExtentionPointRight(m_oldPosition);
		}
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	int id() const {
		return iRIC::generateCommandId(" RawDataRiverPathPointMouseMoveExtension");
	}
	virtual bool mergeWith(const QUndoCommand* other) {
		const RawDataRiverSurveyMouseMoveExtensionCommand* other2 = dynamic_cast<const RawDataRiverSurveyMouseMoveExtensionCommand*>(other);
		if (other2 == nullptr) { return false; }
		if (other2->m_point == m_point && other2->m_left == m_left) {
			m_newPosition = other2->m_newPosition;
			return true;
		}
		return false;
	}

private:
	bool m_left;
	QVector2D m_oldPosition;
	QVector2D m_newPosition;
	RawDataRiverPathPoint* m_point;
	RawDataRiverSurvey* m_rs;
};

void RawDataRiverSurvey::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if ((m_mouseEventMode == meAddingExtension || m_mouseEventMode == meInserting) && m_leftButtonDown) {
		graphicsView()->emitWorldPosition(event->x(), event->y());
	} else {
		if (m_definingBoundingBox) {
			// drawing bounding box using mouse dragging.
			MouseBoundingBox* box = dataModel()->mouseBoundingBox();
			box->setEndPoint(event->x(), event->y());
			renderGraphicsView();
		} else {
			switch (m_mouseEventMode) {
			case meNormal:
			case meTranslatePrepare:
			case meRotatePrepareRight:
			case meRotatePrepareLeft:
			case meShiftPrepare:
			case meMoveExtensionEndPointPrepareLeft:
			case meMoveExtensionEndPointPrepareRight:
			case meExpansionPrepareRight:
			case meExpansionPrepareLeft:
				m_currentPoint = QPoint(event->x(), event->y());
				updateMouseEventMode();
				updateMouseCursor(v);
				break;
			case meTranslate:
				// execute translation.
				iRICUndoStack::instance().push(new RawDataRiverSurveyTranslateCommand(m_currentPoint, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case meRotateRight:
				iRICUndoStack::instance().push(new RawDataRiverSurveyMouseRotateCommand(m_currentPoint, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case meRotateLeft:
				iRICUndoStack::instance().push(new RawDataRiverSurveyMouseRotateCommand(m_currentPoint, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case meShift:
				iRICUndoStack::instance().push(new RawDataRiverSurveyMouseShiftCommand(m_currentPoint, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case meMoveExtentionEndPointLeft:
				iRICUndoStack::instance().push(new RawDataRiverSurveyMouseMoveExtensionCommand(true, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case meMoveExtentionEndPointRight:
				iRICUndoStack::instance().push(new RawDataRiverSurveyMouseMoveExtensionCommand(false, QPoint(event->x(), event->y()), this));
				m_currentPoint = QPoint(event->x(), event->y());
				break;
			case meExpansionRight:
			case meExpansionLeft:
				// not used.
				break;
			case meAddingExtension:
			case meInserting:
				if (m_leftButtonDown) {
					graphicsView()->emitWorldPosition(event->x(), event->y());
				}
				break;
			case meTranslateDialog:
			case meRotateDialog:
			case meShiftDialog:
			case meExpansionDialog:
				break;
			}
		}
	}
}

void RawDataRiverSurvey::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		m_dragStartPoint = QPoint(event->x(), event->y());

		switch (m_mouseEventMode) {
		case meNormal: {
				// start drawing the mouse bounding box.
				m_definingBoundingBox = true;
				MouseBoundingBox* box = dataModel()->mouseBoundingBox();
				box->setStartPoint(event->x(), event->y());
				box->enable();
				v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
				renderGraphicsView();
			}
			break;
		case meTranslatePrepare:
			m_mouseEventMode = meTranslate;
			break;
		case meRotatePrepareRight:
			m_mouseEventMode = meRotateRight;
			break;
		case meRotatePrepareLeft:
			m_mouseEventMode = meRotateLeft;
			break;
		case meShiftPrepare:
			m_mouseEventMode = meShift;
			break;
		case meMoveExtensionEndPointPrepareLeft:
			m_mouseEventMode = meMoveExtentionEndPointLeft;
			break;
		case meMoveExtensionEndPointPrepareRight:
			m_mouseEventMode = meMoveExtentionEndPointRight;
			break;
		case meExpansionPrepareRight:
		case meExpansionPrepareLeft:
			// do nothing. mouse expansion is not used.
			break;
		case meTranslate:
		case meRotateRight:
		case meRotateLeft:
		case meShift:
		case meMoveExtentionEndPointLeft:
		case meMoveExtentionEndPointRight:
		case meExpansionRight:
		case meExpansionLeft:
			// these does not happen.
			break;
		case meAddingExtension:
		case meInserting:
			m_leftButtonDown = true;
			graphicsView()->emitWorldPosition(event->x(), event->y());
			break;
		case meTranslateDialog:
		case meRotateDialog:
		case meShiftDialog:
		case meExpansionDialog:
			// do nothing.
			break;
		}
	} else if (event->button() == Qt::RightButton) {
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

class RawDataRiverSurveySelectionChangeCommand : public QUndoCommand
{
public:
	RawDataRiverSurveySelectionChangeCommand(RawDataRiverSurvey* rs, MouseBoundingBox* box)
		: QUndoCommand(RawDataRiverSurvey::tr("Selection Change")) {
		m_rs = rs;
		// store old selection info.
		buildSelectedPointsSet(m_oldSelectedPoints);

		// now, update the selection statuses of river path points.
		double point[3];
		vtkPoints* points = box->vtkGrid()->GetPoints();
		QVector2D leftTop, rightTop, leftBottom;
		// left top
		points->GetPoint(0, point);
		leftTop = QVector2D(point[0], point[1]);
		// right top
		points->GetPoint(1, point);
		rightTop = QVector2D(point[0], point[1]);
		// left bottom
		points->GetPoint(3, point);
		leftBottom = QVector2D(point[0], point[1]);

		// do the selection!
		m_rs->headPoint()->selectRegion(leftTop, rightTop - leftTop, leftBottom - leftTop);

		// store new selection info.
		buildSelectedPointsSet(m_newSelectedPoints);
	}
	void buildSelectedPointsSet(QSet<RawDataRiverPathPoint*>& set) {
		set.clear();
		RawDataRiverPathPoint* p = m_rs->headPoint();
		if (p != nullptr) {p = p->nextPoint();}
		while (p != nullptr) {
			if (p->IsSelected) {
				set.insert(p);
			}
			p = p->nextPoint();
		}
	}
	void redo() {
		RawDataRiverPathPoint* p = m_rs->headPoint();
		if (p != nullptr) {p = p->nextPoint();}
		while (p != nullptr) {
			p->IsSelected = m_newSelectedPoints.contains(p);
			p = p->nextPoint();
		}
		m_rs->updateActionStatus();
		m_rs->updateSelectionShapeData();
		m_rs->renderGraphicsView();
	}
	void undo() {
		RawDataRiverPathPoint* p = m_rs->headPoint();
		if (p != nullptr) {p = p->nextPoint();}
		while (p != nullptr) {
			p->IsSelected = m_oldSelectedPoints.contains(p);
			p = p->nextPoint();
		}
		m_rs->updateActionStatus();
		m_rs->updateSelectionShapeData();
		m_rs->renderGraphicsView();
	}
private:
	QSet<RawDataRiverPathPoint*> m_oldSelectedPoints;
	QSet<RawDataRiverPathPoint*> m_newSelectedPoints;
	RawDataRiverSurvey* m_rs;
};


void RawDataRiverSurvey::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (m_mouseEventMode) {
		case meNormal:
			if (m_definingBoundingBox) {
				// bounding box selecting ended.
				MouseBoundingBox* box = dataModel()->mouseBoundingBox();
				box->setEndPoint(event->x(), event->y());
				box->disable();
				if (isNear(box->startPoint(), box->endPoint())) {
					int x = (box->startPoint().x() + box->endPoint().x()) / 2;
					int y = (box->startPoint().y() + box->endPoint().y()) / 2;
					box->setStartPoint(x - 8, y - 8);
					box->setEndPoint(x + 8, y + 8);
				}

				v->restoreUpdateRate();

				// selection change is made not undo-able
				QUndoCommand* com = new RawDataRiverSurveySelectionChangeCommand(this, box);
				com->redo();
				delete com;
			}
			m_definingBoundingBox = false;
			m_currentPoint = QPoint(event->x(), event->y());
			updateMouseEventMode();
			updateMouseCursor(v);
			break;
		case meTranslatePrepare:
		case meRotatePrepareRight:
		case meRotatePrepareLeft:
		case meShiftPrepare:
		case meMoveExtensionEndPointPrepareLeft:
		case meMoveExtensionEndPointPrepareRight:
		case meExpansionPrepareRight:
		case meExpansionPrepareLeft:
			// do nothing.
			break;
		case meTranslate:
		case meRotateRight:
		case meRotateLeft:
		case meShift:
		case meMoveExtentionEndPointLeft:
		case meMoveExtentionEndPointRight:
		case meExpansionRight:
		case meExpansionLeft:
			// operation ended.
			m_currentPoint = QPoint(event->x(), event->y());
			updateMouseEventMode();
			updateMouseCursor(v);
			break;
		case meAddingExtension:
		case meInserting:
			m_leftButtonDown = false;
			break;
		case meTranslateDialog:
		case meRotateDialog:
		case meShiftDialog:
		case meExpansionDialog:
			// do nothing.
			break;
		}


	} else if (event->button() == Qt::RightButton) {
		if (isNear(m_dragStartPoint, QPoint(event->x(), event->y()))) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void RawDataRiverSurvey::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meNormal:
	case meAddingExtension:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meTranslate:
	case meTranslatePrepare:
		v->setCursor(m_cursorMove);
		break;
	case meRotateRight:
	case meRotatePrepareRight:
	case meRotateLeft:
	case meRotatePrepareLeft:
		v->setCursor(m_cursorRotate);
		break;
	case meShift:
	case meShiftPrepare:
		v->setCursor(m_cursorShift);
		break;
	case meMoveExtentionEndPointLeft:
	case meMoveExtensionEndPointPrepareLeft:
	case meMoveExtentionEndPointRight:
	case meMoveExtensionEndPointPrepareRight:
		v->setCursor(m_cursorMove);
		break;
	case meExpansionRight:
	case meExpansionPrepareRight:
	case meExpansionLeft:
	case meExpansionPrepareLeft:
		v->setCursor(m_cursorExpand);
		break;
	}
}

void RawDataRiverSurvey::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
}

void RawDataRiverSurvey::doLoadFromProjectMainFile(const QDomNode& node)
{
	RawData::doLoadFromProjectMainFile(node);
	QDomElement elem = node.toElement();
	m_showBackground = static_cast<bool>(elem.attribute("showBackground").toInt());
	m_opacityPercent = loadOpacityPercent(node);
	m_backgroundActor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
	int linearMode = elem.attribute("interpolateLinear").toInt();
	bool lMode = (linearMode != 0);
	RiverSplineSolver::setLinearMode(lMode, m_headPoint);
	if (lMode) {
		m_interpolateSplineAction->setChecked(false);
		m_interpolateLinearAction->setChecked(true);
	}
	m_showLines = static_cast<bool>(elem.attribute("showLines").toInt());
	m_crosssectionLinesScale = elem.attribute("lineZScale", "10").toInt();
	m_crosssectionLinesColor = loadColorAttribute("lineColor", node, Qt::red);
}

void RawDataRiverSurvey::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	RawData::doSaveToProjectMainFile(writer);
	writer.writeAttribute("showBackground", QString::number(static_cast<int>(m_showBackground)));
	writeOpacityPercent(m_opacityPercent, writer);
	writer.writeAttribute("showLines", QString::number(static_cast<int>(m_showLines)));
	writer.writeAttribute("lineZScale", QString::number(static_cast<int>(m_crosssectionLinesScale)));
	writeColorAttribute("lineColor", m_crosssectionLinesColor, writer);

	int lMode = 0;
	if (RiverSplineSolver::linearMode()) {
		lMode = 1;
	}
	writer.writeAttribute("interpolateLinear", QString::number(static_cast<int>(lMode)));
}

void RawDataRiverSurvey::loadExternalData(const QString& filename)
{
	if (projectData()->version().build() >= 3607) {
		iRICLib::RiverSurvey* rs = new iRICLib::RiverSurvey();

		rs->load(iRIC::toStr(filename).c_str());
		RawDataRiverPathPoint* before = m_headPoint;
		RawDataRiverPathPoint* newPoint;
		for (int i = 0; i < rs->points.size(); ++i) {
			iRICLib::RiverPathPoint* libp = rs->points.at(i);
			newPoint = new RawDataRiverPathPoint();
			newPoint->loadFromiRICLibObject(libp);
			before->addPathPoint(newPoint);
			before = newPoint;
		}
		delete rs;
	} else {
		QFile f(filename);
		f.open(QIODevice::ReadOnly);
		QDataStream s(&f);

		RawDataRiverPathPoint* before = m_headPoint;
		RawDataRiverPathPoint* newPoint;
		bool nextExist;
		s >> nextExist;
		while (nextExist) {
			newPoint = new RawDataRiverPathPoint();
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

void RawDataRiverSurvey::saveExternalData(const QString& filename)
{
	iRICLib::RiverSurvey* rs = new iRICLib::RiverSurvey();
	bool first = true;

	iRICLib::RiverPathPoint* prevP = nullptr;
	RawDataRiverPathPoint* p = m_headPoint->nextPoint();
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

bool RawDataRiverSurvey::getValueRange(double* min, double* max)
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

QDialog* RawDataRiverSurvey::propertyDialog(QWidget* /*parent*/)
{
	return nullptr;
}

void RawDataRiverSurvey::handlePropertyDialogAccepted(QDialog* /*propDialog*/)
{

}

void RawDataRiverSurvey::updateShapeData()
{
	m_points->Reset();
	m_rightBankPoints->Reset();
	m_riverCenters->Reset();
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
	RawDataRiverPathPoint* p = m_headPoint->nextPoint();
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
	vtkVertex* nextVertex;
	vtkLine* line;
	bool firstOrLast = false;
	bool first = true;
	while (p != nullptr) {
		firstOrLast = first || (p->nextPoint() == nullptr);
		// left bank
		QVector2D leftBank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
		point[0] = leftBank.x();
		point[1] = leftBank.y();
		m_points->InsertNextPoint(point);
		++index;

		// left fixed point
		QVector2D leftFixed;
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
		nextVertex = vtkVertex::New();
		nextVertex->GetPointIds()->SetId(0, index);
		m_riverCenters->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		nextVertex->Delete();

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
		QVector2D rightFixed;
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
		QVector2D rightBank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
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

		QVector2D tmpp;
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
	m_riverCenters->SetPoints(m_points);
	m_riverCenters->Modified();
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
		double maxHeight;
		RawDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
		// calculate maxHeight.
		for (int i = 0; i < alist.count(); ++i) {
			RawDataRiverCrosssection::Altitude alt = alist[i];
			if (i == 0 || maxHeight < alt.height()) {maxHeight = alt.height();}
		}
		// now draw lines.
		QVector2D offsetDir = p->crosssectionDirection();
		iRIC::rotateVector270(offsetDir);

		double offset;
		RawDataRiverCrosssection::Altitude alt = alist[0];
		offset = (maxHeight - alt.height()) * m_crosssectionLinesScale;
		QVector2D tmpp = p->crosssectionPosition(alt.position()) + offsetDir * offset;
		points->InsertNextPoint(tmpp.x(), tmpp.y(), 0);
		++ pointNum;
		for (int i = 1; i < alist.count(); ++i) {
			RawDataRiverCrosssection::Altitude alt = alist[i];
			offset = (maxHeight - alt.height()) * m_crosssectionLinesScale;
			QVector2D tmpp = p->crosssectionPosition(alt.position()) + offsetDir * offset;
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

	emit dataUpdated();
	m_gridThread->update();
}

void RawDataRiverSurvey::updateSelectionShapeData()
{
	m_selectedCrosssections->Reset();
	m_selectedRiverCenters->Reset();
	m_selectedLeftBanks->Reset();
	m_selectedRightBanks->Reset();

	RawDataRiverPathPoint* p = m_headPoint->nextPoint();
	int index = 0;
	vtkVertex* nextVertex;
	vtkLine* line;
	while (p != nullptr) {
		// left bank
		nextVertex = vtkVertex::New();
		nextVertex->GetPointIds()->SetId(0, index);
		if (p->IsSelected) {
			m_selectedLeftBanks->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		}
		nextVertex->Delete();
		++index;

		// left fixed point
		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		if (p->IsSelected) {
			m_selectedCrosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		}
		line->Delete();

		++index;

		// river center
		nextVertex = vtkVertex::New();
		nextVertex->GetPointIds()->SetId(0, index);
		if (p->IsSelected) {
			m_selectedRiverCenters->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		}
		nextVertex->Delete();

		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		if (p->IsSelected) {
			m_selectedCrosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		}
		line->Delete();

		++index;

		// right fixed point
		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		if (p->IsSelected) {
			m_selectedCrosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		}
		line->Delete();

		++index;

		// right bank
		nextVertex = vtkVertex::New();
		nextVertex->GetPointIds()->SetId(0, index);
		if (p->IsSelected) {
			m_selectedRightBanks->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		}
		nextVertex->Delete();

		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		if (p->IsSelected) {
			m_selectedCrosssections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		}
		line->Delete();

		++index;
		p = p->nextPoint();
	}
	m_selectedLeftBanks->SetPoints(m_points);
	m_selectedLeftBanks->Modified();
	m_selectedRightBanks->SetPoints(m_points);
	m_selectedRightBanks->Modified();
	m_selectedRiverCenters->SetPoints(m_points);
	m_selectedRiverCenters->Modified();
	m_selectedCrosssections->SetPoints(m_points);
	m_selectedCrosssections->Modified();
}

void RawDataRiverSurvey::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(3);
}

void RawDataRiverSurvey::assignActionZValues(const ZDepthRange& range)
{
	double background = range.min();
	double backlines = .7 * range.min() + .3 * range.max();
	double lines = .5 * range.min() + .5 * range.max();
	double points = range.max();

	m_riverCenterActor->SetPosition(0, 0, points);
	m_selectedRiverCenterActor->SetPosition(0, 0, points);
	m_selectedLeftBankActor->SetPosition(0, 0, points);
	m_selectedRightBankActor->SetPosition(0, 0, points);
	m_firstAndLastCrosssectionsActor->SetPosition(0, 0, lines);
	m_crossectionsActor->SetPosition(0, 0, lines);
	m_riverCenterLineActor->SetPosition(0, 0, lines);
	m_leftBankLineActor->SetPosition(0, 0, lines);
	m_rightBankLineActor->SetPosition(0, 0, lines);
	m_selectedCrossectionsActor->SetPosition(0, 0, lines);
	m_blackCrossectionsActor->SetPosition(0, 0, backlines);
	m_redCrossectionsActor->SetPosition(0, 0, backlines);
	m_blueCrossectionsActor->SetPosition(0, 0, backlines);
	m_crosssectionLinesActor->SetPosition(0, 0, backlines);
	m_backgroundActor->SetPosition(0, 0, background);
}

void RawDataRiverSurvey::setupActions()
{
	m_addUpperSideAction = new QAction(tr("Insert Upstream Side(&B)..."), this);
	connect(m_addUpperSideAction, SIGNAL(triggered()), this, SLOT(insertNewPoint()));
	m_addLowerSideAction = new QAction(tr("Insert Downstream Side(&A)..."), this);
	connect(m_addLowerSideAction, SIGNAL(triggered()), this, SLOT(addNewPoint()));
	m_moveAction = new QAction(tr("&Move..."), this);
	connect(m_moveAction, SIGNAL(triggered()), this, SLOT(moveSelectedPoints()));
	m_rotateAction = new QAction(tr("&Rotate..."), this);
	connect(m_rotateAction, SIGNAL(triggered()), this, SLOT(rotateSelectedPoint()));
	m_shiftAction = new QAction(tr("S&hift Center..."), this);
	connect(m_shiftAction, SIGNAL(triggered()), this, SLOT(shiftSelectedPoints()));
	m_expandAction = new QAction(tr("E&xtend Horizontally..."), this);
	connect(m_expandAction, SIGNAL(triggered()), this, SLOT(expandSelectedPoints()));
	m_deleteAction = new QAction(tr("Dele&te Cross Section"), this);
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelectedPoints()));
	m_renameAction = new QAction(tr("R&ename Cross Section..."), this);
	connect(m_renameAction, SIGNAL(triggered()), this, SLOT(renameSelectedPoint()));
	m_addLeftExtensionPointAction = new QAction(tr("Add &Left Bank Extension Line..."), this);
	connect(m_addLeftExtensionPointAction, SIGNAL(triggered()), this, SLOT(addLeftExtensionPoint()));
	m_addRightExtensionPointAction = new QAction(tr("Add &Right Bank Extension Line..."), this);
	connect(m_addRightExtensionPointAction, SIGNAL(triggered()), this, SLOT(addRightExtensionPoint()));
	m_removeLeftExtensionPointAction = new QAction(tr("Remo&ve Left Bank Extension Line"), this);
	connect(m_removeLeftExtensionPointAction, SIGNAL(triggered()), this, SLOT(removeLeftExtensionPoint()));
	m_removeRightExtensionPointAction = new QAction(tr("Rem&ove Right Bank Extension Line"), this);
	connect(m_removeRightExtensionPointAction, SIGNAL(triggered()), this, SLOT(removeRightExtensionPoint()));
	m_openCrossSectionWindowAction = new QAction(tr("Display &Cross Section"), this);
	connect(m_openCrossSectionWindowAction, SIGNAL(triggered()), this, SLOT(openCrossSectionWindow()));
//	m_openVerticalSectionWindowAction = new QAction(tr("Display &Vertical Section"), this);
//	m_reverseAction = new QAction(tr("Reverse &KP Representation"), this);
	m_showBackgroundAction = new QAction(tr("Display &Setting"), this);
	connect(m_showBackgroundAction, SIGNAL(triggered()), this, SLOT(displaySetting()));
	m_interpolateSplineAction = new QAction(tr("Spline"), this);
	connect(m_interpolateSplineAction, SIGNAL(triggered()), this, SLOT(switchInterpolateModeToSpline()));
	m_interpolateSplineAction->setCheckable(true);
	m_interpolateSplineAction->setChecked(true);
	m_interpolateLinearAction = new QAction(tr("Linear Curve"), this);
	connect(m_interpolateLinearAction, SIGNAL(triggered()), this, SLOT(switchInterpolateModeToLinear()));
	m_interpolateLinearAction->setCheckable(true);

	m_addUpperSideAction->setEnabled(false);
	m_addLowerSideAction->setEnabled(false);
	m_moveAction->setEnabled(false);
	m_rotateAction->setEnabled(false);
	m_shiftAction->setEnabled(false);
	m_expandAction->setEnabled(false);
	m_deleteAction->setEnabled(false);
	m_renameAction->setEnabled(false);
	m_addLeftExtensionPointAction->setEnabled(false);
	m_addRightExtensionPointAction->setEnabled(false);
	m_removeLeftExtensionPointAction->setEnabled(false);
	m_removeRightExtensionPointAction->setEnabled(false);
	m_openCrossSectionWindowAction->setEnabled(false);
//	m_openVerticalSectionWindowAction->setEnabled(false);
}

void RawDataRiverSurvey::updateActionStatus()
{
	int selectCount = m_headPoint->selectedPoints();
	bool singleSelection = (selectCount == 1);
	bool selectionExists = (selectCount > 0);
	RawDataRiverPathPoint* selected = nullptr;
	if (singleSelection) {
		selected = selectedPoint();
	}
	m_addUpperSideAction->setEnabled(singleSelection);
	m_addLowerSideAction->setEnabled(singleSelection);
	m_moveAction->setEnabled(selectionExists);
	m_rotateAction->setEnabled(singleSelection);
	m_shiftAction->setEnabled(selectionExists);
	m_expandAction->setEnabled(selectionExists);
	m_deleteAction->setEnabled(selectionExists);
	m_renameAction->setEnabled(singleSelection);

	m_addLeftExtensionPointAction->setEnabled(singleSelection && ! selected->crosssection().fixedPointLSet());
	m_addRightExtensionPointAction->setEnabled(singleSelection && ! selected->crosssection().fixedPointRSet());
	m_removeLeftExtensionPointAction->setEnabled(singleSelection && selected->crosssection().fixedPointLSet());
	m_removeRightExtensionPointAction->setEnabled(singleSelection && selected->crosssection().fixedPointRSet());
	m_openCrossSectionWindowAction->setEnabled(selectionExists);
}

void RawDataRiverSurvey::moveSelectedPoints()
{
	RawDataRiverPathPointMoveDialog* dialog = new RawDataRiverPathPointMoveDialog(this, preProcessorWindow());
	m_mouseEventMode = meTranslateDialog;
	int selectCount = m_headPoint->selectedPoints();
	bool singleSelection = (selectCount == 1);
	RawDataRiverPathPoint* selected = nullptr;
	if (singleSelection) {
		selected = selectedPoint();
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

class RawDataRiverPathPointDeleteCommand : public QUndoCommand
{
public:
	RawDataRiverPathPointDeleteCommand(RawDataRiverSurvey* rs)
		: QUndoCommand(RawDataRiverSurvey::tr("Delete Traversal Lines")) {
		m_redoed = false;
		m_rs = rs;
		RawDataRiverPathPoint* p = m_rs->headPoint();
		while (p != nullptr) {
			if (p->IsSelected) {
				m_deletedPoints.append(p);
				m_beforePoints.append(p->previousPoint());
				m_prevSkips.append(p->previousPoint()->gridSkip());
				if (p->nextPoint() != nullptr) {
					m_nextSkips.append(p->nextPoint()->gridSkip());
				} else {
					m_nextSkips.append(false);
				}
			}
			p = p->nextPoint();
		}
	}
	~RawDataRiverPathPointDeleteCommand() {
		if (m_redoed) {
			// remove the points.
			for (auto it = m_deletedPoints.begin(); it != m_deletedPoints.end(); ++it) {
				delete(*it);
			}
		}
	}

	void undo() {
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_deletedPoints.count(); ++i) {
			m_beforePoints[i]->addPathPoint(m_deletedPoints.at(i));
		}
		for (int i = 0; i < m_deletedPoints.count(); ++i) {
			RawDataRiverPathPoint* p = m_deletedPoints[i];
			p->previousPoint()->setGridSkip(m_prevSkips[i]);
			if (p->nextPoint() != nullptr) {
				p->nextPoint()->setGridSkip(m_nextSkips[i]);
			}
		}
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->updateSelectionShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
		m_redoed = false;
	}
	void redo() {
		m_rs->m_gridThread->cancel();
		for (auto it = m_deletedPoints.begin(); it != m_deletedPoints.end(); ++it) {
			RawDataRiverPathPoint* p = (*it);
			p->remove();
		}
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->updateSelectionShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
		m_redoed = true;
		m_rs->m_mapped = false;
	}

private:
	QList<RawDataRiverPathPoint*> m_deletedPoints;
	QList<RawDataRiverPathPoint*> m_beforePoints;
	QList<bool> m_prevSkips;
	QList<bool> m_nextSkips;
	RawDataRiverSurvey* m_rs;
	bool m_redoed;
};

void RawDataRiverSurvey::deleteSelectedPoints()
{
	int num = 0;
	int selectedNum = 0;
	RawDataRiverPathPoint* p = m_headPoint->nextPoint();
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
		iRICUndoStack::instance().push(new RawDataRiverPathPointDeleteCommand(this));
	}
}

void RawDataRiverSurvey::shiftSelectedPoints()
{
	RawDataRiverPathPointShiftDialog* dialog = new RawDataRiverPathPointShiftDialog(this, preProcessorWindow());
	m_mouseEventMode = meShiftDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void RawDataRiverSurvey::expandSelectedPoints()
{
	RawDataRiverPathPointExpandDialog* dialog = new RawDataRiverPathPointExpandDialog(this, preProcessorWindow());
	m_mouseEventMode = meExpansionDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}


void RawDataRiverSurvey::rotateSelectedPoint()
{
	RawDataRiverPathPoint* selected = selectedPoint();
	QVector2D dir = selected->previousPoint()->position() - selected->position();
	double angle = iRIC::angle(dir, selected->crosssectionDirection());

	RawDataRiverPathPointRotateDialog* dialog = new RawDataRiverPathPointRotateDialog(this, preProcessorWindow());
	dialog->setCurrentRelativeAngle(angle);
	m_mouseEventMode = meRotateDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void RawDataRiverSurvey::renameSelectedPoint()
{
	RawDataRiverPathPoint* selected = selectedPoint();
	RawDataRiverPathPointRenameDialog dialog(selected, this, preProcessorWindow());
	dialog.exec();
}

void RawDataRiverSurvey::addLeftExtensionPoint()
{
	RawDataRiverPathPoint* selected = selectedPoint();
	RawDataRiverPathPointExtensionAddDialog* dialog = new RawDataRiverPathPointExtensionAddDialog(selected, this, preProcessorWindow());
	dialog->setLineMode(RawDataRiverPathPointExtensionAddDialog::Left);
	dialog->setPoint(selected->crosssectionPosition(selected->crosssection().leftBank(true).position()));
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	m_mouseEventMode = meAddingExtension;
	connect(graphicsView(), SIGNAL(worldPositionChanged(QVector2D)), dialog, SLOT(setPoint(QVector2D)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void RawDataRiverSurvey::addRightExtensionPoint()
{
	RawDataRiverPathPoint* selected = selectedPoint();
	RawDataRiverPathPointExtensionAddDialog* dialog = new RawDataRiverPathPointExtensionAddDialog(selected, this, preProcessorWindow());
	dialog->setLineMode(RawDataRiverPathPointExtensionAddDialog::Right);
	dialog->setPoint(selected->crosssectionPosition(selected->crosssection().rightBank(true).position()));
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	m_mouseEventMode = meAddingExtension;
	connect(graphicsView(), SIGNAL(worldPositionChanged(QVector2D)), dialog, SLOT(setPoint(QVector2D)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

class RawDataRiverPathPointRemoveExtensionCommand : public QUndoCommand
{
public:
	RawDataRiverPathPointRemoveExtensionCommand(bool left, const QVector2D& pos, RawDataRiverPathPoint* p, RawDataRiverSurvey* rs)
		: QUndoCommand(RawDataRiverSurvey::tr("Remove Extension Line")) {
		m_left = left;
		m_position = pos;
		m_point = p;
		m_rs = rs;
	}
	void undo() {
		m_rs->m_gridThread->cancel();
		if (m_left) {
			m_point->addExtentionPointLeft(m_position);
		} else {
			m_point->addExtentionPointRight(m_position);
		}
		m_rs->updateActionStatus();
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	void redo() {
		m_rs->m_gridThread->cancel();
		if (m_left) {
			m_point->removeExtentionPointLeft();
		} else {
			m_point->removeExtentionPointRight();
		}
		m_rs->updateActionStatus();
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
		m_rs->m_mapped = false;
	}
private:
	RawDataRiverPathPoint* m_point;
	RawDataRiverSurvey* m_rs;
	bool m_left;
	QVector2D m_position;
};

void RawDataRiverSurvey::removeLeftExtensionPoint()
{
	RawDataRiverPathPoint* selected = selectedPoint();
	iRICUndoStack::instance().push(new RawDataRiverPathPointRemoveExtensionCommand(true, selected->leftBank()->interpolate(0), selected, this));
}

void RawDataRiverSurvey::removeRightExtensionPoint()
{
	RawDataRiverPathPoint* selected = selectedPoint();
	iRICUndoStack::instance().push(new RawDataRiverPathPointRemoveExtensionCommand(false, selected->rightBank()->interpolate(0), selected, this));
}

void RawDataRiverSurvey::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

void RawDataRiverSurvey::setupCursors()
{
	m_pixmapMove = QPixmap(":/libs/guibase/images/cursorItemMove.png");
	m_cursorMove = QCursor(m_pixmapMove, 7, 2);

	m_pixmapRotate = QPixmap(":/libs/rawdata/riversurvey/images/cursorCrosssectionRotate.png");
	m_cursorRotate = QCursor(m_pixmapRotate, 7, 2);

	m_pixmapExpand = QPixmap(":/libs/rawdata/riversurvey/images/cursorExpand.png");
	m_cursorExpand = QCursor(m_pixmapExpand, 7, 2);

	m_pixmapShift = QPixmap(":/libs/rawdata/riversurvey/images/cursorShiftCenter.png");
	m_cursorShift = QCursor(m_pixmapShift, 7, 2);
}

void RawDataRiverSurvey::insertNewPoint()
{
	RawDataRiverPathPoint* selected = selectedPoint();
	RawDataRiverPathPointInsertDialog* dialog = new RawDataRiverPathPointInsertDialog(selected, true, this, preProcessorWindow());
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	m_mouseEventMode = meInserting;

	connect(graphicsView(), SIGNAL(worldPositionChanged(QVector2D)), dialog, SLOT(setPoint(QVector2D)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void RawDataRiverSurvey::addNewPoint()
{
	RawDataRiverPathPoint* selected = selectedPoint();
	RawDataRiverPathPointInsertDialog* dialog = new RawDataRiverPathPointInsertDialog(selected, false, this, preProcessorWindow());
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	m_mouseEventMode = meInserting;

	double val = selected->name().toDouble();
	if (val == 0) {
		QMessageBox::warning(dataModel()->mainWindow(), tr("Warning"), tr("Insert Target Point name is \"0\". You have to change the name if you want to add a new point."));
		return;
	}
	connect(graphicsView(), SIGNAL(worldPositionChanged(QVector2D)), dialog, SLOT(setPoint(QVector2D)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void RawDataRiverSurvey::updateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QVector2D worldPos(dx, dy);
	double stdLen = graphicsView()->stdRadius(1);
	double stdLen2 = stdLen * stdLen;
	int selectCount = m_headPoint->selectedPoints();
	RawDataRiverPathPoint* selected = nullptr;
	if (selectCount == 1) {
		selected = m_headPoint->nextPoint();
		while (1) {
			if (selected->IsSelected) {break;}
			selected = selected->nextPoint();
		}
		// only one point is selected.
		if ((selected->position() - worldPos).lengthSquared() < stdLen2 * 9) {
			// cursor is near to the river center point
			if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
					(m_keyboardModifiers & Qt::ControlModifier) == 0) {
				// preparing for moving
				m_mouseEventMode = meTranslatePrepare;
			}
			if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
					(m_keyboardModifiers & Qt::ControlModifier) == 0) {
				// preparing for center-point shift
				m_mouseEventMode = meShiftPrepare;
			}
		} else {
			m_mouseEventMode = meNormal;
			QVector2D lbank = selected->crosssectionPosition(selected->crosssection().leftBank(true).position());
			if ((lbank - worldPos).lengthSquared() < stdLen2 * 9) {
				// cursor is near left bank.
				if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0) {
					// preparing for rotating
					m_mouseEventMode = meRotatePrepareLeft;
				}
				if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0 &&
						selected->crosssection().fixedPointLSet()) {
					// preparing for center-point shift
					m_mouseEventMode = meMoveExtensionEndPointPrepareLeft;
				}
			}
			QVector2D rbank = selected->crosssectionPosition(selected->crosssection().rightBank(true).position());
			if ((rbank - worldPos).lengthSquared() < stdLen2 * 9) {
				// cursor is near right bank.
				if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0) {
					// preparing for rotating
					m_mouseEventMode = meRotatePrepareRight;
				}
				if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0 &&
						selected->crosssection().fixedPointRSet()) {
					// preparing for center-point shift
					m_mouseEventMode = meMoveExtensionEndPointPrepareRight;
				}
			}
		}
	} else {
		// multiple selection, or none.
		RawDataRiverPathPoint* p = m_headPoint->nextPoint();
		m_mouseEventMode = meNormal;
		while (p != nullptr) {
			if (p->IsSelected) {
				if ((p->position() - worldPos).lengthSquared() < stdLen2 * 9) {
					// cursor is near to the river center point
					if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
							(m_keyboardModifiers & Qt::ControlModifier) == 0) {
						// preparing for moving
						m_mouseEventMode = meTranslatePrepare;
						return;
					}
					if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
							(m_keyboardModifiers & Qt::ControlModifier) == 0) {
						// preparing for center-point shift
						m_mouseEventMode = meShiftPrepare;
						return;
					}
				} else {
					m_mouseEventMode = meNormal;
					QVector2D lbank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
					if ((lbank - worldPos).lengthSquared() < stdLen2 * 9) {
						// cursor is near left bank.
						if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
								(m_keyboardModifiers & Qt::ControlModifier) == 0) {
							// preparing for center-point shift
							m_mouseEventMode = meExpansionPrepareLeft;
							return;
						}
					}
					QVector2D rbank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
					if ((rbank - worldPos).lengthSquared() < stdLen2 * 9) {
						// cursor is near right bank.
						if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
								(m_keyboardModifiers & Qt::ControlModifier) == 0) {
							// preparing for center-point shift
							m_mouseEventMode = meExpansionPrepareRight;
							return;
						}
					}
				}
			}
			p = p->nextPoint();
		}
	}
}


RawDataRiverPathPoint* RawDataRiverSurvey::selectedPoint()
{
	RawDataRiverPathPoint* p = m_headPoint->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {return p;}
		p = p->nextPoint();
	}
	return nullptr;
}

void RawDataRiverSurvey::updateSplineSolvers()
{
	m_CenterLineSolver.update();
	m_LeftBankSolver.update();
	m_RightBankSolver.update();
}

void RawDataRiverSurvey::updateBackgroundGrid()
{
	m_backgroundGrid = m_gridThread->grid();
	m_backgroundGrid->GetPointData()->SetActiveScalars("Data");

	RawDataRiverPathPoint* p = m_headPoint->nextPoint();
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
	if (isVisible() && m_showBackground) {
		m_backgroundActor->VisibilityOn();
	}
	dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent())->informValueRangeChange();
}

void RawDataRiverSurvey::updateInterpolators()
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
	RawDataRiverPathPoint* tmpp = m_headPoint->nextPoint();
	while (tmpp != nullptr) {
		tmpp->updateXSecInterpolators();
		tmpp = tmpp->nextPoint();
	}
	updateShapeData();
}

void RawDataRiverSurvey::openCrossSectionWindow()
{
	PreProcessorRawDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
	gItem->openCrossSectionWindow(this, selectedPoint()->name().toDouble());
}

void RawDataRiverSurvey::updateCrossectionWindows()
{
	PreProcessorRawDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
	gItem->updateCrossectionWindows();
}

void RawDataRiverSurvey::toggleCrosssectionWindowsGridCreatingMode(bool gridMode)
{
	PreProcessorRawDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
	gItem->toggleCrosssectionWindowsGridCreatingMode(gridMode, this);
}

void RawDataRiverSurvey::informCtrlPointUpdateToCrosssectionWindows()
{
	PreProcessorRawDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
	gItem->informCtrlPointUpdateToCrosssectionWindows();
}

class RawDataRiverSurveyDisplaySettingCommand : public QUndoCommand
{
public:
	RawDataRiverSurveyDisplaySettingCommand(bool bgvisible, int opacityP, bool linevisible, QColor col, int scale, RawDataRiverSurvey* s) {
		m_survey = s;
		m_newBgVisible = bgvisible;
		m_newOpacityPercent = opacityP;
		m_newLinesVisible = linevisible;
		m_newLineColor = col;
		m_newZScale = scale;

		m_oldBgVisible = m_survey->m_backgroundActor->GetVisibility();
		m_oldOpacityPercent = m_survey->m_opacityPercent;
		m_oldLinesVisible = m_survey->m_showLines;
		m_oldLineColor = m_survey->m_crosssectionLinesColor;
		m_oldZScale = m_survey->m_crosssectionLinesScale;
	}
	void redo() {
		m_survey->m_backgroundActor->SetVisibility(m_newBgVisible);
		m_survey->m_backgroundActor->GetProperty()->SetOpacity(m_newOpacityPercent / 100.);
		m_survey->m_showBackground = m_newBgVisible;
		m_survey->m_opacityPercent = m_newOpacityPercent;
		m_survey->m_showLines = m_newLinesVisible;
		m_survey->m_crosssectionLinesColor = m_newLineColor;
		m_survey->m_crosssectionLinesActor->SetVisibility(m_newLinesVisible);
		m_survey->m_crosssectionLinesActor->GetProperty()->SetColor(m_newLineColor.redF(), m_newLineColor.greenF(), m_newLineColor.blueF());
		m_survey->m_crosssectionLinesScale = m_newZScale;

		vtkActorCollection* ac = m_survey->actorCollection();
		if (m_newBgVisible) {
			ac->AddItem(m_survey->m_backgroundActor);
		} else {
			ac->RemoveItem(m_survey->m_backgroundActor);
		}
		if (m_newLinesVisible) {
			ac->AddItem(m_survey->m_crosssectionLinesActor);
		} else {
			ac->RemoveItem(m_survey->m_crosssectionLinesActor);
		}
		m_survey->updateShapeData();
		m_survey->renderGraphicsView();
	}
	void undo() {
		m_survey->m_backgroundActor->SetVisibility(m_oldBgVisible);
		m_survey->m_backgroundActor->GetProperty()->SetOpacity(m_oldOpacityPercent / 100.);
		m_survey->m_showBackground = m_oldBgVisible;
		m_survey->m_opacityPercent = m_oldOpacityPercent;
		m_survey->m_showLines = m_oldLinesVisible;
		m_survey->m_crosssectionLinesColor = m_oldLineColor;
		m_survey->m_crosssectionLinesActor->SetVisibility(m_oldLinesVisible);
		m_survey->m_crosssectionLinesActor->GetProperty()->SetColor(m_oldLineColor.redF(), m_oldLineColor.greenF(), m_oldLineColor.blueF());
		m_survey->m_crosssectionLinesScale = m_oldZScale;

		vtkActorCollection* ac = m_survey->actorCollection();
		if (m_oldBgVisible) {
			ac->AddItem(m_survey->m_backgroundActor);
		} else {
			ac->RemoveItem(m_survey->m_backgroundActor);
		}
		if (m_oldLinesVisible) {
			ac->AddItem(m_survey->m_crosssectionLinesActor);
		} else {
			ac->RemoveItem(m_survey->m_crosssectionLinesActor);
		}
		m_survey->updateShapeData();
		m_survey->renderGraphicsView();
	}
private:
	bool m_oldBgVisible;
	int m_oldOpacityPercent;
	bool m_oldLinesVisible;
	QColor m_oldLineColor;
	int m_oldZScale;

	bool m_newBgVisible;
	int m_newOpacityPercent;
	bool m_newLinesVisible;
	QColor m_newLineColor;
	int m_newZScale;

	RawDataRiverSurvey* m_survey;
};

void RawDataRiverSurvey::displaySetting()
{
	RawDataRiverSurveyDisplaySettingDialog dialog(preProcessorWindow());
	dialog.setColormapVisible(m_showBackground);
	dialog.setOpacityPercent(m_opacityPercent);
	dialog.setLinesVisible(m_showLines);
	dialog.setLineColor(m_crosssectionLinesColor);
	dialog.setZScale(m_crosssectionLinesScale);

	int ret = dialog.exec();
	if (ret != dialog.Accepted) {return;}
	iRICUndoStack::instance().push(new RawDataRiverSurveyDisplaySettingCommand(dialog.colormapVisible(), dialog.opacityPercent(), dialog.linesVisible(), dialog.lineColor(), dialog.zScale(), this));
}

void RawDataRiverSurvey::switchInterpolateModeToLinear()
{
	RiverSplineSolver::setLinearMode(true, m_headPoint);
	m_interpolateLinearAction->setChecked(true);
	m_interpolateSplineAction->setChecked(false);
	updateInterpolators();
}

void RawDataRiverSurvey::switchInterpolateModeToSpline()
{
	RiverSplineSolver::setLinearMode(false, m_headPoint);
	m_interpolateLinearAction->setChecked(false);
	m_interpolateSplineAction->setChecked(true);
	updateInterpolators();
}

/*
void RawDataRiverSurvey::updateBackgroundVisibility(bool visible)
{
	m_showBackground = visible;
	m_backgroundActor->SetVisibility(visible);
	if (m_showBackground){
		actorCollection()->AddItem(m_backgroundActor);
	}else{
		actorCollection()->RemoveItem(m_backgroundActor);
	}
	updateVisibility();
}
*/

void RawDataRiverSurvey::setColoredPoints(RawDataRiverPathPoint* black, RawDataRiverPathPoint* red, RawDataRiverPathPoint* blue)
{
	if (black == nullptr) {
		m_blackCrossectionsActor->VisibilityOff();
	} else {
		setupLine(m_blackCrosssection, black);
		m_blackCrossectionsActor->VisibilityOn();
	}
	if (red == nullptr) {
		m_redCrossectionsActor->VisibilityOff();
	} else {
		setupLine(m_redCrosssection, red);
		m_redCrossectionsActor->VisibilityOn();
	}
	if (blue == nullptr) {
		m_blueCrossectionsActor->VisibilityOff();
	} else {
		setupLine(m_blueCrosssection, blue);
		m_blueCrossectionsActor->VisibilityOn();
	}
	renderGraphicsView();
}

void RawDataRiverSurvey::setupLine(vtkUnstructuredGrid* grid, RawDataRiverPathPoint* p)
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
	QVector2D leftBank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
	point[0] = leftBank.x();
	point[1] = leftBank.y();
	points->InsertNextPoint(point);

	// left fixed point
	QVector2D leftFixed;
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
	QVector2D rightFixed;
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
	QVector2D rightBank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
	point[0] = rightBank.x();
	point[1] = rightBank.y();
	points->InsertNextPoint(point);

	// line betwen right fixed point and right bank
	line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 3);
	line->GetPointIds()->SetId(1, 4);
	grid->InsertNextCell(line->GetCellType(), line->GetPointIds());
}

void RawDataRiverSurvey::useDivisionPointsForBackgroundGrid(bool use)
{
	m_gridThread->setUseDivisionPoints(use);
	m_gridThread->update();
}

void RawDataRiverSurvey::refreshBackgroundGrid()
{
	m_gridThread->update();
}

void RawDataRiverSurvey::cancelBackgroundGridUpdate()
{
	m_gridThread->cancel();
}

RawDataProxy* RawDataRiverSurvey::getProxy()
{
	return new RawDataRiverSurveyProxy(this);
}

void RawDataRiverSurvey::doApplyOffset(double x, double y)
{
	RawDataRiverPathPoint* p = this->m_headPoint->nextPoint();
	while (p != nullptr) {
		QVector2D v = p->position();
		v.setX(v.x() - x);
		v.setY(v.y() - y);
		p->setPosition(v);
		p = p->nextPoint();
	}
	this->headPoint()->updateRiverShapeInterpolators();
	this->updateShapeData();
}
