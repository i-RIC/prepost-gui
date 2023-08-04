#ifndef IMAGESETTINGCONTAINER_CONTROLLER_H
#define IMAGESETTINGCONTAINER_CONTROLLER_H

#include "../../guicore_global.h"
#include "../imagesettingcontainer.h"

#include <QPoint>

#include <unordered_set>

class GraphicsWindowDataItem;
class VTKGraphicsView;

class QMouseEvent;
class QResizeEvent;

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

	GraphicsWindowDataItem* item() const;
	void setItem(GraphicsWindowDataItem* item);

	const std::unordered_set<GraphicsWindowDataItem*>& items() const;
	void addItem(GraphicsWindowDataItem* item);
	void removeItem(GraphicsWindowDataItem* item);

	Position resizePosition() const;

	void handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate = false);
	void handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate = false);
	void handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate = false);
	void handleResize(QResizeEvent* event, VTKGraphicsView* v);

	static void updateMouseCursor(VTKGraphicsView* v, const std::vector<ImageSettingContainer::Controller*>& controllers);

private:
	void updateMouseEventMode(QMouseEvent* event, VTKGraphicsView* v);
	void updateMouseCursor(VTKGraphicsView* v);
	GraphicsWindowDataItem* firstItem() const;

	MouseEventMode m_mouseEventMode;
	Position m_resizePosition;

	QPoint m_previousPosition;

	GraphicsWindowDataItem* m_item;
	std::unordered_set<GraphicsWindowDataItem*> m_items;
	ImageSettingContainer* m_setting;
};

#endif // IMAGESETTINGCONTAINER_CONTROLLER_H
