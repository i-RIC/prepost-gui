#ifndef PREPROCESSORGRIDDATAITEM_SELECTEDDATACONTROLLER_H
#define PREPROCESSORGRIDDATAITEM_SELECTEDDATACONTROLLER_H

#include "../preprocessorgriddataitem.h"

class PreProcessorGridDataItem::SelectedDataController
{
public:
	SelectedDataController(PreProcessorGridDataItem* item);
	virtual ~SelectedDataController();

	virtual void handleDataUpdate() = 0;
	virtual void setVisibility(bool visible) = 0;
	virtual void handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* view) = 0;
	virtual void handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* view) = 0;
	virtual void handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* view) = 0;
	virtual void clearSelection() = 0;
	void handleKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v);
	void handleKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v);

protected:
	virtual void updateVtkObjects() = 0;

	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDDATAITEM_SELECTEDDATACONTROLLER_H
