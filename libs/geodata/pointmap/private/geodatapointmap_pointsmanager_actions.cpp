#include "geodatapointmap_pointsmanager_actions.h"

#include <QAction>
#include <QMenu>

GeoDataPointmap::PointsManager::Actions::Actions(PointsManager* manager) :
	selectionModeMenu {new QMenu(GeoDataPointmap::tr("Selection Mode"))},
	selectionModeRectAction {new QAction(GeoDataPointmap::tr("Select points with Rectangle Region"), manager)},
	selectionModePolygonAction {new QAction(GeoDataPointmap::tr("Select Points With &Polygon"), manager)},
	addPointsAction {new QAction(GeoDataPointmap::tr("&Add New Point"), manager)},
	// interpolatePointsAction {new QAction(GeoDataPointmap::tr("&Interpolate Points"), manager)},
	editSelectedPointsAction {new QAction(GeoDataPointmap::tr("&Edit Selected Points..."), manager)},
	deleteSelectedPointsAction {new QAction(GeoDataPointmap::tr("Delete Selected P&oints"), manager)},
	filterSelectedPointsAction {new QAction(GeoDataPointmap::tr("&Filter selected points"), manager)},
	exportSelectedPointsAction {new QAction(GeoDataPointmap::tr("E&xport Selected Points..."), manager)},
	remeshTinAction {new QAction(GeoDataPointmap::tr("Remesh &TIN"), manager)}
{
	selectionModeMenu->addAction(selectionModeRectAction);
	selectionModeMenu->addAction(selectionModePolygonAction);

	selectionModeRectAction->setCheckable(true);
	selectionModeRectAction->setChecked(true);
	connect(selectionModeRectAction, &QAction::triggered, manager, &PointsManager::toggleRectSelectMode);

	selectionModePolygonAction->setCheckable(true);
	connect(selectionModePolygonAction, &QAction::triggered, manager, &PointsManager::togglePolygonSelectMode);

	addPointsAction->setCheckable(true);
	connect(addPointsAction, &QAction::triggered, manager, &PointsManager::toggleAddPointsMode);

	// interpolatePointsAction->setCheckable(true);
	// connect(interpolatePointsAction, &QAction::triggered, manager, &PointsManager::toggleInterpolatePointsMode);

	connect(editSelectedPointsAction, &QAction::triggered, [=](bool) {manager->editSelectedPoints();});
	connect(deleteSelectedPointsAction, &QAction::triggered, [=](bool) {manager->deleteSelectedPoints();});
	connect(filterSelectedPointsAction, &QAction::triggered, [=](bool) {manager->filterSelectedPoints();});
	connect(exportSelectedPointsAction, &QAction::triggered, [=](bool) {manager->exportSelectedPoints();});

	connect(remeshTinAction, &QAction::triggered, [=](bool) {manager->remeshTinAndSwitchToTinEditMode();});

	checkableActions.push_back(selectionModeRectAction);
	checkableActions.push_back(selectionModePolygonAction);
	checkableActions.push_back(addPointsAction);
	// checkableActions.push_back(interpolatePointsAction);
}

GeoDataPointmap::PointsManager::Actions::~Actions()
{
	delete selectionModeMenu;
}
