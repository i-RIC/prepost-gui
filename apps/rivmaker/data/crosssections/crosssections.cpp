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
	newS->setId(static_cast<int>(childItems().size() - 1));

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
	auto item = new QStandardItem(QIcon(":/images/iconFolder.png"), tr("Cross Sections"));
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
