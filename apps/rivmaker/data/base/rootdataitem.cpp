#include "rootdataitem.h"

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
	return new QStandardItem("Root Item");
}
