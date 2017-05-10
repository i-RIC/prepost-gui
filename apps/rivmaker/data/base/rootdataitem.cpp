#include "rootdataitem.h"
#include "../base/dataitemview.h"

#include<QStandardItem>

RootDataItem::RootDataItem(Project* project) :
	DataItem {},
	m_project {project}
{}

RootDataItem::~RootDataItem()
{}

Project* RootDataItem::project() const
{
	return m_project;
}

QStandardItem* RootDataItem::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem("Root Item");
	setupStandardItem(item);
	return item;
}

DataItemView* RootDataItem::buildPreProcessorDataItemView(Model *model)
{
	return new DataItemView(model, this);
}
