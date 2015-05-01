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
	~PreProcessorWindowInterface() {}

	virtual const PreProcessorDataModelInterface* dataModel() const = 0;
	virtual PreProcessorDataModelInterface* dataModel() = 0;
	virtual bool checkMappingStatus() = 0;
	virtual const QString checkGrid(bool detail) = 0;
};

#endif // PREPROCESSORWINDOWINTERFACE_H
