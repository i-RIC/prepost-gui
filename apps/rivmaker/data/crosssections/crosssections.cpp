#include "crosssections.h"
#include "crosssectionspreprocessorcontroller.h"
#include "../base/dataitemview.h"
#include "../crosssection/crosssection.h"

#include "private/crosssections_impl.h"

#include <QStandardItem>

CrossSections::CrossSections(DataItem* parent) :
	DataItem {parent},
	impl {new Impl {}}
{}

CrossSections::~CrossSections()
{
	delete impl;
}

CrossSection* CrossSections::addCrossSection()
{
	auto newS = new CrossSection(this);
	childItems().push_back(newS);

	return newS;
}

std::vector<CrossSection*> CrossSections::crossSectionVector() const
{
	std::vector<CrossSection*> ret;
	for (auto c : childItems()) {
		ret.push_back(dynamic_cast<CrossSection*> (c));
	}
	return ret;
}

QStandardItem* CrossSections::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("Cross Sections"));
	setupStandardItem(item);
	return item;
}

DataItemController* CrossSections::buildPreProcessorDataItemController(Model* model)
{
	return new CrossSectionsPreProcessorController(model, this);
}

DataItemView* CrossSections::buildPreProcessorDataItemView(Model* model)
{
	return new DataItemView(model, this);
}
