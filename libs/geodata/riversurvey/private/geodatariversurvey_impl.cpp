#include "geodatariversurvey_impl.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

#include <geoio/polylineio.h>
#include <guibase/coordinates/coordinatesedit.h>
#include <guibase/polyline/polylinecontrollerutil.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>

#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkCellArray.h>
#include <vtkDataSetMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStringArray.h>
#include <vtkTextProperty.h>

#include <QAction>
#include <QMessageBox>
#include <QMenu>

namespace {

const int SPLINE_FACTOR = 10;
const int POINTSIZE = 5;
const int FONTSIZE = 17;

void addPoint(vtkPoints* points, const QPointF& p)
{
	points->InsertNextPoint(p.x(), p.y(), 0);
}

void setupLabelActor(vtkLabel2DActor* actor)
{
	actor->setLabelPosition(vtkLabel2DActor::lpMiddleLeft);
	auto prop = actor->labelTextProperty();
	prop->SetFontSize(FONTSIZE);
	prop->BoldOn();
}

} // namespace

GeoDataRiverSurvey::Impl::Impl(GeoDataRiverSurvey* rs) :
	m_mode {Mode::CreateMode},
	m_createMouseEventMode {CreateMouseEventMode::BeforeDefining},
	m_editMouseEventMode {EditMouseEventMode::Normal},
	m_activePoints {&m_centerLineController},
	m_activeLine {&m_centerLineSplineController},
	m_previousLeftBankDistance {10},
	m_previousRightBankDistance {10},
	m_pointPoints {vtkPoints::New()},
	m_riverCenterPoints {vtkPolyData::New()},
	m_riverCenterPointsActor {vtkActor::New()},
	m_crossSectionLines {vtkPolyData::New()},
	m_crossSectionLinesActor {vtkActor::New()},
	m_centerAndBankLines {vtkPolyData::New()},
	m_centerAndBankLinesActor {vtkActor::New()},
	m_selectedRiverCenterPoints {vtkPolyData::New()},
	m_selectedRiverCenterPointsActor {vtkActor::New()},
	m_selectedLeftBankPoints {vtkPolyData::New()},
	m_selectedLeftBankPointsActor {vtkActor::New()},
	m_selectedRightBankPoints {vtkPolyData::New()},
	m_selectedRightBankPointsActor {vtkActor::New()},
	m_selectedCrossSectionLines {vtkPolyData::New()},
	m_selectedCrossSectionLinesActor {vtkActor::New()},
	m_verticalCrossSectionLines {vtkPolyData::New()},
	m_verticalCrossSectionLinesActor {vtkActor::New()},
	m_blackCrossSection {vtkPolyData::New()},
	m_blackCrossSectionActor {vtkActor::New()},
	m_rightBankPoints {vtkPoints::New()},
	m_rightBankPointSet {vtkUnstructuredGrid::New()},
	m_labelArray {vtkStringArray::New()},
	m_labelMapper {vtkLabeledDataMapper::New()},
	m_labelActor {vtkActor2D::New()},
	m_backgroundGrid {vtkSmartPointer<vtkStructuredGrid>::New()},
	m_backgroundActor {vtkActor::New()},
	m_rightClickingMenu {nullptr},
	m_menuIsSetup {false},
	m_generateAction {new QAction(GeoDataRiverSurvey::tr("Generate River Survey data"), rs)},
	m_buildBankLinesAction {new QAction(GeoDataRiverSurvey::tr("Build Left bank and Right bank lines"), rs)},
	m_addVertexAction {new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), GeoDataRiverSurvey::tr("&Add Vertex"), rs)},
	m_removeVertexAction {new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), GeoDataRiverSurvey::tr("&Remove Vertex"), rs)},
	m_importCenterLineAction {new QAction(QIcon(":/libs/guibase/images/iconImport.png"), GeoDataRiverSurvey::tr("Import C&enter Line..."), rs)},
	m_exportCenterLineAction {new QAction(QIcon(":/libs/guibase/images/iconExport.png"), GeoDataRiverSurvey::tr("Export Ce&nter Line..."), rs)},
	m_addUpperSideAction {new QAction(GeoDataRiverSurvey::tr("Insert Upstream Side(&B)..."), rs)},
	m_addLowerSideAction {new QAction(GeoDataRiverSurvey::tr("Insert Downstream Side(&A)..."), rs)},
	m_moveAction {new QAction(GeoDataRiverSurvey::tr("&Move..."), rs)},
	m_rotateAction {new QAction(GeoDataRiverSurvey::tr("&Rotate..."), rs)},
	m_shiftAction {new QAction(GeoDataRiverSurvey::tr("S&hift Center..."), rs)},
	m_expandAction {new QAction(GeoDataRiverSurvey::tr("E&xtend Horizontally..."), rs)},
	m_deleteAction {new QAction(GeoDataRiverSurvey::tr("Dele&te Cross Section"), rs)},
	m_renameAction{ new QAction(GeoDataRiverSurvey::tr("R&ename Cross Section..."), rs)},
	m_addLeftExtensionPointAction {new QAction(GeoDataRiverSurvey::tr("Add &Left Bank Extension Line..."), rs)},
	m_addRightExtensionPointAction {new QAction(GeoDataRiverSurvey::tr("Add &Right Bank Extension Line..."), rs)},
	m_removeLeftExtensionPointAction {new QAction(GeoDataRiverSurvey::tr("Remo&ve Left Bank Extension Line"), rs)},
	m_removeRightExtensionPointAction {new QAction(GeoDataRiverSurvey::tr("Rem&ove Right Bank Extension Line"), rs)},
	m_openCrossSectionWindowAction {new QAction(GeoDataRiverSurvey::tr("Display &Cross Section"), rs)},
	m_showBackgroundAction {new QAction(GeoDataRiverSurvey::tr("Display &Setting"), rs)},
	m_interpolateSplineAction {new QAction(GeoDataRiverSurvey::tr("Spline"), rs)},
	m_interpolateLinearAction {new QAction(GeoDataRiverSurvey::tr("Linear Curve"), rs)},
	m_mapPointsAction {new QAction(GeoDataRiverSurvey::tr("Map points data"), rs)},
	m_pixmapAdd {":/libs/guibase/images/cursorAdd.png"},
	m_pixmapRemove {":/libs/guibase/images/cursorRemove.png"},
	m_pixmapMove {":/libs/guibase/images/cursorItemMove.png"},
	m_pixmapRotate {":/libs/geodata/riversurvey/images/cursorCrosssectionRotate.png"},
	m_pixmapExpand {":/libs/geodata/riversurvey/images/cursorExpand.png"},
	m_pixmapShift {":/libs/geodata/riversurvey/images/cursorShiftCenter.png"},
	m_cursorAdd {m_pixmapAdd, 0, 0},
	m_cursorRemove {m_pixmapRemove, 0, 0},
	m_cursorMove {m_pixmapMove, 7, 2},
	m_cursorRotate {m_pixmapRotate, 7, 2},
	m_cursorExpand {m_pixmapExpand, 7, 2},
	m_cursorShift {m_pixmapShift, 7, 2},
	m_definingBoundingBox {false},
	m_leftButtonDown {false},
	m_gridCreatingCondition {nullptr},
	m_gridThread {new GeoDataRiverSurveyBackgroundGridCreateThread(rs)},
	m_rs {rs}
{
	m_rightBankPoints->SetDataTypeToDouble();
	m_labelArray->SetName("Label");

	m_addVertexAction->setCheckable(true);
	m_removeVertexAction->setCheckable(true);

	m_centerLineController.pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
	m_leftBankLineController.pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
	m_rightBankLineController.pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
}

