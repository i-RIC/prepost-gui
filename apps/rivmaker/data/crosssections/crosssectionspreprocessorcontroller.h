#ifndef CROSSSECTIONSPREPROCESSORCONTROLLER_H
#define CROSSSECTIONSPREPROCESSORCONTROLLER_H

#include "crosssections.h"
#include "../base/dataitemcontroller.h"

class CrossSectionsPreProcessorController : public DataItemController
{
public:
	CrossSectionsPreProcessorController(Model* model, CrossSections* item);
	~CrossSectionsPreProcessorController();

	void addCrossSection();

	void rebuildStandardItemsAndViews();
};

#endif // CROSSSECTIONSPREPROCESSORCONTROLLER_H
