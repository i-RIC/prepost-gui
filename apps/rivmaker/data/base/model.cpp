#include "model.h"
#include "private/model_impl.h"

#include <QStandardItem>

Model::Impl::Impl() :
	m_view {nullptr},
	m_selectedItem {nullptr}
{}

// public interfaces

Model::Model(QObject* parent) :
	QObject {parent},
	impl {new Impl {}}
{}

Model::~Model()
{
	delete impl;
}

View* Model::view() const
{
	return impl->m_view;
}

void Model::setView(View* view)
{
	impl->m_view = view;
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