GeoDataRiverSurvey::Impl::~Impl()
{
	auto r = m_rs->renderer();

	r->RemoveActor2D(m_upstreamActor.actor());
	r->RemoveActor2D(m_downstreamActor.actor());

	r->RemoveActor(m_centerLineSplineController.linesActor());
	r->RemoveActor(m_centerLineController.pointsActor());
	r->RemoveActor(m_leftBankLineSplineController.linesActor());
	r->RemoveActor(m_leftBankLineController.pointsActor());
	r->RemoveActor(m_rightBankLineSplineController.linesActor());
	r->RemoveActor(m_rightBankLineController.pointsActor());
	r->RemoveActor(m_upstreamLineController.linesActor());
	r->RemoveActor(m_downstreamLineController.linesActor());

	r->RemoveActor(m_riverCenterPointsActor);
	r->RemoveActor(m_crossSectionLinesActor);
	r->RemoveActor(m_centerAndBankLinesActor);
	r->RemoveActor(m_selectedRiverCenterPointsActor);
	r->RemoveActor(m_selectedLeftBankPointsActor);
	r->RemoveActor(m_selectedRightBankPointsActor);
	r->RemoveActor(m_selectedCrossSectionLinesActor);
	r->RemoveActor(m_verticalCrossSectionLinesActor);
	r->RemoveActor(m_blackCrossSectionActor);
	r->RemoveActor(m_labelActor);
	r->RemoveActor(m_backgroundActor);

	m_pointPoints->Delete();
	m_riverCenterPoints->Delete();
	m_riverCenterPointsActor->Delete();
	m_crossSectionLines->Delete();
	m_crossSectionLinesActor->Delete();
	m_centerAndBankLines->Delete();
	m_centerAndBankLinesActor->Delete();
	m_selectedRiverCenterPoints->Delete();
	m_selectedRiverCenterPointsActor->Delete();
	m_selectedLeftBankPoints->Delete();
	m_selectedLeftBankPointsActor->Delete();
	m_selectedRightBankPoints->Delete();
	m_selectedRightBankPointsActor->Delete();
	m_selectedCrossSectionLines->Delete();
	m_selectedCrossSectionLinesActor->Delete();
	m_verticalCrossSectionLines->Delete();
	m_verticalCrossSectionLinesActor->Delete();
	m_blackCrossSection->Delete();
	m_blackCrossSectionActor->Delete();
	m_rightBankPoints->Delete();
	m_rightBankPointSet->Delete();
	m_labelArray->Delete();
	m_labelMapper->Delete();
	m_labelActor->Delete();
	m_backgroundActor->Delete();

	delete m_rightClickingMenu;
	delete m_gridThread;
}

