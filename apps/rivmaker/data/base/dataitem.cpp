#include "dataitem.h"

#include <QStandardItem>

DataItem::DataItem(DataItem *parent) :
	QObject(parent),
	m_isDeletable {false}
{
	if (parent == nullptr) {return;}

	parent->m_childItems.push_back(this);
}

DataItem::~DataItem()
{
	clearChildItems();
	auto p = parentItem();
	if (p != nullptr) {p->unregisterChild(this);}
}
Project* DataItem::project() const
{
	return parentItem()->project();
}

bool DataItem::isDeletable() const
{
	return m_isDeletable;
}

void DataItem::setParent(DataItem* parent)
{
	QObject::setParent(parent);
	if (parent == nullptr) {return;}

	parent->m_childItems.push_back(this);
}

DataItem* DataItem::parentItem() const
{
	return dynamic_cast<DataItem*> (parent());
}

const std::vector<DataItem*>& DataItem::childItems() const
{
	return m_childItems;
}

std::vector<DataItem*>& DataItem::childItems()
{
	return m_childItems;
}

void DataItem::unregisterChild(DataItem* child)
{
	auto it = std::find(m_childItems.begin(), m_childItems.end(), child);
	if (it == m_childItems.end()) {return;}

	m_childItems.erase(it);
}

void DataItem::clearChildItems()
{
	for (int i = static_cast<int>(m_childItems.size()) - 1; i >= 0; --i) {
		delete m_childItems[i];
	}
	m_childItems.clear();
}

void DataItem::setDeletable(bool deletable)
{
	m_isDeletable = deletable;
}

void DataItem::setupStandardItem(QStandardItem* item)
{
	item->setCheckable(true);
	item->setCheckState(Qt::Checked);
	item->setEditable(false);
}
