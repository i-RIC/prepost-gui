#ifndef MODEL_H
#define MODEL_H

#include <QObject>

class DataItem;
class DataItemController;
class DataItemView;
class ObjectBrowserView;
class RootDataItem;
class View;

class QStandardItem;
class QStandardItemModel;

class Model : public QObject
{
	Q_OBJECT

public:
	Model(QObject* parent);
	virtual ~Model();

	View* view() const;
	void setView(View* view);

	void setObjectBrowserView(ObjectBrowserView* obView);

	void showRightClickMenu(const QPoint& pos);

	void select(DataItem* item) const;

	bool isSelected(DataItem* item) const;
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
	DataItemView* buildDataItemViews(T* item, DataItemView* (T::*f)(Model*));

	template<typename T>
	DataItemController* buildDataItemControllers(T* item, DataItemController* (T::*f)(Model*));

private slots:
	void handleObjectBrowserChange(QStandardItem*);

	void handleObjectBrowserSelection(const QModelIndex& current);

private:
	virtual RootDataItem* rootDataItem() const = 0;

	class Impl;
	Impl* impl;
};

#include "model_detail.h"

#ifdef _DEBUG
	#include "private/model_impl.h"
#endif // _DEBUG

#endif // MODEL_H
