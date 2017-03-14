#include "crosssections.h"

#include <QStandardItem>

CrossSections::CrossSections(DataItem* parent) :
	DataItem {parent}
{}

CrossSections::~CrossSections()
{}

QStandardItem* CrossSections::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("Cross Sections"));
	setupStandardItem(item);
	return item;
}