void GeoDataRiverSurvey::Impl::setupActions()
{
	connect(m_generateAction, SIGNAL(triggered()), m_rs, SLOT(generateData()));
	connect(m_buildBankLinesAction, SIGNAL(triggered()), m_rs, SLOT(buildBankLines()));
	connect(m_addVertexAction, SIGNAL(triggered(bool)), m_rs, SLOT(addVertexMode(bool)));
	connect(m_removeVertexAction, SIGNAL(triggered(bool)), m_rs, SLOT(removeVertexMode(bool)));
	connect(m_importCenterLineAction, SIGNAL(triggered()), m_rs, SLOT(importCenterLine()));
	connect(m_exportCenterLineAction, SIGNAL(triggered()), m_rs, SLOT(exportCenterLine()));
	connect(m_addUpperSideAction, SIGNAL(triggered()), m_rs, SLOT(insertNewPoint()));
	connect(m_addLowerSideAction, SIGNAL(triggered()), m_rs, SLOT(addNewPoint()));
	connect(m_moveAction, SIGNAL(triggered()), m_rs, SLOT(moveSelectedPoints()));
	connect(m_rotateAction, SIGNAL(triggered()), m_rs, SLOT(rotateSelectedPoint()));
	connect(m_shiftAction, SIGNAL(triggered()), m_rs, SLOT(shiftSelectedPoints()));
	connect(m_expandAction, SIGNAL(triggered()), m_rs, SLOT(expandSelectedPoints()));
	connect(m_deleteAction, SIGNAL(triggered()), m_rs, SLOT(deleteSelectedPoints()));
	connect(m_renameAction, SIGNAL(triggered()), m_rs, SLOT(renameSelectedPoint()));
	connect(m_addLeftExtensionPointAction, SIGNAL(triggered()), m_rs, SLOT(addLeftExtensionPoint()));
	connect(m_addRightExtensionPointAction, SIGNAL(triggered()), m_rs, SLOT(addRightExtensionPoint()));
	connect(m_removeLeftExtensionPointAction, SIGNAL(triggered()), m_rs, SLOT(removeLeftExtensionPoint()));
	connect(m_removeRightExtensionPointAction, SIGNAL(triggered()), m_rs, SLOT(removeRightExtensionPoint()));
	connect(m_openCrossSectionWindowAction, SIGNAL(triggered()), m_rs, SLOT(openCrossSectionWindow()));
	connect(m_showBackgroundAction, SIGNAL(triggered()), m_rs, SLOT(displaySetting()));
	connect(m_interpolateSplineAction, SIGNAL(triggered()), m_rs, SLOT(switchInterpolateModeToSpline()));
	connect(m_mapPointsAction, SIGNAL(triggered()), m_rs, SLOT(mapPointsData()));
	m_interpolateSplineAction->setCheckable(true);
	m_interpolateSplineAction->setChecked(true);
	connect(m_interpolateLinearAction, SIGNAL(triggered()), m_rs, SLOT(switchInterpolateModeToLinear()));
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
}

void GeoDataRiverSurvey::Impl::setupVtkObjects()
{
	auto r = m_rs->renderer();

	r->AddActor2D(m_upstreamActor.actor());
	r->AddActor2D(m_downstreamActor.actor());
	r->AddActor(m_centerLineSplineController.linesActor());
	r->AddActor(m_centerLineController.pointsActor());
	r->AddActor(m_leftBankLineSplineController.linesActor());
	r->AddActor(m_leftBankLineController.pointsActor());
	r->AddActor(m_rightBankLineSplineController.linesActor());
	r->AddActor(m_rightBankLineController.pointsActor());
	r->AddActor(m_upstreamLineController.linesActor());
	r->AddActor(m_downstreamLineController.linesActor());

	m_upstreamActor.setLabel("Upstream");
	setupLabelActor(&m_upstreamActor);
	m_downstreamActor.setLabel("Downstream");
	setupLabelActor(&m_downstreamActor);
	m_upstreamActor.actor()->VisibilityOff();
	m_downstreamActor.actor()->VisibilityOff();

	// river center points
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_riverCenterPoints);
	m_riverCenterPointsActor->SetMapper(mapper);
	auto prop = m_riverCenterPointsActor->GetProperty();
	prop->SetPointSize(5);
	prop->SetColor(0, 0, 1);
	m_riverCenterPointsActor->VisibilityOff();
	r->AddActor(m_riverCenterPointsActor);

	// cross section lines
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_crossSectionLines);
	m_crossSectionLinesActor->SetMapper(mapper);
	prop = m_crossSectionLinesActor->GetProperty();
	prop->SetLineWidth(1);
	prop->SetColor(0, 0, 0);
	m_crossSectionLinesActor->VisibilityOff();
	r->AddActor(m_crossSectionLinesActor);

	// center and bank lines
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_centerAndBankLines);
	m_centerAndBankLinesActor->SetMapper(mapper);
	prop = m_centerAndBankLinesActor->GetProperty();
	prop->SetLineWidth(1);
	prop->SetColor(0, 0, 0);
	m_centerAndBankLinesActor->VisibilityOff();
	r->AddActor(m_centerAndBankLinesActor);

	// selected river center points
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_selectedRiverCenterPoints);
	m_selectedRiverCenterPointsActor->SetMapper(mapper);
	prop = m_selectedRiverCenterPointsActor->GetProperty();
	prop->SetPointSize(9);
	prop->SetColor(0, 0, 1);
	m_selectedRiverCenterPointsActor->VisibilityOff();
	r->AddActor(m_selectedRiverCenterPointsActor);

	// selected left bank points
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_selectedLeftBankPoints);
	m_selectedLeftBankPointsActor->SetMapper(mapper);
	prop = m_selectedLeftBankPointsActor->GetProperty();
	prop->SetPointSize(5);
	prop->SetColor(1, 0, 0);
	m_selectedLeftBankPointsActor->VisibilityOff();
	r->AddActor(m_selectedLeftBankPointsActor);

	// selected right bank points
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_selectedRightBankPoints);
	m_selectedRightBankPointsActor->SetMapper(mapper);
	prop = m_selectedRightBankPointsActor->GetProperty();
	prop->SetPointSize(5);
	prop->SetColor(0, 1, 0);
	m_selectedRightBankPointsActor->VisibilityOff();
	r->AddActor(m_selectedRightBankPointsActor);

	// selected cross section lines
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_selectedCrossSectionLines);
	m_selectedCrossSectionLinesActor->SetMapper(mapper);
	prop = m_selectedCrossSectionLinesActor->GetProperty();
	prop->SetLineWidth(3);
	prop->SetColor(0, 0, 0);
	m_selectedCrossSectionLinesActor->VisibilityOff();
	r->AddActor(m_selectedCrossSectionLinesActor);

	// vertical cross section lines
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_verticalCrossSectionLines);
	m_verticalCrossSectionLinesActor->SetMapper(mapper);
	prop = m_verticalCrossSectionLinesActor->GetProperty();
	prop->SetColor(0, 0, 0);
	prop->SetLineWidth(1);
	m_verticalCrossSectionLinesActor->VisibilityOff();
	r->AddActor(m_verticalCrossSectionLinesActor);

	// black cross section
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_blackCrossSection);
	m_blackCrossSectionActor->SetMapper(mapper);
	prop = m_blackCrossSectionActor->GetProperty();
	prop->SetColor(0, 0, 0);
	prop->SetLineWidth(7);
	prop->SetOpacity(0.3);
	m_blackCrossSectionActor->VisibilityOff();
	r->AddActor(m_blackCrossSectionActor);

	// name label
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

	m_labelActor->SetMapper(m_labelMapper);
	r->AddActor(m_labelActor);

	// background color
	auto dsmapper = vtkSmartPointer<vtkDataSetMapper>::New();
	dsmapper->SetInputData(m_backgroundGrid);
	dsmapper->SetScalarModeToUsePointData();
	dsmapper->SetLookupTable(m_rs->scalarsToColorsContainer()->vtkObj());
	dsmapper->UseLookupTableScalarRangeOn();
	dsmapper->SetScalarVisibility(true);

	m_backgroundActor->SetMapper(dsmapper);
	m_backgroundActor->VisibilityOff();
	r->AddActor(m_backgroundActor);
}

