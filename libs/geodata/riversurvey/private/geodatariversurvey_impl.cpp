#include "geodatariversurvey_impl.h"

#include <QAction>
#include <QMenu>

GeoDataRiverSurvey::Impl::Impl(GeoDataRiverSurvey* rs) :
	m_mouseEventMode {meNormal},
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
{}

GeoDataRiverSurvey::Impl::~Impl()
{
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
