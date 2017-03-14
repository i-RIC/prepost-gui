#ifndef MODEL_DETAIL_H
#define MODEL_DETAIL_H

#include "model.h"
#include "dataitemview.h"

#include <QStandardItem>

template<typename T>
QStandardItem* Model::buildStandardItems(T* item, QStandardItem* (T::*f)())
{
	QStandardItem* sItem = (item->*f)();
	if (sItem == nullptr) {return nullptr;}

	impl->m_standardItemMap.insert(std::make_pair(item, sItem));
	impl->m_reverseStandardItemMap.insert(std::make_pair(sItem, item));

	for (DataItem* child : item->childItems) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		QStandardItem* childSItem = buildStandardItems(t, f);
		if (childSItem == nullptr) {continue;}

		sItem->appendRow(childSItem);
	}
	return sItem;
}

template<typename T>
DataItemView* Model::buildDataItemViews(T* item, DataItemView* (T::*f)())
{
	DataItemView* v = (item->*f)();
	if (v == nullptr) {return nullptr;}

	impl->m_viewMap.insert(std::make_pair(item, v));

	for (DataItem* child : item->childItems) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		DataItemView* childV = buildDataItemViews(t, f);
		if (childV == nullptr) {continue;}

		v->addChildItem(childV);
	}
	return v;
}

template<typename T>
DataItemController* Model::buildDataItemControllers(T* rootItem, DataItemController* (T::*f)())
{
	DataItemController* c = (item->*f)();
	if (c == nullptr) {return nullptr;}

	impl->m_controllerMap.insert(std::make_pair(item, c));

	for (DataItem* child : item->childItems) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		buildDataItemControllers(t, f);
	}
	return c;
}

#endif // MODEL_DETAIL_H
