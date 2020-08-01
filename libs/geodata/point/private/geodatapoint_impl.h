#ifndef GEODATAPOINT_IMPL_H
#define GEODATAPOINT_IMPL_H

#include "../geodatapoint.h"

#include <guibase/point/pointcontroller.h>

class GeoDataPoint::Impl
{
public:
	Impl(GeoDataPoint* parent);
	~Impl();

	GeoDataPoint* m_parent;
	PointController m_pointController;

	MouseEventMode m_mouseEventMode;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	QMenu* m_rightClickingMenu;

	QAction* m_coordEditAction;
};

#endif // GEODATAPOINT_IMPL_H