void GeoDataRiverSurvey::Impl::setupMenu()
{
	if (m_menuIsSetup) {return;}

	auto m = m_rs->m_menu;
	m->clear();
	delete m_rightClickingMenu;
	m_rightClickingMenu = new QMenu();

	m->setTitle(tr("&River Survey"));
	m->addAction(m_rs->m_editNameAction);

	if (m_mode == Mode::CreateMode) {
		m->addSeparator();
		setupCreateModeMenu(m);
		setupCreateModeMenu(m_rightClickingMenu);
	} else if (m_mode == Mode::EditMode) {
		m->addSeparator();
		setupEditModeMenu(m);
		setupEditModeMenu(m_rightClickingMenu);
	}
	m_menuIsSetup = true;
}

void GeoDataRiverSurvey::Impl::setupCreateModeMenu(QMenu* m)
{
	m->addAction(m_generateAction);

	m->addSeparator();

	m->addAction(m_buildBankLinesAction);

	m->addSeparator();

	m->addAction(m_addVertexAction);
	m->addAction(m_removeVertexAction);

	m->addSeparator();

	m->addAction(m_importCenterLineAction);
	m->addAction(m_exportCenterLineAction);
}

void GeoDataRiverSurvey::Impl::setupEditModeMenu(QMenu* m)
{
	m->addAction(m_openCrossSectionWindowAction);

	m->addSeparator();
	m->addAction(m_addUpperSideAction);
	m->addAction(m_addLowerSideAction);
	m->addAction(m_moveAction);
	m->addAction(m_rotateAction);
	m->addAction(m_shiftAction);
	m->addAction(m_expandAction);
	m->addAction(m_deleteAction);
	m->addAction(m_renameAction);

	m->addSeparator();
	m->addAction(m_addLeftExtensionPointAction);
	m->addAction(m_addRightExtensionPointAction);
	m->addAction(m_removeLeftExtensionPointAction);
	m->addAction(m_removeRightExtensionPointAction);

	m->addSeparator();
	m->addAction(m_showBackgroundAction);

	m->addSeparator();
	QMenu* iMenu = m->addMenu(GeoDataRiverSurvey::tr("Interpolation Mode"));
	iMenu->addAction(m_interpolateSplineAction);
	iMenu->addAction(m_interpolateLinearAction);

	m->addSeparator();
	m->addAction(m_mapPointsAction);

	m->addSeparator();
	m->addAction(m_rs->deleteAction());
}

