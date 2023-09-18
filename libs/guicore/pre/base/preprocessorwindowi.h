#ifndef PREPROCESSORWINDOWI_H
#define PREPROCESSORWINDOWI_H

#include "../../guicore_global.h"
#include "../../base/qmainwindowwithsnapshot.h"

class PreProcessorDataModelI;

class QMenu;

class GUICOREDLL_EXPORT PreProcessorWindowI : public QMainWindowWithSnapshot
{

public:
	PreProcessorWindowI(QWidget* widget);
	virtual ~PreProcessorWindowI();

	virtual QMenu* calcCondMenu() const = 0;
	virtual PreProcessorDataModelI* dataModel() const = 0;
	virtual bool checkMappingStatus() = 0;
	virtual void checkCalculationConditionImportSourceUpdate() = 0;
	virtual QString checkGrid(bool detail) = 0;
};

#endif // PREPROCESSORWINDOWI_H
