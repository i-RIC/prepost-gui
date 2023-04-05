#ifndef GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTCONTROLLER_H
#define GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTCONTROLLER_H

#include "geodatapointmap_pointsmanager.h"

#include <misc/mouseeventcontroller.h>

class GeoDataPointmap::PointsManager::AddPointController : public MouseEventController
{
public:
	enum class MouseEventMode {
		SelectExistingPointNotPossible,
		SelectExistingPointPossible,
		AddPoint,
	};

	AddPointController(PointsManager* manager);
	~AddPointController();

	void setZDepthForPoints(double z);

	void addActorsToRenderer(vtkRenderer* renderer) override;
	void removeActorsFromRenderer(vtkRenderer* renderer) override;

	void activate(VTK2DGraphicsView* v) override;
	void deactivate(VTK2DGraphicsView* v) override;

	void handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;

private:
	void updateMouseCursor(VTK2DGraphicsView* v);
	void finish();

	QPixmap m_addPixmap;
	QCursor m_addCursor;

	PolyLineController m_pointsController;
	vtkIdType m_existingPointId;
	double m_existingPointValue;

	MouseEventMode m_mouseEventMode;

	PointsManager* m_manager;

	class SetExistingPointCommand;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTCONTROLLER_H