void GeoDataRiverSurvey::Impl::updateLabelsAndSplines()
{
	auto col = m_rs->actor2DCollection();

	m_centerLineSplineController.setPolyLine(PolyLineControllerUtil::buildSplinePoints(m_centerLineController, SPLINE_FACTOR));
	m_leftBankLineSplineController.setPolyLine(PolyLineControllerUtil::buildSplinePoints(m_leftBankLineController, SPLINE_FACTOR));
	m_rightBankLineSplineController.setPolyLine(PolyLineControllerUtil::buildSplinePoints(m_rightBankLineController, SPLINE_FACTOR));

	m_upstreamActor.actor()->VisibilityOff();
	m_downstreamActor.actor()->VisibilityOff();
	col->RemoveItem(m_upstreamActor.actor());
	col->RemoveItem(m_downstreamActor.actor());

	m_upstreamLineController.clear();
	m_downstreamLineController.clear();

	auto line = m_centerLineController.polyLine();
	if (line.size() < 2) {return;}

	m_upstreamActor.setPosition(line.at(0));
	m_downstreamActor.setPosition(line.at(line.size() - 1));
	col->AddItem(m_upstreamActor.actor());
	col->AddItem(m_downstreamActor.actor());

	if (m_leftBankLineController.polyLine().size() > 0 && m_rightBankLineController.polyLine().size() > 0) {
		std::vector<QPointF> upstream, downstream;

		auto center = m_centerLineController.polyLine();
		auto left = m_leftBankLineController.polyLine();
		auto right = m_rightBankLineController.polyLine();

		upstream.push_back(left.at(0));
		upstream.push_back(center.at(0));
		upstream.push_back(right.at(0));

		downstream.push_back(left.at(left.size() - 1));
		downstream.push_back(center.at(center.size() - 1));
		downstream.push_back(right.at(right.size() - 1));

		m_upstreamLineController.setPolyLine(upstream);
		m_downstreamLineController.setPolyLine(downstream);
	}

	m_rs->updateVisibilityWithoutRendering();
}

void GeoDataRiverSurvey::Impl::updateActionStatus()
{
	if (m_mode == Mode::CreateMode) {
		createModeUpdateActionStatus();
	} else if (m_mode == Mode::EditMode) {
		editModeUpdateActionStatus();
	}
}

void GeoDataRiverSurvey::Impl::createModeUpdateActionStatus()
{
	switch (m_createMouseEventMode) {
	case CreateMouseEventMode::BeforeDefining:
	case CreateMouseEventMode::Defining:
	case CreateMouseEventMode::MoveVertex:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		break;
	case CreateMouseEventMode::Normal:
	case CreateMouseEventMode::MoveVertexPrepare:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(false);
		break;
	case CreateMouseEventMode::AddVertexPrepare:
	case CreateMouseEventMode::AddVertexNotPossible:
	case CreateMouseEventMode::AddVertex:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(true);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		break;
	case CreateMouseEventMode::RemoveVertexPrepare:
	case CreateMouseEventMode::RemoveVertexNotPossible:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(true);
		break;
	}
	m_buildBankLinesAction->setEnabled(m_centerLineController.polyLine().size() >= 2);
	m_generateAction->setEnabled(
				m_leftBankLineController.polyLine().size() >= 2 &&
				m_rightBankLineController.polyLine().size() >= 2);
}

void GeoDataRiverSurvey::Impl::editModeUpdateActionStatus()
{
	int selectCount = m_rs->m_headPoint->selectedPoints();
	bool singleSelection = (selectCount == 1);
	bool selectionExists = (selectCount > 0);
	GeoDataRiverPathPoint* selected = nullptr;
	if (singleSelection) {
		selected = m_rs->singleSelectedPoint();
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
	m_mapPointsAction->setEnabled(selectionExists);
	m_openCrossSectionWindowAction->setEnabled(selectionExists);
}

void GeoDataRiverSurvey::Impl::updateVtkPointsObjects()
{
	m_pointPoints->Initialize();
	m_pointPoints->SetDataTypeToDouble();

	auto centerPoints = vtkSmartPointer<vtkCellArray>::New();
	auto crossSectionLines = vtkSmartPointer<vtkCellArray>::New();

	m_riverCenterPoints->Initialize();
	m_crossSectionLines->Initialize();

	auto p = m_rs->headPoint()->nextPoint();
	int pointId = 0;
	while (p != nullptr) {
		// left bank
		addPoint(m_pointPoints, p->crosssectionPosition(p->crosssection().leftBank(true).position()));
		// left fixed point
		if (p->crosssection().fixedPointLSet()) {
			addPoint(m_pointPoints, p->crosssectionPosition(p->crosssection().fixedPointL().position()));
		} else {
			addPoint(m_pointPoints, p->crosssectionPosition(p->crosssection().leftBank(true).position()));
		}
		// center
		addPoint(m_pointPoints, p->position());
		// right fixed point
		if (p->crosssection().fixedPointRSet()) {
			addPoint(m_pointPoints, p->crosssectionPosition(p->crosssection().fixedPointR().position()));
		} else {
			addPoint(m_pointPoints, p->crosssectionPosition(p->crosssection().rightBank(true).position()));
		}
		// right bank
		addPoint(m_pointPoints, p->crosssectionPosition(p->crosssection().rightBank(true).position()));

		vtkIdType ids[5];
		for (int i = 0; i < 5; ++i) {
			ids[i] = pointId * 5 + i;
		}
		centerPoints->InsertNextCell(1, &(ids[2]));
		crossSectionLines->InsertNextCell(5, &(ids[0]));

		p = p->nextPoint();
		++ pointId;
	}
	m_riverCenterPoints->SetPoints(m_pointPoints);
	m_riverCenterPoints->SetVerts(centerPoints);
	m_crossSectionLines->SetPoints(m_pointPoints);
	m_crossSectionLines->SetLines(crossSectionLines);
}

void GeoDataRiverSurvey::Impl::updateVtkCenterAndBankLinesObjects()
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	m_centerAndBankLines->Initialize();

	auto p = m_rs->headPoint()->nextPoint();
	int pointId = 0;
	while (p != nullptr) {
		// center
		addPoint(points, p->position());
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				addPoint(points, p->riverCenter()->interpolate(i / static_cast<double> (LINEDIVS)));
			}
		}
		// left bank
		addPoint(points, p->crosssectionPosition(p->crosssection().leftBank(true).position()));
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				addPoint(points, p->leftBank()->interpolate(i / static_cast<double> (LINEDIVS)));
			}
		}
		// right bank
		addPoint(points, p->crosssectionPosition(p->crosssection().rightBank(true).position()));
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				addPoint(points, p->rightBank()->interpolate(i / static_cast<double> (LINEDIVS)));
			}
		}
		p = p->nextPoint();
		++ pointId;
	}
	auto lines = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < 3; ++i) {
		std::vector<vtkIdType> ids;
		for (int j = 0; j < pointId - 1; ++j) {
			for (int k = 0; k < LINEDIVS; ++k) {
				ids.push_back(i * LINEDIVS + j * LINEDIVS * 3 + k);
			}
		}
		ids.push_back(LINEDIVS * 3 * (pointId - 1) + i);
		lines->InsertNextCell(ids.size(), ids.data());
	}
	m_centerAndBankLines->SetPoints(points);
	m_centerAndBankLines->SetLines(lines);
}

