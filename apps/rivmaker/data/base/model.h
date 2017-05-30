#ifndef MODEL_H
#define MODEL_H

#include <QObject>

class DataItem;
class DataItemController;
class DataItemView;
class ObjectBrowserView;
class RootDataItem;
class View;

class QAction;
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

	void deleteItem(DataItem* item);

	void addStandardItem(DataItem* item, QStandardItem* sitem);
	void removeStandardItem(DataItem* item);

	template<typename T>
	QStandardItem* buildStandardItems(const T* item, QStandardItem* (T::*f)() const);
	void clearStandardItems();

	void addDataItemView(DataItem* item, DataItemView* view);
	void removeDataItemView(DataItem* item);

	template<typename T>
	DataItemView* buildDataItemViews(T* item, DataItemView* (T::*f)(Model*));
	void clearDataItemViews();

	void addDataItemController(DataItem* item, DataItemController* controller);
	void removeDataItemController(DataItem* item);

	template<typename T>
	DataItemController* buildDataItemControllers(T* item, DataItemController* (T::*f)(Model*));
	void clearDataItemControllers();

	QAction* deleteAction() const;

private slots:
	void handleObjectBrowserChange(QStandardItem* item);
	void handleObjectBrowserPress(const QModelIndex& index);

	void handleObjectBrowserSelection(const QModelIndex& current);

	virtual void deleteSelectedItem();

protected:
	ObjectBrowserView* objectBrowserView() const;

private:
	virtual RootDataItem* rootDataItem() const = 0;

	std::map<DataItem*, DataItemView*>& viewMap();
	std::map<DataItem*, DataItemController*>& controllerMap();

	std::map<DataItem*, QStandardItem*>& standardItemMap();
	std::map<QStandardItem*, DataItem*>& reverseStandardItemMap();

	class Impl;
	Impl* impl;
};

#include "model_detail.h"

#ifdef _DEBUG
	#include "private/model_impl.h"
#endif // _DEBUG

#endif // MODEL_H
