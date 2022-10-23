#include "geodatapointmap_impl.h"

#include <QAction>

GeoDataPointmap::Impl::Impl(GeoDataPointmap* pointmap) :
	m_pointsManager {pointmap},
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_removePixmap {":/libs/guibase/images/cursorRemove.png"},
	m_removeCursor {m_removePixmap, 0, 0},
	m_interpPointAddPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_interpPointAddCursor {m_interpPointAddPixmap, 0, 0},
	m_interpPointCtrlAddPixmap {":/images/cursorCtrlAdd.png"},
	m_interpPointCtrlAddCursor {m_interpPointCtrlAddPixmap, 0, 0},

	m_selectionModePoint {new QAction(QIcon(":/libs/geodata/pointmap/images/iconPointSelection.svg"), GeoDataPointmap::tr("Select point"), pointmap)},
	m_selectionModePolygon {new QAction(GeoDataPointmap::tr("Select Points With &Polygon"), pointmap)},
	m_addPointAction {new QAction(GeoDataPointmap::tr("&Add New Point"), pointmap)},
	m_interpolatePointAction {new QAction(GeoDataPointmap::tr("&Interpolate Points"), pointmap)},
	m_removeTrianglesWithLongEdgeAction {new QAction(GeoDataPointmap::tr("Remove Triangles &with Long edge..."), pointmap)},
	m_displaySettingAction {new QAction(GeoDataPointmap::tr("Display &Setting..."), pointmap)},
	m_editPointsAction {new QAction(GeoDataPointmap::tr("&Edit Selected Points..."), pointmap)},
	m_editPointsDeleteAction {new QAction(GeoDataPointmap::tr("Delete Selected P&oints"), pointmap)},
	m_editPointsExportAction {new QAction(GeoDataPointmap::tr("E&xport Selected Points..."), pointmap)},
	m_editPointsLessThanAction {new QAction(GeoDataPointmap::tr("Delete Selected Points &Less Than Value..."), pointmap)},
	m_editPointsGreaterThanAction {new QAction(GeoDataPointmap::tr("Delete Selected Points &Greater Than Value..."), pointmap)},
	m_remeshAction {new QAction(GeoDataPointmap::tr("Remesh &TINs"), pointmap)},
	m_addBreakLineAction {new QAction(GeoDataPointmap::tr("Add &Break Line"), pointmap)},
	m_removeBreakLineAction {new QAction(GeoDataPointmap::tr("&Remove Break Line"), pointmap)},
	m_removeAllBreakLinesAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), GeoDataPointmap::tr("Re&move All Break Lines..."), pointmap)},
	m_mergeAction {new QAction(GeoDataPointmap::tr("Merge..."), pointmap)},
	m_parent {pointmap}
{}

GeoDataPointmap::Impl::~Impl()
{}

void GeoDataPointmap::Impl::setupActions()
{
	m_selectionModePoint->setCheckable(true);
	GeoDataPointmap::connect(m_selectionModePoint, SIGNAL(triggered(bool)), m_parent, SLOT(selectionModePoint(bool)));

	m_selectionModePolygon->setIcon(QIcon(":/libs/geodata/pointmap/images/iconPolygonSelection.svg"));
	m_selectionModePolygon->setCheckable(true);
	GeoDataPointmap::connect(m_selectionModePolygon, SIGNAL(triggered(bool)), m_parent, SLOT(selectionModePolygon(bool)));

	m_addPointAction->setIcon(QIcon(":/libs/geodata/pointmap/images/iconAddPoint.svg"));
	m_addPointAction->setCheckable(true);
	GeoDataPointmap::connect(m_addPointAction, SIGNAL(triggered(bool)), m_parent, SLOT(addPoints(bool)));

	m_interpolatePointAction->setIcon(QIcon(":/libs/geodata/pointmap/images/iconInterpolatePoint.svg"));
	m_interpolatePointAction->setCheckable(true);
	GeoDataPointmap::connect(m_interpolatePointAction, SIGNAL(triggered(bool)), m_parent, SLOT(interpolatePoints(bool)));

	GeoDataPointmap::connect(m_removeTrianglesWithLongEdgeAction, SIGNAL(triggered()), m_parent, SLOT(removeTrianglesWithLongEdgeStart()));

	GeoDataPointmap::connect(m_displaySettingAction, SIGNAL(triggered()), m_parent, SLOT(showDisplaySetting()));

	m_editPointsAction->setCheckable(false);
	GeoDataPointmap::connect(m_editPointsAction, SIGNAL(triggered(bool)), m_parent, SLOT(editPoints()));

	m_editPointsDeleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.svg"));
	m_editPointsDeleteAction->setCheckable(false);
	GeoDataPointmap::connect(m_editPointsDeleteAction, SIGNAL(triggered(bool)), m_parent, SLOT(editPointsDelete()));

	m_editPointsExportAction->setIcon(QIcon(":/libs/guibase/images/iconExport.svg"));
	m_editPointsExportAction->setCheckable(false);
	GeoDataPointmap::connect(m_editPointsExportAction, SIGNAL(triggered(bool)), m_parent, SLOT(editPointsExport()));

	m_editPointsLessThanAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.svg"));
	m_editPointsLessThanAction->setCheckable(false);
	GeoDataPointmap::connect(m_editPointsLessThanAction, SIGNAL(triggered(bool)), m_parent, SLOT(editPointsLessThan()));

	m_editPointsGreaterThanAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.svg"));
	m_editPointsGreaterThanAction->setCheckable(false);
	GeoDataPointmap::connect(m_editPointsGreaterThanAction, SIGNAL(triggered(bool)), m_parent, SLOT(editPointsGreaterThan()));

	enablePointSelectedActions(false);

	GeoDataPointmap::connect(m_remeshAction, SIGNAL(triggered()), m_parent, SLOT(remeshTINS()));

	m_addBreakLineAction->setIcon(QIcon(":/libs/geodata/pointmap/images/iconBreakLineAdd.svg"));
	m_addBreakLineAction->setCheckable(true);
	GeoDataPointmap::connect(m_addBreakLineAction, SIGNAL(triggered()), m_parent, SLOT(addBreakLine()));

	m_removeBreakLineAction->setIcon(QIcon(":/libs/geodata/pointmap/images/iconBreakLineDelete.svg"));
	m_removeBreakLineAction->setCheckable(true);
	GeoDataPointmap::connect(m_removeBreakLineAction, SIGNAL(triggered()), m_parent, SLOT(removeBreakLine()));

	GeoDataPointmap::connect(m_removeAllBreakLinesAction, SIGNAL(triggered()), m_parent, SLOT(removeAllBreakLines()));

	GeoDataPointmap::connect(m_mergeAction, SIGNAL(triggered()), m_parent, SLOT(mergePointmaps()));
}

void GeoDataPointmap::Impl::enablePointSelectedActions(bool enable)
{
	m_editPointsAction->setEnabled(enable);
	m_editPointsDeleteAction->setEnabled(enable);
	m_editPointsExportAction->setEnabled(enable);
	m_editPointsGreaterThanAction->setEnabled(enable);
	m_editPointsLessThanAction->setEnabled(enable);
}
