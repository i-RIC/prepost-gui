#ifndef GEODATAPOINTMAP_IMPL_H
#define GEODATAPOINTMAP_IMPL_H

#include "../geodatapointmap.h"

#include <QCursor>
#include <QPixmap>

#include "geodatapointmap_pointsmanager.h"

class GeoDataPointmap::Impl
{
public:
	Impl(GeoDataPointmap* pointmap);
	~Impl();

	void setupActions();
	void enablePointSelectedActions(bool enable);

	PointsManager m_pointsManager;

	// cursor related resources
	QPixmap m_addPixmap;
	QCursor m_addCursor;
	QPixmap m_removePixmap;
	QCursor m_removeCursor;
	QPixmap m_interpPointAddPixmap;
	QCursor m_interpPointAddCursor;
	QPixmap m_interpPointCtrlAddPixmap;
	QCursor m_interpPointCtrlAddCursor;

	// menu related resources
	QAction* m_selectionModePoint;
	QAction* m_selectionModePolygon;
	QAction* m_addPointAction;
	QAction* m_interpolatePointAction;
	QAction* m_removeTrianglesWithLongEdgeAction;
	QAction* m_displaySettingAction;

	QAction* m_editPointsAction;
	QAction* m_editPointsDeleteAction;
	QAction* m_editPointsExportAction;
	QAction* m_editPointsLessThanAction;
	QAction* m_editPointsGreaterThanAction;

	QAction* m_remeshAction;
	QAction* m_addBreakLineAction;
	QAction* m_removeBreakLineAction;
	QAction* m_removeAllBreakLinesAction;
	QAction* m_mergeAction;

	QMenu* m_rightClickingMenu;

private:
	GeoDataPointmap* m_parent;
};

#endif // GEODATAPOINTMAP_IMPL_H
