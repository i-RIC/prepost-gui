#include "../private/preprocessorgriddataitem_impl.h"
#include "preprocessorgriddataitem_selecteddatacontroller.h"

#include <guicore/datamodel/vtkgraphicsview.h>

PreProcessorGridDataItem::SelectedDataController::SelectedDataController(PreProcessorGridDataItem* item) :
	m_item {item}
{}

PreProcessorGridDataItem::SelectedDataController::~SelectedDataController()
{}

void PreProcessorGridDataItem::SelectedDataController::handleKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
		v->setCursor(m_item->impl->m_addCursor);
	} else {
		v->unsetCursor();
	}
}

void PreProcessorGridDataItem::SelectedDataController::handleKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
		v->setCursor(m_item->impl->m_addCursor);
	} else {
		v->unsetCursor();
	}
}