void GeoDataRiverSurvey::Impl::updateVtkSelectedObjects()
{
	auto selectedCenterPoints = vtkSmartPointer<vtkCellArray>::New();
	auto selectedLeftBankPoints = vtkSmartPointer<vtkCellArray>::New();
	auto selectedRightBankPoints = vtkSmartPointer<vtkCellArray>::New();
	auto selectedCrossSectionLines = vtkSmartPointer<vtkCellArray>::New();

	m_selectedRiverCenterPoints->Initialize();
	m_selectedLeftBankPoints->Initialize();
	m_selectedRightBankPoints->Initialize();
	m_selectedCrossSectionLines->Initialize();

	auto p = m_rs->headPoint()->nextPoint();
	int pointId = 0;
	while (p != nullptr) {
		vtkIdType ids[5];

		if (p->IsSelected) {
			for (int i = 0; i < 5; ++i) {
				ids[i] = pointId * 5 + i;
			}
			selectedCenterPoints->InsertNextCell(1, &(ids[2]));
			selectedLeftBankPoints->InsertNextCell(1, &(ids[0]));
			selectedRightBankPoints->InsertNextCell(1, &(ids[4]));
			selectedCrossSectionLines->InsertNextCell(5, &(ids[0]));
		}
		p = p->nextPoint();
		++ pointId;
	}
	m_selectedRiverCenterPoints->SetPoints(m_pointPoints);
	m_selectedRiverCenterPoints->SetVerts(selectedCenterPoints);
	m_selectedLeftBankPoints->SetPoints(m_pointPoints);
	m_selectedLeftBankPoints->SetVerts(selectedLeftBankPoints);
	m_selectedRightBankPoints->SetPoints(m_pointPoints);
	m_selectedRightBankPoints->SetVerts(selectedRightBankPoints);
	m_selectedCrossSectionLines->SetPoints(m_pointPoints);
	m_selectedCrossSectionLines->SetLines(selectedCrossSectionLines);
}

void GeoDataRiverSurvey::Impl::updateVtkVerticalCenterLinesObjects()
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto verticalLines = vtkSmartPointer<vtkCellArray>::New();

	m_verticalCrossSectionLines->Initialize();

	auto p = m_rs->headPoint()->nextPoint();
	while (p != nullptr) {
		double maxHeight = 0;
		const auto& alist = p->crosssection().AltitudeInfo();
		for (int i = 0; i < alist.size(); ++i) {
			const auto& alt = alist[i];
			if (i == 0 || maxHeight < alt.height()) {maxHeight = alt.height();}
		}
		QPointF offsetDir = p->crosssectionDirection();
		iRIC::rotateVector270(offsetDir);

		vtkIdType firstId = points->GetNumberOfPoints();
		std::vector<vtkIdType> ids;
		ids.reserve(alist.size());

		for (int i = 0; i < alist.size(); ++i) {
			const auto& alt = alist[i];
			double offset = (maxHeight - alt.height()) * m_rs->m_setting.crosssectionLinesScale;
			QPointF tmpp = p->crosssectionPosition(alt.position()) + offsetDir * offset;
			points->InsertNextPoint(tmpp.x(), tmpp.y(), 0);
			ids.push_back(firstId + i);
		}
		verticalLines->InsertNextCell(ids.size(), ids.data());
		p = p->nextPoint();
	}
	m_verticalCrossSectionLines->SetPoints(points);
	m_verticalCrossSectionLines->SetLines(verticalLines);

	if (m_rs->m_setting.showLines) {
		m_verticalCrossSectionLinesActor->VisibilityOn();
		m_verticalCrossSectionLinesActor->GetProperty()->SetColor(m_rs->m_setting.crosssectionLinesColor);
	}	else {
		m_verticalCrossSectionLinesActor->VisibilityOff();
	}
}

void GeoDataRiverSurvey::Impl::updateVtkNameLabelObjects()
{
	auto p = m_rs->m_headPoint->nextPoint();

	m_rightBankPoints->Reset();
	double point[3];
	point[2] = 0;
	while (p != nullptr) {
		QPointF rightBank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
		point[0] = rightBank.x();
		point[1] = rightBank.y();
		m_rightBankPoints->InsertNextPoint(point);
		p = p->nextPoint();
	}
	m_rightBankPoints->Modified();

	m_rightBankPointSet->SetPoints(m_rightBankPoints);

	m_labelArray->Reset();
	p = m_rs->m_headPoint->nextPoint();
	while (p != nullptr) {
		QString name = p->name();
		name.prepend(tr("  "));
		m_labelArray->InsertNextValue(iRIC::toStr(name).c_str());
		p = p->nextPoint();
	}
	m_rightBankPointSet->Modified();
}

