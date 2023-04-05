#ifndef MOUSEEVENTCONTROLLER_H
#define MOUSEEVENTCONTROLLER_H

#include "misc_global.h"

class VTK2DGraphicsView;
class vtkRenderer;

class QKeyEvent;
class QMenu;
class QMouseEvent;
class QToolBar;

class MISCDLL_EXPORT MouseEventController
{
public:
	MouseEventController();
	virtual ~MouseEventController();

	virtual void addActorsToRenderer(vtkRenderer* renderer);
	virtual void removeActorsFromRenderer(vtkRenderer* renderer);

	virtual void addActionsToMenu(QMenu* menu);
	virtual void addActionsToToolBar(QToolBar* toolbar);

	virtual void activate(VTK2DGraphicsView* v);
	virtual void deactivate(VTK2DGraphicsView* v);

	virtual void handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v);
	virtual void handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v);
	virtual void handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v);
	virtual void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v);
	virtual void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v);
	virtual void handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v);
};

#endif // MOUSEEVENTCONTROLLER_H
