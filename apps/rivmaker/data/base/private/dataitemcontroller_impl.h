#ifndef DATAITEMCONTROLLER_IMPL_H
#define DATAITEMCONTROLLER_IMPL_H

#include "../dataitemcontroller.h"

#include <QMenu>

class DataItemController::Impl
{
public:
	Impl(Model* model, DataItem* item);

	QMenu m_objectBrowserRightClickMenu;
	QMenu m_viewRightClickMenu;

	Model* m_model;
	DataItem* m_item;
};

#endif // DATAITEMCONTROLLER_IMPL_H
