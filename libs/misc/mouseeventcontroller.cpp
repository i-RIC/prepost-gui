#include "mouseeventcontroller.h"

MouseEventController::MouseEventController()
{}

MouseEventController::~MouseEventController()
{}


void MouseEventController::addActorsToRenderer(vtkRenderer* /*renderer*/)
{}

void MouseEventController::removeActorsFromRenderer(vtkRenderer* /*renderer*/)
{}

void MouseEventController::addActionsToMenu(QMenu* /*menu*/)
{}

void MouseEventController::addActionsToToolBar(QToolBar* /*toolbar*/)
{}

void MouseEventController::activate(VTK2DGraphicsView* /*v*/)
{}

void MouseEventController::deactivate(VTK2DGraphicsView* /*v*/)
{}

void MouseEventController::handleKeyPressEvent(QKeyEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{}

void MouseEventController::handleKeyReleaseEvent(QKeyEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{}

void MouseEventController::handleMouseDoubleClickEvent(QMouseEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{}

void MouseEventController::handleMouseMoveEvent(QMouseEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{}

void MouseEventController::handleMousePressEvent(QMouseEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{}

void MouseEventController::handleMouseReleaseEvent(QMouseEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{}
