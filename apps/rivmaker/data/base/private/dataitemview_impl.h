#ifndef DATAITEMVIEW_IMPL_H
#define DATAITEMVIEW_IMPL_H

#include "../dataitemview.h"

#include <vector>

class DataItemView::Impl
{
public:
	Impl(Model* model, DataItem* item);
	~Impl();

	Model* m_model;
	DataItem* m_item;
	DataItemView* m_parentView;

	std::vector<DataItemViewHelperI*> m_viewHelpers;

	std::vector<DataItemView*> m_childItems;
};

#endif // DATAITEMVIEW_IMPL_H
