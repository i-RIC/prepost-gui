#ifndef PREPROCESSORWINDOWINTERFACE_H
#define PREPROCESSORWINDOWINTERFACE_H

#include "../../guicore_global.h"
#include <QMainWindow>

class PreProcessorDataModelInterface;
class PropertyBrowser;

class GUICOREDLL_EXPORT PreProcessorWindowInterface : public QMainWindow
{

public:
	/// constructor
	PreProcessorWindowInterface(QWidget* widget);
	/// destructor
	virtual ~PreProcessorWindowInterface();

	virtual PreProcessorDataModelInterface* dataModel() const = 0;
	virtual bool checkMappingStatus() = 0;
	virtual QString checkGrid(bool detail) = 0;
};

#endif // PREPROCESSORWINDOWINTERFACE_H
