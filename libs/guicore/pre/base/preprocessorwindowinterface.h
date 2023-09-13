#ifndef PREPROCESSORWINDOWINTERFACE_H
#define PREPROCESSORWINDOWINTERFACE_H

#include "../../guicore_global.h"
#include "../../base/qmainwindowwithsnapshot.h"

class PreProcessorDataModelInterface;

class QMenu;

class GUICOREDLL_EXPORT PreProcessorWindowInterface : public QMainWindowWithSnapshot
{

public:
	PreProcessorWindowInterface(QWidget* widget);
	virtual ~PreProcessorWindowInterface();

	virtual QMenu* calcCondMenu() const = 0;
	virtual PreProcessorDataModelInterface* dataModel() const = 0;
	virtual bool checkMappingStatus() = 0;
	virtual void checkCalculationConditionImportSourceUpdate() = 0;
	virtual QString checkGrid(bool detail) = 0;
};

#endif // PREPROCESSORWINDOWINTERFACE_H
