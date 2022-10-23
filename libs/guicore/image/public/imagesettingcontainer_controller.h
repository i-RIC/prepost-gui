#ifndef IMAGESETTINGCONTAINER_CONTROLLER_H
#define IMAGESETTINGCONTAINER_CONTROLLER_H

#include "../../guicore_global.h"
#include "../imagesettingcontainer.h"

#include <QPoint>

class GraphicsWindowDataItem;
class VTKGraphicsView;

class QMouseEvent;

class GUICOREDLL_EXPORT ImageSettingContainer::Controller
{
public:
	enum class MouseEventMode {
		Normal,
		MovePrepare,
		Move,
		ResizePrepare,
		Resize,
	};

	Controller(ImageSettingContainer* setting);

	MouseEventMode mouseEventMode() const;
	bool selected() const;

	GraphicsWindowDataItem* item() const;
	void setItem(GraphicsWindowDataItem* item);

	Position resizePosition() const;

	void handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate = false);
	void handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate = false);
	void handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate = false);
	void handleResize(VTKGraphicsView* v);

	void handleSelection(VTKGraphicsView* v);
	void handleDeselection(VTKGraphicsView* v);

	static void updateMouseCursor(VTKGraphicsView* v, const std::vector<ImageSettingContainer::Controller*>& controllers);

private:
	void updateMouseEventMode(QMouseEvent* event, VTKGraphicsView* v);
	void updateMouseCursor(VTKGraphicsView* v);

	MouseEventMode m_mouseEventMode;
	Position m_resizePosition;

	QPoint m_previousPosition;

	GraphicsWindowDataItem* m_item;
	bool m_selected;
	ImageSettingContainer* m_setting;
};

#endif // IMAGESETTINGCONTAINER_CONTROLLER_H
