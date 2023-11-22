#ifndef GEODATAPOINTMAP_POINTSMANAGER_ACTIONS_H
#define GEODATAPOINTMAP_POINTSMANAGER_ACTIONS_H

#include "geodatapointmap_pointsmanager.h"

class QAction;
class QMenu;

class GeoDataPointmap::PointsManager::Actions
{
public:
	Actions(PointsManager* manager);
	~Actions();

	QMenu* selectionModeMenu;
	QAction* selectionModeRectAction;
	QAction* selectionModePolygonAction;

	QAction* addPointsAction;
	// QAction* interpolatePointsAction;

	QAction* editSelectedPointsAction;
	QAction* deleteSelectedPointsAction;
	QAction* filterSelectedPointsAction;
	QAction* exportSelectedPointsAction;

	QAction* remeshTinAction;

	std::vector<QAction*> checkableActions;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_ACTIONS_H
