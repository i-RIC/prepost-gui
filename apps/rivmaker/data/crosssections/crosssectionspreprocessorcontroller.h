#ifndef CROSSSECTIONSPREPROCESSORCONTROLLER_H
#define CROSSSECTIONSPREPROCESSORCONTROLLER_H

#include "crosssections.h"
#include "../base/dataitemcontroller.h"

class CrossSectionsPreProcessorController : public DataItemController
{
	Q_OBJECT

public:
	CrossSectionsPreProcessorController(Model* model, CrossSections* item);
	~CrossSectionsPreProcessorController();

	void setCrossSections(const std::vector<CrossSection*>& crossSections);
	void rebuildStandardItemsAndViews();

	void setupObjectBrowserRightClickMenu(QMenu* menu);

public slots:
	void importData();
	void exportData();

	void addCrossSection();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/crosssectionspreprocessorcontroller_impl.h"
#endif // _DEBUG

#endif // CROSSSECTIONSPREPROCESSORCONTROLLER_H
