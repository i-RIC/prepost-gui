#include "crosssectionspreprocessorcontroller.h"
#include "../crosssection/crosssection.h"
#include "../base/model.h"
#include "../../window/preprocessor/preprocessormodel.h"

CrossSectionsPreProcessorController::CrossSectionsPreProcessorController(Model* model, CrossSections* item) :
	DataItemController {model, item}
{}

CrossSectionsPreProcessorController::~CrossSectionsPreProcessorController()
{}

void CrossSectionsPreProcessorController::addCrossSection()
{
	auto crossSections = dynamic_cast<CrossSections*> (item());
	auto cs = crossSections->addCrossSection();

	dynamic_cast<PreProcessorModel*> (model())->setupStandardItemAndViewAndController(cs, crossSections);

	model()->select(cs);
}

void CrossSectionsPreProcessorController::rebuildStandardItemsAndViews()
{
	auto m = model();
	auto itm = item();
	auto mySItem = m->standardItem(itm);
	auto myView = m->dataItemView(itm);
	for (auto cs : itm->childItems()) {
		auto sItem = m->standardItem(cs);
		mySItem->takeRow(sItem->row());
		m->removeStandardItem(cs);

		myView->removeChildItem(m->dataItemView(cs));
		m->removeDataItemView(cs);
	}

	int id = 0;
	for (auto cs : item()->childItems()) {
		auto cs2 = dynamic_cast<CrossSection*> (cs);
		cs2->setId(id++);
		auto sItem = cs2->buildPreProcessorStandardItem();
		mySItem->appendRow(sItem);
		m->addStandardItem(cs, sItem);

		auto v = cs2->buildPreProcessorDataItemView(m);
		myView->addChildItem(v);
		m->addDataItemView(cs, v);
	}
}
