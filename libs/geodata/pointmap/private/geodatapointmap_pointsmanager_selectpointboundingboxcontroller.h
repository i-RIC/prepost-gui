#ifndef GEODATAPOINTMAP_POINTSMANAGER_SELECTPOINTBOUNDINGBOXCONTROLLER_H
#define GEODATAPOINTMAP_POINTSMANAGER_SELECTPOINTBOUNDINGBOXCONTROLLER_H

#include "geodatapointmap_pointsmanager.h"

#include <misc/mouseeventcontroller.h>

class GeoDataPointmap::PointsManager::SelectPointBoundingBoxController : public MouseEventController
{
public:
	SelectPointBoundingBoxController(PointsManager* manager);
	~SelectPointBoundingBoxController();

	void activate(VTK2DGraphicsView * v) override;
	void deactivate(VTK2DGraphicsView* v) override;
	void handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;

private:
	void updateMouseCursor(VTK2DGraphicsView* v);

	QPixmap m_addPixmap;
	QCursor m_addCursor;

	bool m_definingBoundingBox;
	bool m_addMode;

	PointsManager* m_manager;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_SELECTPOINTBOUNDINGBOXCONTROLLER_H
