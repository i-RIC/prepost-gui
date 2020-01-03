#include "geodatariversurvey_impl.h"

#include <misc/mathsupport.h>
#include <misc/stringtool.h>

#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextProperty.h>

#include <QAction>
#include <QMenu>

namespace {

void addPoint(vtkPoints* points, const QPointF& p)
{
	points->InsertNextPoint(p.x(), p.y(), 0);
}

} // namespace

GeoDataRiverSurvey::Impl::Impl(GeoDataRiverSurvey* rs) :
	m_editMouseEventMode {EditMouseEventMode::Normal},
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
	m_rightClickingMenu {nullptr},
	m_addUpperSideAction {new QAction(GeoDataRiverSurvey::tr("Insert Upstream Side(&B)..."), rs)},
	m_addLowerSideAction {new QAction(GeoDataRiverSurvey::tr("Insert Downstream Side(&A)..."), rs)},
	m_moveAction {new QAction(GeoDataRiverSurvey::tr("&Move..."), rs)},
	m_rotateAction {new QAction(GeoDataRiverSurvey::tr("&Rotate..."), rs)},
	m_shiftAction {new QAction(GeoDataRiverSurvey::tr("S&hift Center..."), rs)},
	m_expandAction {new QAction(GeoDataRiverSurvey::tr("E&xtend Horizontally..."), rs)},
	m_deleteAction {new QAction(GeoDataRiverSurvey::tr("Dele&te Cross Section"), rs)},
	m_renameAction{ new QAction(GeoDataRiverSurvey::tr("R&ename Cross Section..."), rs) },
	m_addLeftExtensionPointAction{ new QAction(GeoDataRiverSurvey::tr("Add &Left Bank Extension Line..."), rs) },
	m_addRightExtensionPointAction{ new QAction(GeoDataRiverSurvey::tr("Add &Right Bank Extension Line..."), rs) },
	m_removeLeftExtensionPointAction{ new QAction(GeoDataRiverSurvey::tr("Remo&ve Left Bank Extension Line"), rs) },
	m_removeRightExtensionPointAction{ new QAction(GeoDataRiverSurvey::tr("Rem&ove Right Bank Extension Line"), rs) },
	m_openCrossSectionWindowAction{ new QAction(GeoDataRiverSurvey::tr("Display &Cross Section"), rs) },
	m_showBackgroundAction{ new QAction(GeoDataRiverSurvey::tr("Display &Setting"), rs) },
	m_interpolateSplineAction{ new QAction(GeoDataRiverSurvey::tr("Spline"), rs) },
	m_interpolateLinearAction{ new QAction(GeoDataRiverSurvey::tr("Linear Curve"), rs) },
	m_rs {rs}
{
	m_rightBankPoints->SetDataTypeToDouble();
	m_labelArray->SetName("Label");
}

GeoDataRiverSurvey::Impl::~Impl()
{
	auto r = m_rs->renderer();
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

	delete m_rightClickingMenu;
}

void GeoDataRiverSurvey::Impl::setupActions()
{
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
}

void GeoDataRiverSurvey::Impl::setupMenu()
{
	auto m = m_rs->m_menu;
	m->setTitle(tr("&River Survey"));
	m->addAction(m_rs->m_editNameAction);

	m->addSeparator();
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
	QMenu* iMenu = m->addMenu(tr("Interpolation Mode"));
	iMenu->addAction(m_interpolateSplineAction);
	iMenu->addAction(m_interpolateLinearAction);

	m->addSeparator();
	m->addAction(m_rs->deleteAction());

	m_rightClickingMenu = new QMenu();
	m = m_rightClickingMenu;
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
	iMenu = m->addMenu(tr("Interpolation Mode"));
	iMenu->addAction(m_interpolateSplineAction);
	iMenu->addAction(m_interpolateLinearAction);
}

void GeoDataRiverSurvey::Impl::updateActionStatus()
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

void GeoDataRiverSurvey::Impl::setupCursors()
{
	m_pixmapMove = QPixmap(":/libs/guibase/images/cursorItemMove.png");
	m_cursorMove = QCursor(m_pixmapMove, 7, 2);

	m_pixmapRotate = QPixmap(":/libs/geodata/riversurvey/images/cursorCrosssectionRotate.png");
	m_cursorRotate = QCursor(m_pixmapRotate, 7, 2);

	m_pixmapExpand = QPixmap(":/libs/geodata/riversurvey/images/cursorExpand.png");
	m_cursorExpand = QCursor(m_pixmapExpand, 7, 2);

	m_pixmapShift = QPixmap(":/libs/geodata/riversurvey/images/cursorShiftCenter.png");
	m_cursorShift = QCursor(m_pixmapShift, 7, 2);
}
