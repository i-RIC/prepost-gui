#ifndef GEODATAPOINTMAP_TINMANAGER_ADDBREAKLINECONTROLLER_H
#define GEODATAPOINTMAP_TINMANAGER_ADDBREAKLINECONTROLLER_H

#include "geodatapointmap_tinmanager.h"

#include <misc/mouseeventcontroller.h>

class GeoDataPointmap::TINManager::AddBreakLineController : public MouseEventController
{
public:
	AddBreakLineController(TINManager* manager);

	void setBreakLine(BreakLine* line);

	void activate(VTK2DGraphicsView* v) override;
	void deactivate(VTK2DGraphicsView* v) override;

	void handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;

private:
	void finish();

	QPixmap m_addPixmap;
	QCursor m_addCursor;

	vtkIdType m_currentVertex;
	BreakLine* m_breakLine;
	TINManager* m_manager;

	class AddPointCommand;
};

#endif // GEODATAPOINTMAP_TINMANAGER_ADDBREAKLINECONTROLLER_H
