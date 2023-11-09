#ifndef POSTPROCESSORWINDOWFACTORYI_H
#define POSTPROCESSORWINDOWFACTORYI_H

#include "../guicore_global.h"

class QDomNode;
class QWidget;
class ProjectDataItem;
class PostProcessorWindowProjectDataItem;

class GUICOREDLL_EXPORT PostProcessorWindowFactoryI
{

public:
	PostProcessorWindowFactoryI();

	virtual PostProcessorWindowProjectDataItem* factory(const QString& name, ProjectDataItem* parent, QWidget* parentwindow) = 0;
	virtual PostProcessorWindowProjectDataItem* restore(const QDomNode& node, ProjectDataItem* parent, QWidget* parentwindow, bool ignoreIndex) = 0;
};

#endif // POSTPROCESSORWINDOWFACTORYI_H