void GeoDataRiverSurvey::Impl::updateVtkBackgroundObjects()
{
	m_backgroundActor->VisibilityOff();
	auto col = m_rs->actorCollection();
	col->RemoveItem(m_backgroundActor);

	if (m_rs->m_setting.showBackground) {
		col->AddItem(m_backgroundActor);
		m_backgroundActor->GetProperty()->SetOpacity(m_rs->m_setting.opacity);
		m_rs->updateVisibilityWithoutRendering();
	}
}

void GeoDataRiverSurvey::Impl::importLine(PolyLineController* line)
{
	auto polyLine = CoordinatesEdit::unapplyOffset(
				PolylineIO::importData(m_rs->preProcessorWindow()), m_rs->offset());
	if (polyLine.size() == 0) {return;}

	line->setPolyLine(polyLine);
	m_createMouseEventMode = CreateMouseEventMode::Normal;

	updateLabelsAndSplines();
	m_rs->renderGraphicsView();
}

void GeoDataRiverSurvey::Impl::exportLine(PolyLineController* line, const QString& lineName)
{
	auto polyLine = CoordinatesEdit::applyOffset(line->polyLine(), m_rs->offset());
	if (polyLine.size() == 0) {
		QMessageBox::warning(m_rs->preProcessorWindow(), GeoDataRiverSurvey::tr("Warning"),
												 GeoDataRiverSurvey::tr("%1 is not defined yet").arg(lineName));
		return;
	}
	PolylineIO::exportData(polyLine, m_rs->preProcessorWindow());
}

void GeoDataRiverSurvey::Impl::createModeUpdateMouseEventMode(const QPoint& mousePosition)
{
	auto v = m_rs->graphicsView();
	auto p = v->viewportToWorld(mousePosition);
	double radius = v->stdRadius(iRIC::nearRadius());

	// switch active line;
	int tmp_edgeid;
	if (m_leftBankLineSplineController.isEdgeSelectable(p, radius, &tmp_edgeid)) {
		m_activePoints = &m_leftBankLineController;
		m_activeLine = &m_leftBankLineSplineController;
	} else if (m_rightBankLineSplineController.isEdgeSelectable(p, radius, &tmp_edgeid)) {
		m_activePoints = &m_rightBankLineController;
		m_activeLine = &m_rightBankLineSplineController;
	} else {
		m_activePoints = &m_centerLineController;
		m_activeLine = &m_centerLineSplineController;
	}

	switch (m_createMouseEventMode) {
	case CreateMouseEventMode::AddVertexPrepare:
	case CreateMouseEventMode::AddVertexNotPossible:
		if (m_activeLine->isEdgeSelectable(p, radius, &m_selectedEdgeId)) {
			m_selectedEdgeId = m_activePoints->findNearestLine(p);
			m_createMouseEventMode = CreateMouseEventMode::AddVertexPrepare;
		} else {
			m_createMouseEventMode = CreateMouseEventMode::AddVertexNotPossible;
		}
		break;
	case CreateMouseEventMode::RemoveVertexPrepare:
	case CreateMouseEventMode::RemoveVertexNotPossible:
		if (m_activePoints->isVertexSelectable(p, radius, &m_selectedVertexId)) {
			m_createMouseEventMode = CreateMouseEventMode::RemoveVertexPrepare;
		} else {
			m_createMouseEventMode = CreateMouseEventMode::RemoveVertexNotPossible;
		}
		break;
	case CreateMouseEventMode::Normal:
	case CreateMouseEventMode::MoveVertexPrepare:
	case CreateMouseEventMode::MoveVertex:
	case CreateMouseEventMode::AddVertex:
		if (m_activePoints->isVertexSelectable(p, radius, &m_selectedVertexId)) {
			m_createMouseEventMode = CreateMouseEventMode::MoveVertexPrepare;
		} else {
			m_createMouseEventMode = CreateMouseEventMode::Normal;
		}
		break;

	default:
		break;
	}
}

