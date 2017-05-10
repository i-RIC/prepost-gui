#ifndef MODEL_DETAIL_H
#define MODEL_DETAIL_H

#include "model.h"
#include "dataitemview.h"
#include "rootdataitem.h"

#include <QStandardItem>

template<typename T>
QStandardItem* Model::buildStandardItems(const T* item, QStandardItem* (T::*f)() const)
{
	QStandardItem* sItem = (item->*f)();
	if (sItem == nullptr) {return nullptr;}

	const DataItem* cdItem = dynamic_cast<const DataItem*> (item);
	DataItem* dItem = const_cast<DataItem*> (cdItem);

	standardItemMap().insert(std::make_pair(dItem, sItem));
	reverseStandardItemMap().insert(std::make_pair(sItem, dItem));

	for (DataItem* child : cdItem->childItems()) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		QStandardItem* childSItem = buildStandardItems(t, f);
		if (childSItem == nullptr) {continue;}

		if (dynamic_cast<const RootDataItem*> (item) != nullptr) {continue;}

		sItem->appendRow(childSItem);
	}
	return sItem;
}

template<typename T>
DataItemView* Model::buildDataItemViews(T *item, DataItemView* (T::*f)(Model* model))
{
	DataItemView* v = (item->*f)(this);
	if (v == nullptr) {return nullptr;}

	DataItem* dItem = dynamic_cast<DataItem*> (item);

	viewMap().insert(std::make_pair(dItem, v));

	for (DataItem* child : dItem->childItems()) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		DataItemView* childV = buildDataItemViews(t, f);
		if (childV == nullptr) {continue;}

		v->addChildItem(childV);
	}
	return v;
}

template<typename T>
DataItemController* Model::buildDataItemControllers(T* item, DataItemController* (T::*f)(Model* model))
{
	DataItem* dItem = dynamic_cast<DataItem*> (item);

	DataItemController* c = (item->*f)(this);
	if (c != nullptr) {
		controllerMap().insert(std::make_pair(dItem, c));
	}

	for (DataItem* child : dItem->childItems()) {
		T* t = dynamic_cast<T*> (child);
		if (t == nullptr) {continue;}

		buildDataItemControllers(t, f);
	}
	return c;
}

#endif // MODEL_DETAIL_H
