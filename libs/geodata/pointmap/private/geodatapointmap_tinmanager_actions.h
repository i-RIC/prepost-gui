#ifndef GEODATAPOINTMAP_TINMANAGER_ACTIONS_H
#define GEODATAPOINTMAP_TINMANAGER_ACTIONS_H

#include "geodatapointmap_tinmanager.h"

class GeoDataPointmap::TINManager::Actions
{
public:
	Actions(TINManager* manager);

	QAction* addBreakLineAction;
	QAction* removeBreakLineAction;
	QAction* removeAllBreakLinesAction;

	QAction* remeshTinAction;
	QAction* removeTrianglesSettingAction;
};

#endif // GEODATAPOINTMAP_TINMANAGER_ACTIONS_H
