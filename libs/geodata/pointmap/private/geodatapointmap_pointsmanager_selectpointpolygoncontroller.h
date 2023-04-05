#ifndef GEODATAPOINTMAP_POINTSMANAGER_SELECTPOINTPOLYGONCONTROLLER_H
#define GEODATAPOINTMAP_POINTSMANAGER_SELECTPOINTPOLYGONCONTROLLER_H

#include "geodatapointmap_pointsmanager.h"

#include <guibase/polygon/polygoncontroller.h>
#include <misc/mouseeventcontroller.h>

class GeoDataPointmap::PointsManager::SelectPointPolygonController : public MouseEventController
{
public:
	SelectPointPolygonController(PointsManager* manager);

	void setSelectionPolygonZDepth(double z);
	void cancelDefiningPolygon();

	virtual void addActorsToRenderer(vtkRenderer* renderer) override;
	virtual void removeActorsFromRenderer(vtkRenderer* renderer) override;

	void activate(VTK2DGraphicsView* v) override;
	void deactivate(VTK2DGraphicsView* v) override;

	void handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;

private:
	void finishDefiningPolygonAndSelectPoints(bool doubleClick, bool xOr);
	void updateMouseCursor(VTK2DGraphicsView* v);

	QPixmap m_addPixmap;
	QCursor m_addCursor;

	bool m_addMode;

	PolygonController m_polygonController;

	PointsManager* m_manager;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_SELECTPOINTPOLYGONCONTROLLER_H
