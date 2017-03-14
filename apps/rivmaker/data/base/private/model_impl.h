#ifndef MODEL_IMPL_H
#define MODEL_IMPL_H

#include "../model.h"

#include <QStandardItemModel>

#include <map>

class QStandardItem;

class Model::Impl
{
public:
	Impl();

	DataItem* itemFromIndex(const QModelIndex& index);

	View* m_view;

	DataItem* m_selectedItem;

	std::map<DataItem*, DataItemView*> m_viewMap;
	std::map<DataItem*, DataItemController*> m_controllerMap;

	std::map<DataItem*, QStandardItem*> m_standardItemMap;
	std::map<QStandardItem*, DataItem*> m_reverseStandardItemMap;

	ObjectBrowserView* m_objectBrowserView;
	QStandardItemModel m_standardItemModel;
};

#endif // MODEL_IMPL_H
