#ifndef POSTPROCESSORWINDOWFACTORYINTERFACE_H
#define POSTPROCESSORWINDOWFACTORYINTERFACE_H

#include "../guicore_global.h"

#include <QObject>
#include <QMap>

class QDomNode;
class QWidget;
class QXmlStreamWriter;
class ProjectDataItem;
class PostProcessorWindowProjectDataItem;

class GUICOREDLL_EXPORT PostProcessorWindowFactoryInterface
{
public:
	PostProcessorWindowFactoryInterface();

	virtual PostProcessorWindowProjectDataItem* factory(const QString& name, ProjectDataItem* parent, QWidget* parentwindow) = 0;
	virtual PostProcessorWindowProjectDataItem* restore(const QDomNode& node, ProjectDataItem* parent, QWidget* parentwindow) const = 0;
};

#endif // POSTPROCESSORWINDOWFACTORYINTERFACE_H
