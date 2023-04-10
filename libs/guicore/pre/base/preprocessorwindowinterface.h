#ifndef PREPROCESSORWINDOWINTERFACE_H
#define PREPROCESSORWINDOWINTERFACE_H

#include "../../guicore_global.h"
#include <QMainWindow>

class PreProcessorDataModelInterface;
class PropertyBrowser;

class QMenu;

class GUICOREDLL_EXPORT PreProcessorWindowInterface : public QMainWindow
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
