#ifndef DATAITEMCONTROLLER_IMPL_H
#define DATAITEMCONTROLLER_IMPL_H

#include "../dataitemcontroller.h"

class DataItemController::Impl
{
public:
	Impl(Model* model, DataItem* item);

	Model* m_model;
	DataItem* m_item;
};

#endif // DATAITEMCONTROLLER_IMPL_H
