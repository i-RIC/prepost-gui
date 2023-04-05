#ifndef GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_H
#define GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_H

#include "geodatapointmap_pointsmanager.h"

#include <misc/mouseeventcontroller.h>

class GeoDataPointmap::PointsManager::InterpolatePointController : public MouseEventController
{
public:
	enum class MouseEventMode {
		SelectFirstPointNotPossible,
		SelectFirstPointPossible,
		AddExistingPoint,
		AddNewPoint,
	};

	InterpolatePointController(PointsManager* manager);
	~InterpolatePointController();

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
	std::vector<bool> m_newFlags;
	std::vector<double> m_values;

	MouseEventMode m_mouseEventMode;

	PointsManager* m_manager;

	class AddPointCommand;
	class SettingDialog;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_H
