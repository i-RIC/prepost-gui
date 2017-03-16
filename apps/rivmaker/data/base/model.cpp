#include "dataitemcontroller.h"
#include "../../window/preprocessor/objectbrowser/objectbrowserview.h"
#include "model.h"
#include "view.h"

#include "private/model_impl.h"

#include <QStandardItem>

Model::Impl::Impl() :
	m_view {nullptr},
	m_selectedItem {nullptr},
	m_objectBrowserView {nullptr}
{}

DataItem* Model::Impl::itemFromIndex(const QModelIndex& index)
{
	QStandardItem* i = m_standardItemModel.itemFromIndex(index);
	if (i == nullptr) {return nullptr;}

	auto it = m_reverseStandardItemMap.find(i);
	if (it == m_reverseStandardItemMap.end()) {return nullptr;}

	return it->second;
}

// public interfaces

Model::Model(QObject* parent) :
	QObject {parent},
	impl {new Impl {}}
{
	connect(&(impl->m_standardItemModel), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleObjectBrowserChange(QStandardItem*)));
}

Model::~Model()
{
	clearStandardItems();
	clearDataItemViews();
	clearDataItemControllers();

	delete impl;
}

View* Model::view() const
{
	return impl->m_view;
}

void Model::setView(View* view)
{
	impl->m_view = view;
	view->setModel(this);
}

void Model::setObjectBrowserView(ObjectBrowserView* obView)
{
	impl->m_objectBrowserView = obView;

	connect(obView, SIGNAL(itemSelected(QModelIndex)), this, SLOT(handleObjectBrowserSelection(QModelIndex)));
}

void Model::showRightClickMenu(const QPoint& pos)
{
	auto s = selectedItemController();
	if (s == nullptr) {return;}

	QMenu& menu = s->rightClickMenu();
	menu.move(pos);
	menu.show();
}

void Model::select(DataItem* item) const
{
	if (impl->m_objectBrowserView == nullptr) {return;}

	auto it = impl->m_standardItemMap.find(item);
	if (it == impl->m_standardItemMap.end()) {return;}

	QStandardItem* i = it->second;
	impl->m_objectBrowserView->select(i->index());
}

bool Model::isSelected(DataItem* item) const
{
	return item == selectedItem();
}

DataItem* Model::selectedItem() const
{
	return impl->m_selectedItem;
}

DataItemController *Model::selectedItemController() const
{
	auto s = selectedItem();
	if (s == nullptr) {return nullptr;}

	return dataItemController(s);
}

bool Model::shouldDraw(DataItem* item) const
{
	auto it = impl->m_standardItemMap.find(item);
	if (it == impl->m_standardItemMap.end()) {return false;}

	QStandardItem* i = it->second;

	while (i != nullptr) {
		if (i->checkState() == Qt::Unchecked) {return false;}
		i = i->parent();
	}

	return true;
}

QStandardItem* Model::standardItem(DataItem* item) const
{
	auto it = impl->m_standardItemMap.find(item);
	if (it == impl->m_standardItemMap.end()) {return nullptr;}

	return it->second;
}

DataItemView* Model::dataItemView(DataItem* item) const
{
	auto it = impl->m_viewMap.find(item);
	if (it == impl->m_viewMap.end()) {return nullptr;}

	return it->second;
}

DataItemController* Model::dataItemController(DataItem* item) const
{
	auto it = impl->m_controllerMap.find(item);
	if (it == impl->m_controllerMap.end()) {return nullptr;}

	return it->second;
}

DataItemView* Model::rootDataItemView() const
{
	return dataItemView(rootDataItem());
}

QStandardItemModel* Model::standardItemModel() const
{
	return &(impl->m_standardItemModel);
}

void Model::deleteItem(DataItem* item)
{
	removeStandardItem(item);
	removeDataItemView(item);
	removeDataItemController(item);

	delete item;
}

void Model::addStandardItem(DataItem* item, QStandardItem* sitem)
{
	impl->m_standardItemMap.insert(std::make_pair(item, sitem));
	impl->m_reverseStandardItemMap.insert(std::make_pair(sitem, item));
}

void Model::removeStandardItem(DataItem* item)
{
	auto it = impl->m_standardItemMap.find(item);
	if (it == impl->m_standardItemMap.end()) {return;}

	auto it2 = impl->m_reverseStandardItemMap.find(it->second);
	if (it2 != impl->m_reverseStandardItemMap.end()) {
		impl->m_reverseStandardItemMap.erase(it2);
	}
	QStandardItem* sItem = it->second;
	QStandardItem* parentSItem = sItem->parent();
	if (parentSItem!= nullptr) {
		parentSItem->takeRow(sItem->row());
	}
	delete it->second;
	impl->m_standardItemMap.erase(it);
}

void Model::clearStandardItems()
{
	impl->m_standardItemModel.clear();
	impl->m_standardItemMap.clear();
	impl->m_reverseStandardItemMap.clear();
}

void Model::addDataItemView(DataItem* item, DataItemView* view)
{
	impl->m_viewMap.insert(std::make_pair(item, view));
}

void Model::removeDataItemView(DataItem* item)
{
	auto it = impl->m_viewMap.find(item);
	if (it == impl->m_viewMap.end()) {return;}

	delete it->second;
	impl->m_viewMap.erase(it);
}

void Model::clearDataItemViews()
{
	for (auto pair : impl->m_viewMap) {
		delete pair.second;
	}
	impl->m_viewMap.clear();
}

void Model::addDataItemController(DataItem* item, DataItemController* controller)
{
	impl->m_controllerMap.insert(std::make_pair(item, controller));
}

void Model::removeDataItemController(DataItem* item)
{
	auto it = impl->m_controllerMap.find(item);
	if (it == impl->m_controllerMap.end()) {return;}

	delete it->second;
	impl->m_controllerMap.erase(it);
}

void Model::clearDataItemControllers()
{
	for (auto pair : impl->m_controllerMap) {
		delete pair.second;
	}
	impl->m_controllerMap.clear();
}

void Model::handleObjectBrowserChange(QStandardItem*)
{
	view()->update();
}

void Model::handleObjectBrowserSelection(const QModelIndex& current)
{
	auto item = impl->itemFromIndex(current);
	if (item == nullptr) {return;}
	impl->m_selectedItem = item;

	view()->update();
}