void GeoDataRiverSurvey::Impl::editModeUpdateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();
	m_rs->graphicsView()->viewportToWorld(dx, dy);
	QPointF worldPos(dx, dy);
	double stdLen = m_rs->graphicsView()->stdRadius(1);
	double stdLen2 = stdLen * stdLen;
	int selectCount = m_rs->m_headPoint->selectedPoints();
	GeoDataRiverPathPoint* selected = nullptr;
	if (selectCount == 1) {
		selected = m_rs->m_headPoint->nextPoint();
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
				m_editMouseEventMode = EditMouseEventMode::TranslatePrepare;
			}
			if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
					(m_keyboardModifiers & Qt::ControlModifier) == 0) {
				// preparing for center-point shift
				m_editMouseEventMode = EditMouseEventMode::ShiftPrepare;
			}
		} else {
			m_editMouseEventMode = EditMouseEventMode::Normal;
			QPointF lbank = selected->crosssectionPosition(selected->crosssection().leftBank(true).position());
			if (iRIC::lengthSquared(lbank - worldPos) < stdLen2 * 9) {
				// cursor is near left bank.
				if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0) {
					// preparing for rotating
					m_editMouseEventMode = EditMouseEventMode::RotatePrepareLeft;
				}
				if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0 &&
						selected->crosssection().fixedPointLSet()) {
					// preparing for center-point shift
					m_editMouseEventMode = EditMouseEventMode::MoveExtensionEndPointPrepareLeft;
				}
			}
			QPointF rbank = selected->crosssectionPosition(selected->crosssection().rightBank(true).position());
			if (iRIC::lengthSquared(rbank - worldPos) < stdLen2 * 9) {
				// cursor is near right bank.
				if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0) {
					// preparing for rotating
					m_editMouseEventMode = EditMouseEventMode::RotatePrepareRight;
				}
				if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
						(m_keyboardModifiers & Qt::ControlModifier) == 0 &&
						selected->crosssection().fixedPointRSet()) {
					// preparing for center-point shift
					m_editMouseEventMode = EditMouseEventMode::MoveExtensionEndPointPrepareRight;
				}
			}
		}
	} else {
		// multiple selection, or none.
		GeoDataRiverPathPoint* p = m_rs->m_headPoint->nextPoint();
		m_editMouseEventMode = EditMouseEventMode::Normal;
		while (p != nullptr) {
			if (p->IsSelected) {
				if (iRIC::lengthSquared(p->position() - worldPos) < stdLen2 * 9) {
					// cursor is near to the river center point
					if ((m_keyboardModifiers & Qt::ShiftModifier) == 0 &&
							(m_keyboardModifiers & Qt::ControlModifier) == 0) {
						// preparing for moving
						m_editMouseEventMode = EditMouseEventMode::TranslatePrepare;
						return;
					}
					if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
							(m_keyboardModifiers & Qt::ControlModifier) == 0) {
						// preparing for center-point shift
						m_editMouseEventMode = EditMouseEventMode::ShiftPrepare;
						return;
					}
				} else {
					m_editMouseEventMode = EditMouseEventMode::Normal;
					QPointF lbank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
					if (iRIC::lengthSquared(lbank - worldPos) < stdLen2 * 9) {
						// cursor is near left bank.
						if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
								(m_keyboardModifiers & Qt::ControlModifier) == 0) {
							// preparing for center-point shift
							m_editMouseEventMode = EditMouseEventMode::ExpansionPrepareLeft;
							return;
						}
					}
					QPointF rbank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
					if (iRIC::lengthSquared(rbank - worldPos) < stdLen2 * 9) {
						// cursor is near right bank.
						if ((m_keyboardModifiers & Qt::ShiftModifier) != 0 &&
								(m_keyboardModifiers & Qt::ControlModifier) == 0) {
							// preparing for center-point shift
							m_editMouseEventMode = EditMouseEventMode::ExpansionPrepareRight;
							return;
						}
					}
				}
			}
			p = p->nextPoint();
		}
	}
}

void GeoDataRiverSurvey::Impl::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	if (m_mode == Mode::CreateMode) {
		createModeUpdateMouseCursor(v);
	} else if (m_mode == Mode::EditMode) {
		editModeUpdateMouseCursor(v);
	}
}

void GeoDataRiverSurvey::Impl::createModeUpdateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_createMouseEventMode) {
	case CreateMouseEventMode::Normal:
	case CreateMouseEventMode::AddVertexNotPossible:
	case CreateMouseEventMode::RemoveVertexNotPossible:
	case CreateMouseEventMode::EditCoodinatesDialog:
		v->setCursor(Qt::ArrowCursor);
		break;
	case CreateMouseEventMode::BeforeDefining:
	case CreateMouseEventMode::Defining:
		v->setCursor(Qt::CrossCursor);
		break;
	case CreateMouseEventMode::MoveVertexPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case CreateMouseEventMode::AddVertexPrepare:
		v->setCursor(m_cursorAdd);
		break;
	case CreateMouseEventMode::RemoveVertexPrepare:
		v->setCursor(m_cursorRemove);
		break;
	case CreateMouseEventMode::MoveVertex:
	case CreateMouseEventMode::AddVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	}
}

void GeoDataRiverSurvey::Impl::editModeUpdateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_editMouseEventMode) {
	case Impl::EditMouseEventMode::Normal:
	case Impl::EditMouseEventMode::AddingExtension:
		v->setCursor(Qt::ArrowCursor);
		break;
	case Impl::EditMouseEventMode::Translate:
	case Impl::EditMouseEventMode::TranslatePrepare:
		v->setCursor(m_cursorMove);
		break;
	case Impl::EditMouseEventMode::RotateRight:
	case Impl::EditMouseEventMode::RotatePrepareRight:
	case Impl::EditMouseEventMode::RotateLeft:
	case Impl::EditMouseEventMode::RotatePrepareLeft:
		v->setCursor(m_cursorRotate);
		break;
	case Impl::EditMouseEventMode::Shift:
	case Impl::EditMouseEventMode::ShiftPrepare:
		v->setCursor(m_cursorShift);
		break;
	case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
	case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
		v->setCursor(m_cursorMove);
		break;
	case Impl::EditMouseEventMode::ExpansionRight:
	case Impl::EditMouseEventMode::ExpansionPrepareRight:
	case Impl::EditMouseEventMode::ExpansionLeft:
	case Impl::EditMouseEventMode::ExpansionPrepareLeft:
		v->setCursor(m_cursorExpand);
		break;
	default:
		break;
	}
}
