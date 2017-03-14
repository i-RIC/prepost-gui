#ifndef MODEL_H
#define MODEL_H

#include <QObject>

class DataItem;
class DataItemController;
class DataItemView;
class View;

class QStandardItem;
class QStandardItemModel;

class Model : public QObject
{
public:
	Model(QObject* parent);
	virtual ~Model();

	View* view() const;
	void setView(View* view);

	DataItem* selectedItem() const;
	DataItemController* selectedItemController() const;

	bool shouldDraw(DataItem* item) const;

	QStandardItem* standardItem(DataItem* item) const;
	DataItemView* dataItemView(DataItem* item) const;
	DataItemController* dataItemController(DataItem* item) const;

	DataItemView* rootDataItemView() const;

	QStandardItemModel* standardItemModel() const;

	template<typename T>
	QStandardItem* buildStandardItems(const T* item, QStandardItem* (T::*f)() const);
	void clearStandardItems();

	template<typename T>
	DataItemView* buildDataItemViews(const T* item, DataItemView* (T::*f)() const);

	template<typename T>
	DataItemController* buildDataItemControllers(const T* item, DataItemController* (T::*f)() const);

private:
	class Impl;
	Impl* impl;
};

#include "model_detail.h"

#ifdef _DEBUG
	#include "private/model_impl.h"
#endif // _DEBUG

#endif // MODEL_H
