#ifndef GEODATAPOINTMAP_TINMANAGER_DELETEBREAKLINECONTROLLER_H
#define GEODATAPOINTMAP_TINMANAGER_DELETEBREAKLINECONTROLLER_H

#include "geodatapointmap_tinmanager.h"

#include <misc/mouseeventcontroller.h>

class GeoDataPointmap::TINManager::DeleteBreakLineController : public MouseEventController
{
public:
	DeleteBreakLineController(TINManager* manager);

	void activate(VTK2DGraphicsView* v) override;
	void deactivate(VTK2DGraphicsView* v) override;

	void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;

private:
	QPixmap m_removePixmap;
	QCursor m_removeCursor;

	BreakLine* m_deleteTarget;

	TINManager* m_manager;
};

#endif // GEODATAPOINTMAP_TINMANAGER_DELETEBREAKLINECONTROLLER_H
