#ifndef CROSSSECTIONSPREPROCESSORCONTROLLER_H
#define CROSSSECTIONSPREPROCESSORCONTROLLER_H

#include "crosssections.h"
#include "../base/dataitemcontroller.h"

class CrossSectionsPreProcessorController : public DataItemController
{
public:
	CrossSectionsPreProcessorController(Model* model, CrossSections* item);
	~CrossSectionsPreProcessorController();

	void rebuildStandardItemsAndViews();

public slots:
	void addCrossSection();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/crosssectionspreprocessorcontroller_impl.h"
#endif // _DEBUG

#endif // CROSSSECTIONSPREPROCESSORCONTROLLER_H
