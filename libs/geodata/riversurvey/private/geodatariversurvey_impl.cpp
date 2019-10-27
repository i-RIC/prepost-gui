#include "geodatariversurvey_impl.h"

#include <QAction>

GeoDataRiverSurvey::Impl::Impl(GeoDataRiverSurvey* rs) :
	m_mouseEventMode {meNormal},
	m_rs {rs}
{}

GeoDataRiverSurvey::Impl::~Impl()
{}

void GeoDataRiverSurvey::Impl::setupActions()
{
	m_addUpperSideAction = new QAction(GeoDataRiverSurvey::tr("Insert Upstream Side(&B)..."), m_rs);
	connect(m_addUpperSideAction, SIGNAL(triggered()), m_rs, SLOT(insertNewPoint()));
	m_addLowerSideAction = new QAction(GeoDataRiverSurvey::tr("Insert Downstream Side(&A)..."), m_rs);
	connect(m_addLowerSideAction, SIGNAL(triggered()), m_rs, SLOT(addNewPoint()));
	m_moveAction = new QAction(GeoDataRiverSurvey::tr("&Move..."), m_rs);
	connect(m_moveAction, SIGNAL(triggered()), m_rs, SLOT(moveSelectedPoints()));
	m_rotateAction = new QAction(GeoDataRiverSurvey::tr("&Rotate..."), m_rs);
	connect(m_rotateAction, SIGNAL(triggered()), m_rs, SLOT(rotateSelectedPoint()));
	m_shiftAction = new QAction(GeoDataRiverSurvey::tr("S&hift Center..."), m_rs);
	connect(m_shiftAction, SIGNAL(triggered()), m_rs, SLOT(shiftSelectedPoints()));
	m_expandAction = new QAction(GeoDataRiverSurvey::tr("E&xtend Horizontally..."), m_rs);
	connect(m_expandAction, SIGNAL(triggered()), m_rs, SLOT(expandSelectedPoints()));
	m_deleteAction = new QAction(GeoDataRiverSurvey::tr("Dele&te Cross Section"), m_rs);
	connect(m_deleteAction, SIGNAL(triggered()), m_rs, SLOT(deleteSelectedPoints()));
	m_renameAction = new QAction(GeoDataRiverSurvey::tr("R&ename Cross Section..."), m_rs);
	connect(m_renameAction, SIGNAL(triggered()), m_rs, SLOT(renameSelectedPoint()));
	m_addLeftExtensionPointAction = new QAction(GeoDataRiverSurvey::tr("Add &Left Bank Extension Line..."), m_rs);
	connect(m_addLeftExtensionPointAction, SIGNAL(triggered()), m_rs, SLOT(addLeftExtensionPoint()));
	m_addRightExtensionPointAction = new QAction(GeoDataRiverSurvey::tr("Add &Right Bank Extension Line..."), m_rs);
	connect(m_addRightExtensionPointAction, SIGNAL(triggered()), m_rs, SLOT(addRightExtensionPoint()));
	m_removeLeftExtensionPointAction = new QAction(GeoDataRiverSurvey::tr("Remo&ve Left Bank Extension Line"), m_rs);
	connect(m_removeLeftExtensionPointAction, SIGNAL(triggered()), m_rs, SLOT(removeLeftExtensionPoint()));
	m_removeRightExtensionPointAction = new QAction(GeoDataRiverSurvey::tr("Rem&ove Right Bank Extension Line"), m_rs);
	connect(m_removeRightExtensionPointAction, SIGNAL(triggered()), m_rs, SLOT(removeRightExtensionPoint()));
	m_openCrossSectionWindowAction = new QAction(GeoDataRiverSurvey::tr("Display &Cross Section"), m_rs);
	connect(m_openCrossSectionWindowAction, SIGNAL(triggered()), m_rs, SLOT(openCrossSectionWindow()));

	m_showBackgroundAction = new QAction(GeoDataRiverSurvey::tr("Display &Setting"), m_rs);
	connect(m_showBackgroundAction, SIGNAL(triggered()), m_rs, SLOT(displaySetting()));
	m_interpolateSplineAction = new QAction(GeoDataRiverSurvey::tr("Spline"), m_rs);
	connect(m_interpolateSplineAction, SIGNAL(triggered()), m_rs, SLOT(switchInterpolateModeToSpline()));
	m_interpolateSplineAction->setCheckable(true);
	m_interpolateSplineAction->setChecked(true);
	m_interpolateLinearAction = new QAction(GeoDataRiverSurvey::tr("Linear Curve"), m_rs);
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
