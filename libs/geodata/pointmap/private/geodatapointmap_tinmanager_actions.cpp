#include "geodatapointmap_tinmanager_actions.h"
#include "geodatapointmap_tinmanager_impl.h"

GeoDataPointmap::TINManager::Actions::Actions(TINManager* manager, GeoDataPointmap* pointmap) :
	addBreakLineAction {new QAction(GeoDataPointmap::tr("Add &Break Line"), manager)},
	removeBreakLineAction {new QAction(GeoDataPointmap::tr("&Remove Break Line"), manager)},
	removeAllBreakLinesAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), GeoDataPointmap::tr("Re&move All Break Lines..."), manager)},
	remeshTinAction {new QAction(GeoDataPointmap::tr("Remesh &TIN"), manager)},
	removeTrianglesSettingAction {new QAction(GeoDataPointmap::tr("Remove triangles with long edge setting"), manager)}
{
	addBreakLineAction->setCheckable(true);
	connect(addBreakLineAction, &QAction::triggered, manager, &TINManager::toggleAddBreakLineMode);

	removeBreakLineAction->setCheckable(true);
	connect(removeBreakLineAction, &QAction::triggered, manager, &TINManager::toggleRemoveBreakLineMode);

	connect(removeAllBreakLinesAction, &QAction::triggered, manager, &TINManager::removeAllBreakLines);
	connect(remeshTinAction, &QAction::triggered, pointmap, &GeoDataPointmap::remeshTin);
	connect(removeTrianglesSettingAction, &QAction::triggered, manager, &TINManager::showRemoveTrianglesSettingDialog);
}
