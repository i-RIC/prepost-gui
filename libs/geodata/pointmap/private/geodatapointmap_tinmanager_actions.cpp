#include "geodatapointmap_tinmanager_actions.h"

GeoDataPointmap::TINManager::Actions::Actions(TINManager* manager) :
	addBreakLineAction {new QAction(GeoDataPointmap::tr("Add &Break Line"), manager)},
	removeBreakLineAction {new QAction(GeoDataPointmap::tr("&Remove Break Line"), manager)},
	removeAllBreakLinesAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), GeoDataPointmap::tr("Re&move All Break Lines..."), manager)},
	remeshTinAction {new QAction(GeoDataPointmap::tr("Remesh &TIN"), manager)}
{
	addBreakLineAction->setCheckable(true);
	connect(addBreakLineAction, &QAction::triggered, manager, &TINManager::toggleAddBreakLineMode);

	removeBreakLineAction->setCheckable(true);
	connect(removeBreakLineAction, &QAction::triggered, manager, &TINManager::toggleRemoveBreakLineMode);

	connect(removeAllBreakLinesAction, &QAction::triggered, [=](bool) {manager->removeAllBreakLines();});

	connect(remeshTinAction, &QAction::triggered, [=](bool) {manager->m_parent->remeshTin();});
}
