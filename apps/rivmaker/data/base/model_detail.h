#ifndef MODEL_DETAIL_H
#define MODEL_DETAIL_H

#include "model.h"
#include "dataitemview.h"

#include <QStandardItem>

template<typename T>
QStandardItem* Model::buildStandardItems(const T* item, QStandardItem* (T::*f)() const)
{
	QStandardItem* sItem = (item->*f)();
	if (sItem == nullptr) {return nullptr;}

	const DataItem* cdItem = dynamic_cast<const DataItem*> (item);
	DataItem* dItem = const_cast<DataItem*> (cdItem);

	impl->m_standardItemMap.insert(std::make_pair(dItem, sItem));
	impl->m_reverseStandardItemMap.insert(std::make_pair(sItem, dItem));

	for (DataItem* child : cdItem->childItems()) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		QStandardItem* childSItem = buildStandardItems(t, f);
		if (childSItem == nullptr) {continue;}

		sItem->appendRow(childSItem);
	}
	return sItem;
}

template<typename T>
DataItemView* Model::buildDataItemViews(const T* item, DataItemView* (T::*f)() const)
{
	DataItemView* v = (item->*f)();
	if (v == nullptr) {return nullptr;}

	const DataItem* cdItem = dynamic_cast<const DataItem*> (item);
	DataItem* dItem = const_cast<DataItem*> (cdItem);

	impl->m_viewMap.insert(std::make_pair(dItem, v));

	for (DataItem* child : cdItem->childItems()) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		DataItemView* childV = buildDataItemViews(t, f);
		if (childV == nullptr) {continue;}

		v->addChildItem(childV);
	}
	return v;
}

template<typename T>
DataItemController* Model::buildDataItemControllers(const T* item, DataItemController* (T::*f)() const)
{
	DataItemController* c = (item->*f)();
	if (c == nullptr) {return nullptr;}

	const DataItem* cdItem = dynamic_cast<const DataItem*> (item);
	DataItem* dItem = const_cast<DataItem*> (cdItem);

	impl->m_controllerMap.insert(std::make_pair(dItem, c));

	for (DataItem* child : cdItem->childItems()) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		buildDataItemControllers(t, f);
	}
	return c;
}

#endif // MODEL_DETAIL_H
